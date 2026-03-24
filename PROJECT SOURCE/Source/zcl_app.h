#ifndef ZCL_APP_H
#define ZCL_APP_H

#ifdef __cplusplus
extern "C" {
#endif

/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "zcl.h"

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

// Application Events
#define APP_REPORT_EVT                                                          0x0001
#define APP_READ_SENSORS_EVT                                                    0x0002
#define APP_SAVE_ATTRS_EVT                                                      0x0004
#define APP_READ_SEND_BATT_EVT                                                  0x0008
#define APP_REPORT_BATT_EVT                                                     0x0010
#define APP_LED_EVT                                                             0x0020
#define APP_TIME_EVT                                                            0x0040

// Network application configuration NV item ID
#define NW_APP_CONFIG                                                           0x0401

// Access control macros for ZCL attributes
#define R                                                                       ACCESS_CONTROL_READ
#define RW                                                                      (R | ACCESS_CONTROL_WRITE | ACCESS_CONTROL_AUTH_WRITE)
#define RR                                                                      (R | ACCESS_REPORTABLE)
#define RRW                                                                     (R | ACCESS_REPORTABLE | ACCESS_CONTROL_WRITE | ACCESS_CONTROL_AUTH_WRITE)

// ZCL Cluster IDs
#define BASIC                                                                   ZCL_CLUSTER_ID_GEN_BASIC
#define POWER_CFG                                                               ZCL_CLUSTER_ID_GEN_POWER_CFG
#define IDENTIFY                                                                ZCL_CLUSTER_ID_GEN_IDENTIFY
#define GEN_TIME                                                                ZCL_CLUSTER_ID_GEN_TIME 
#define TEMP                                                                    ZCL_CLUSTER_ID_MS_TEMPERATURE_MEASUREMENT
#define HUMIDITY                                                                ZCL_CLUSTER_ID_MS_RELATIVE_HUMIDITY
#define ILLUMINANCE                                                             ZCL_CLUSTER_ID_MS_ILLUMINANCE_MEASUREMENT
#define SOIL_HUMIDITY                                                           0x0408

// Custom Attribute IDs
#define ATTRID_ReportDelay                                                      0x0201
#define ATTRID_MS_ILLUMINANCE_COOF                                              0x0310
#define ATTRID_MODE1                                                            0x0504
#define ATTRID_TEMP_RES                                                         0x0520
#define ATTRID_ComparisonPreviousData3                                          0x0216
#define ATTRID_TXPOWER                                                          0x0236

// ZCL Data Types (aliases for convenience)
#define ZCL_UINT8                                                               ZCL_DATATYPE_UINT8
#define ZCL_UINT16                                                              ZCL_DATATYPE_UINT16
#define ZCL_INT16                                                               ZCL_DATATYPE_INT16
#define ZCL_INT8                                                                ZCL_DATATYPE_INT8
#define ZCL_INT32                                                               ZCL_DATATYPE_INT32
#define ZCL_UINT32                                                              ZCL_DATATYPE_UINT32
#define ZCL_SINGLE                                                              ZCL_DATATYPE_SINGLE_PREC

/*********************************************************************
 * TYPEDEFS
 *********************************************************************/

// Application Configuration Structure
typedef struct {
    uint32 ReportDelay;                 // Report interval in minutes
    bool mode1;                         // Temperature compensation mode
    uint8 Resolution;                   // DS18B20 resolution (9-12 bits)
    uint32 localTime;                   // Local time counter
    bool ComparisonPreviousData3;       // Enable threshold-based reporting
    int8 txPower;                       // Transmit power level (4, 0, -4 dBm)
    int16 illumCoof;                    // Illuminance correction coefficient
} application_config_t;

/*********************************************************************
 * VARIABLES
 *********************************************************************/

extern SimpleDescriptionFormat_t zclApp_FirstEP;

// Basic Cluster Attributes
extern uint8 zclLocationDescription[17];
extern uint8 zclPhysicalEnvironment;
extern uint8 zclBS;
extern uint8 zclBQ;
extern uint16 zclApp_IdentifyTime;
extern int16 sendInitReportCount;

// Battery Status
extern uint8 zclBattery_Voltage;
extern uint8 zclBattery_PercentageRemainig;
extern uint16 zclBattery_RawAdc;

// Sensor Values
extern int16 zclApp_Temperature_Sensor_MeasuredValue;
extern uint16 zclApp_IlluminanceSensor_MeasuredValue;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValue;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValue_old;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValueTr;
extern uint16 zclApp_SoilHumiditySensor_MeasuredValueRawAdc;

// Attribute Lists
extern CONST zclAttrRec_t zclApp_AttrsFirstEP[];
extern CONST uint8 zclApp_AttrsFirstEPCount;

// Basic Cluster Attributes
extern const uint8 zclApp_ManufacturerName[];
extern const uint8 zclApp_ModelId[];
extern const uint8 zclApp_PowerSource;

// Application Configuration
extern application_config_t zclApp_Config;

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

// Initialization for the task
extern void zclApp_Init(byte task_id);

// Event processing loop for the task
extern UINT16 zclApp_event_loop(byte task_id, UINT16 events);

// Reset ZCL attributes to default values
extern void zclResetAttributesToDefaultValues(void);

#ifdef __cplusplus
}
#endif

#endif /* ZCL_APP_H */