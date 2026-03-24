#ifndef FACTORY_RESET_H
#define FACTORY_RESET_H

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

// Factory Reset Event IDs
#define FACTORY_RESET_EVT                0x1000
#define FACTORY_BOOTCOUNTER_RESET_EVT    0x2000
#define FACTORY_LED_EVT                  0x4000
#define FACTORY_LEDOFF_EVT               0x8000

/*********************************************************************
 * Factory Reset Timing Configuration
 *********************************************************************/

#ifndef FACTORY_RESET_HOLD_TIME_LONG
    #define FACTORY_RESET_HOLD_TIME_LONG ((uint32)9 * 1000)   // 9 seconds for long press when on network
#endif

#ifndef FACTORY_RESET_HOLD_TIME_FAST
    #define FACTORY_RESET_HOLD_TIME_FAST 2000                  // 2 seconds for fast press when not on network
#endif

#ifndef FACTORY_RESET_BOOTCOUNTER_MAX_VALUE
    #define FACTORY_RESET_BOOTCOUNTER_MAX_VALUE 5              // Number of boot failures to trigger reset
#endif

#ifndef FACTORY_RESET_BOOTCOUNTER_RESET_TIME
    #define FACTORY_RESET_BOOTCOUNTER_RESET_TIME 10 * 1000     // 10 seconds to reset boot counter
#endif

#ifndef FACTORY_RESET_BY_LONG_PRESS
    #define FACTORY_RESET_BY_LONG_PRESS TRUE                   // Enable factory reset by long press
#endif

#ifndef FACTORY_RESET_BY_LONG_PRESS_PORT
    #define FACTORY_RESET_BY_LONG_PRESS_PORT 0x00              // Port mask for long press detection
#endif

#ifndef FACTORY_RESET_BY_BOOT_COUNTER
    #define FACTORY_RESET_BY_BOOT_COUNTER TRUE                 // Enable factory reset by boot counter
#endif

/*********************************************************************
 * FUNCTION PROTOTYPES
 *********************************************************************/

extern void zclFactoryResetter_Init(uint8 task_id);
extern uint16 zclFactoryResetter_loop(uint8 task_id, uint16 events);
extern void zclFactoryResetter_HandleKeys(uint8 portAndAction, uint8 keyCode);

#endif /* FACTORY_RESET_H */