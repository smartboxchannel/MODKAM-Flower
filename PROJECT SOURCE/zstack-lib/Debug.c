/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "Debug.h"
#include "DebugTrace.h"
#include "MT.h"
#include "OSAL.h"
#include "OSAL_Memory.h"

/*********************************************************************
 * @fn      vprint
 * @brief   Format and print a string via va_list
 *********************************************************************/

void vprint(const char *fmt, va_list argp)
{
    uint8 string[100];
    if (0 < vsprintf((char *)string, fmt, argp)) // Build string
    {
        LREPMaster(string);
    }
}

/*********************************************************************
 * UART Debug Output
 *********************************************************************/

#ifdef DO_DEBUG_UART

#define UART_PORT HAL_UART_PORT_0

/*********************************************************************
 * @fn      DebugInit
 * @brief   Initialize debug output over UART
 *********************************************************************/

bool DebugInit()
{
    halUARTCfg_t halUARTConfig;
    halUARTConfig.configured = TRUE;
    halUARTConfig.baudRate = HAL_UART_BR_115200;
    halUARTConfig.flowControl = FALSE;
    halUARTConfig.flowControlThreshold = 48; // Number of bytes left before Rx Buffer reaches maxRxBufSize
    halUARTConfig.idleTimeout = 10;          // RX timeout period in milliseconds
    halUARTConfig.rx.maxBufSize = 0;
    halUARTConfig.tx.maxBufSize = BUFFLEN;
    halUARTConfig.intEnable = TRUE;
    halUARTConfig.callBackFunc = NULL;
    HalUARTInit();
    
    if (HalUARTOpen(UART_PORT, &halUARTConfig) == HAL_UART_SUCCESS) {
        LREPMaster("Initialized debug module \r\n");
        return true;
    }
    return false;
}

/*********************************************************************
 * @fn      LREPMaster
 * @brief   Write data to UART debug output
 *********************************************************************/

void LREPMaster(uint8 *data)
{
    if (data == NULL) {
        return;
    }
    HalUARTWrite(UART_PORT, data, osal_strlen((char *)data));
}

/*********************************************************************
 * @fn      LREP
 * @brief   Print formatted debug message
 *********************************************************************/

void LREP(char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    vprint(format, argp);
    va_end(argp);
}

/*********************************************************************
 * MT Debug Output
 *********************************************************************/

#elif defined(DO_DEBUG_MT)

/*********************************************************************
 * @fn      DebugInit
 * @brief   Initialize debug output over MT (Monitor & Test)
 *********************************************************************/

bool DebugInit()
{
    debugThreshold = 0x04; // Increase threshold as soon as we initialize debug module
    LREPMaster("Initialized debug module \r\n");
    return TRUE;
}

/*********************************************************************
 * @fn      LREP
 * @brief   Print formatted debug message via MT
 *********************************************************************/

void LREP(char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    vprint(format, argp);
    va_end(argp);
}

/*********************************************************************
 * @fn      LREPMaster
 * @brief   Write data to MT debug output
 *********************************************************************/

void LREPMaster(uint8 *data)
{
    debug_str(data);
}

/*********************************************************************
 * Default (No Debug) Output
 *********************************************************************/

#else

/*********************************************************************
 * @fn      DebugInit
 * @brief   Stub debug initialization (no output)
 *********************************************************************/

bool DebugInit()
{
    return true;
}

/*********************************************************************
 * @fn      LREP
 * @brief   Stub for formatted debug message (no output)
 *********************************************************************/

void LREP(char *format, ...)
{
    va_list argp;
    va_start(argp, format);
    printf(format, argp);
    va_end(argp);
}

/*********************************************************************
 * @fn      LREPMaster
 * @brief   Stub for debug output (no output)
 *********************************************************************/

void LREPMaster(uint8 *data)
{
    printf((const char*)data);
}

#endif /* DO_DEBUG_UART */