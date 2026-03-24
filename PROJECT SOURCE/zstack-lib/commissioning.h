#ifndef COMMISSIONING_H
#define COMMISSIONING_H

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

// Commissioning Event IDs
#define APP_COMMISSIONING_CLOCK_DOWN_POLING_RATE_EVT  0x0001
#define APP_COMMISSIONING_END_DEVICE_REJOIN_EVT       0x0002
#define APP_COMMISSIONING_BY_LONG_PRESS_EVT           0x0004
#define APP_COMMISSIONING_OFF_EVT                     0x0008

// End Device Rejoin Configuration
#define APP_COMMISSIONING_END_DEVICE_REJOIN_MAX_DELAY ((uint32)900000)    // Maximum rejoin delay (15 minutes)
#define APP_COMMISSIONING_END_DEVICE_REJOIN_START_DELAY 10 * 1000          // Initial rejoin delay (10 seconds)
#define APP_COMMISSIONING_END_DEVICE_REJOIN_BACKOFF ((float) 1.2)          // Exponential backoff factor
#define APP_COMMISSIONING_END_DEVICE_REJOIN_TRIES 24                       // Maximum rejoin attempts

// Long Press Commissioning Configuration
#ifndef APP_COMMISSIONING_BY_LONG_PRESS
    #define APP_COMMISSIONING_BY_LONG_PRESS FALSE
#endif

#ifndef APP_COMMISSIONING_BY_LONG_PRESS_PORT
    #define APP_COMMISSIONING_BY_LONG_PRESS_PORT 0x00
#endif

#ifndef APP_COMMISSIONING_HOLD_TIME_FAST
    #define APP_COMMISSIONING_HOLD_TIME_FAST 1500      // Hold time for fast commissioning (1.5 seconds)
#endif

/*********************************************************************
 * FUNCTION PROTOTYPES
 *********************************************************************/

extern void zclCommissioning_Init(uint8 task_id);
extern uint16 zclCommissioning_event_loop(uint8 task_id, uint16 events);
extern void zclCommissioning_Sleep(uint8 allow);
extern void zclCommissioning_HandleKeys(uint8 portAndAction, uint8 keyCode);
extern bool zclApp_onNet;

#endif /* COMMISSIONING_H */