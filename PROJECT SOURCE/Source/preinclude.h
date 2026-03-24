/*********************************************************************
 * POWER MANAGEMENT
 *********************************************************************/
#define POWER_SAVING

/*********************************************************************
 * NETWORK CONFIGURATION
 *********************************************************************/
#define TC_LINKKEY_JOIN
#define NV_INIT
#define NV_RESTORE
#define TP2_LEGACY_ZC
#define NWK_AUTO_POLL
#define MULTICAST_ENABLED                                                       FALSE

/*********************************************************************
 * ZCL FEATURES
 *********************************************************************/
#define ZCL_READ
#define ZCL_WRITE
#define ZCL_BASIC
#define ZCL_IDENTIFY
#define ZCL_REPORTING_DEVICE

/*********************************************************************
 * DEVICE TYPE
 *********************************************************************/
#define ZSTACK_DEVICE_BUILD                                                     (DEVICE_BUILD_ENDDEVICE)

/*********************************************************************
 * GREEN POWER
 *********************************************************************/
#define DISABLE_GREENPOWER_BASIC_PROXY

/*********************************************************************
 * BDB (Base Device Behavior) CONFIGURATION
 *********************************************************************/
#define BDB_FINDING_BINDING_CAPABILITY_ENABLED                                  1
#define BDB_MAX_CLUSTERENDPOINTS_REPORTING                                      10

/*********************************************************************
 * MONITOR & TEST
 *********************************************************************/
#define MT_ZDO_MGMT

/*********************************************************************
 * HARDWARE CONFIGURATION
 *********************************************************************/
#define ISR_KEYINTERRUPT
#define HAL_BUZZER                                                              FALSE

// Luminosity sensor (photoresistor) - P0.7
#define LUMOISITY_PORT                                                          0
#define LUMOISITY_PIN                                                           7

// Soil moisture sensor - P0.4
#define SOIL_MOISTURE_PORT                                                      0
#define SOIL_MOISTURE_PIN                                                       4

// Factory reset button - Port 2
#define FACTORY_RESET_BY_LONG_PRESS_PORT                                        0x04
#define APP_COMMISSIONING_BY_LONG_PRESS                                         TRUE
#define APP_COMMISSIONING_BY_LONG_PRESS_PORT                                    0x04

// DS18B20 temperature sensor - P1.3
#define DS18B20_PORT                                                            1
#define DS18B20_PIN                                                             3
#define ONEWIRE2_SBIT                                                           P1_3
#define ONEWIRE2_BV                                                             BV(3)
#define ONEWIRE2_DIR                                                            P1DIR

// Key input configuration
#define HAL_KEY_P2_INPUT_PINS                                                   BV(0)

/*********************************************************************
 * HEAP CONFIGURATION
 *********************************************************************/
#define INT_HEAP_LEN                                                            2600

/*********************************************************************
 * INCLUDES
 *********************************************************************/
#include "hal_board_cfg.h"
#include "stdint.h"
#include "stddef.h"