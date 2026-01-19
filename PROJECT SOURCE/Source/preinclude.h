#define POWER_SAVING

#define TC_LINKKEY_JOIN
#define NV_INIT
#define NV_RESTORE
#define TP2_LEGACY_ZC
#define NWK_AUTO_POLL
#define MULTICAST_ENABLED                                                       FALSE
#define ZCL_READ
#define ZCL_WRITE
#define ZCL_BASIC
#define ZCL_IDENTIFY
#define ZCL_REPORTING_DEVICE
#define ZSTACK_DEVICE_BUILD                                                     (DEVICE_BUILD_ENDDEVICE)
#define DISABLE_GREENPOWER_BASIC_PROXY
#define BDB_FINDING_BINDING_CAPABILITY_ENABLED                                  1
#define BDB_MAX_CLUSTERENDPOINTS_REPORTING                                      10
#define MT_ZDO_MGMT

#define ISR_KEYINTERRUPT
#define HAL_BUZZER                                                              FALSE

#define LUMOISITY_PORT                                                          0
#define LUMOISITY_PIN                                                           7
#define SOIL_MOISTURE_PORT                                                      0
#define SOIL_MOISTURE_PIN                                                       4
#define FACTORY_RESET_BY_LONG_PRESS_PORT                                        0x04 //port2
#define APP_COMMISSIONING_BY_LONG_PRESS TRUE
#define APP_COMMISSIONING_BY_LONG_PRESS_PORT                                    0x04 //port2
#define DS18B20_PORT                                                            1
#define DS18B20_PIN                                                             3
#define ONEWIRE2_SBIT                                                           P1_3
#define ONEWIRE2_BV                                                             BV(3)
#define ONEWIRE2_DIR                                                            P1DIR
#define HAL_KEY_P2_INPUT_PINS                                                   BV(0)

#define INT_HEAP_LEN                                                            2600

#include "hal_board_cfg.h"
#include "stdint.h"
#include "stddef.h"
