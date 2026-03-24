/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "factory_reset.h"
#include "AF.h"
#include "OnBoard.h"
#include "bdb.h"
#include "bdb_interface.h"
#include "ZComDef.h"
#include "hal_key.h"
#include "zcl_app.h"

/*********************************************************************
 * LOCAL FUNCTIONS PROTOTYPES
 *********************************************************************/

static void zclFactoryResetter_ResetToFN(void);
static void zclFactoryResetter_ProcessBootCounter(void);
static void zclFactoryResetter_ResetBootCounter(void);
static void zclApp_LedOnF(void);
static void zclApp_LedOffF(void);

/*********************************************************************
 * LOCAL VARIABLES
 *********************************************************************/

static uint8 zclFactoryResetter_TaskID;

/*********************************************************************
 * @fn      zclFactoryResetter_loop
 * @brief   Factory reset event processing loop
 *********************************************************************/

uint16 zclFactoryResetter_loop(uint8 task_id, uint16 events)
{
    if (events & FACTORY_RESET_EVT) {
        zclFactoryResetter_ResetToFN();
        return (events ^ FACTORY_RESET_EVT);
    }

    if (events & FACTORY_BOOTCOUNTER_RESET_EVT) {
        zclFactoryResetter_ResetBootCounter();
        return (events ^ FACTORY_BOOTCOUNTER_RESET_EVT);
    }
    
    if (events & FACTORY_LED_EVT) {
        zclApp_LedOnF();
        return (events ^ FACTORY_LED_EVT);
    }
    
    if (events & FACTORY_LEDOFF_EVT) {
        zclApp_LedOffF();
        return (events ^ FACTORY_LEDOFF_EVT);
    }
    
    return 0;
}

/*********************************************************************
 * @fn      zclFactoryResetter_ResetBootCounter
 * @brief   Reset the boot counter in NV memory
 *********************************************************************/

void zclFactoryResetter_ResetBootCounter(void)
{
    uint16 bootCnt = 0;
    osal_nv_write(ZCD_NV_BOOTCOUNTER, 0, sizeof(bootCnt), &bootCnt);
}

/*********************************************************************
 * @fn      zclFactoryResetter_Init
 * @brief   Initialize factory reset module
 *********************************************************************/

void zclFactoryResetter_Init(uint8 task_id)
{
    zclFactoryResetter_TaskID = task_id;
    /**
     * We can't register more than one task, call in main app task
     * zclFactoryResetter_HandleKeys(portAndAction, keyCode);
     */
    // RegisterForKeys(task_id);
    
#if FACTORY_RESET_BY_BOOT_COUNTER
    zclFactoryResetter_ProcessBootCounter();
#endif
}

/*********************************************************************
 * @fn      zclFactoryResetter_ResetToFN
 * @brief   Perform factory reset to factory new state
 *********************************************************************/

void zclFactoryResetter_ResetToFN(void)
{
    osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_LED_EVT);
    osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_LEDOFF_EVT);
    zclResetAttributesToDefaultValues();
    bdb_resetLocalAction();
}

/*********************************************************************
 * @fn      zclFactoryResetter_HandleKeys
 * @brief   Handle key presses for factory reset
 *********************************************************************/

void zclFactoryResetter_HandleKeys(uint8 portAndAction, uint8 keyCode)
{
#if FACTORY_RESET_BY_LONG_PRESS
    if (portAndAction & HAL_KEY_RELEASE) {
        // Cancel factory reset on key release
        osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_RESET_EVT);
        osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_LED_EVT);
        osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_LEDOFF_EVT);
        HAL_TURN_OFF_LED1();
    } else {
        bool statTimer = true;
#if FACTORY_RESET_BY_LONG_PRESS_PORT
        statTimer = FACTORY_RESET_BY_LONG_PRESS_PORT & portAndAction;
#endif
        if (statTimer) {
            // Start factory reset timer with appropriate delay
            uint32 timeout = bdbAttributes.bdbNodeIsOnANetwork ? 
                             FACTORY_RESET_HOLD_TIME_LONG : 
                             FACTORY_RESET_HOLD_TIME_FAST;
            osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_RESET_EVT, timeout);
            osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_LED_EVT, 2000);
        }
    }
#endif
}

/*********************************************************************
 * @fn      zclFactoryResetter_ProcessBootCounter
 * @brief   Process boot counter to detect boot failures
 *********************************************************************/

void zclFactoryResetter_ProcessBootCounter(void)
{
    // Schedule boot counter reset timer
    osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_BOOTCOUNTER_RESET_EVT, 
                       FACTORY_RESET_BOOTCOUNTER_RESET_TIME);

    uint16 bootCnt = 0;
    
    // Read current boot counter
    if (osal_nv_item_init(ZCD_NV_BOOTCOUNTER, sizeof(bootCnt), &bootCnt) == ZSUCCESS) {
        osal_nv_read(ZCD_NV_BOOTCOUNTER, 0, sizeof(bootCnt), &bootCnt);
    }
    
    bootCnt += 1;
    
    // Check if maximum boot failures reached
    if (bootCnt >= FACTORY_RESET_BOOTCOUNTER_MAX_VALUE) {
        bootCnt = 0;
        osal_stop_timerEx(zclFactoryResetter_TaskID, FACTORY_BOOTCOUNTER_RESET_EVT);
        osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_RESET_EVT, 5000);
    }
    
    // Store updated boot counter
    osal_nv_write(ZCD_NV_BOOTCOUNTER, 0, sizeof(bootCnt), &bootCnt);
}

/*********************************************************************
 * @fn      zclApp_LedOnF
 * @brief   Turn LED on and schedule turn off
 *********************************************************************/

static void zclApp_LedOnF(void)
{
    HAL_TURN_ON_LED1();
    osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_LEDOFF_EVT, 50);
}

/*********************************************************************
 * @fn      zclApp_LedOffF
 * @brief   Turn LED off and schedule next blink
 *********************************************************************/

static void zclApp_LedOffF(void)
{
    HAL_TURN_OFF_LED1();
    osal_start_timerEx(zclFactoryResetter_TaskID, FACTORY_LED_EVT, 1000);
}