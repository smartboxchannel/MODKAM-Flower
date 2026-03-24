/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "AF.h"
#include "OSAL.h"
#include "ZComDef.h"
#include "ZDConfig.h"
#include "zcl.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "zcl_ha.h"
#include "zcl_app.h"

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

#define APP_DEVICE_VERSION                                                      2
#define APP_FLAGS                                                               0
#define APP_HWVERSION                                                           1
#define APP_ZCLVERSION                                                          1
#define BATTERY_QUANTITY                                                        2
#define APP_REPORT_DELAY                                                        3

/*********************************************************************
 * GLOBAL VARIABLES
 *********************************************************************/

// Cluster revision
const uint16 zclApp_clusterRevision_all = 0x0001;

// Basic Cluster attributes
const uint8 zclApp_HWRevision = APP_HWVERSION;
const uint8 zclApp_ZCLVersion = APP_ZCLVERSION;
const uint8 zclApp_ApplicationVersion = 3;
const uint8 zclApp_StackVersion = 4;

// Power Configuration attributes
uint8 zclBS = BAT_SIZE_AAA;
uint8 zclBQ = BATTERY_QUANTITY;

// Identify Cluster attribute
uint16 zclApp_IdentifyTime = 0;

// Sensor measurement values
int16 zclApp_Temperature_Sensor_MeasuredValue = 0;
uint16 zclApp_IlluminanceSensor_MeasuredValue = 0;

// Soil humidity sensor values
uint16 zclApp_SoilHumiditySensor_MeasuredValue = 0;
uint16 zclApp_SoilHumiditySensor_MeasuredValue_old = 0;
uint16 zclApp_SoilHumiditySensor_MeasuredValueTr = 1;
uint16 zclApp_SoilHumiditySensor_MeasuredValueRawAdc = 0;

// Battery status
uint8 zclBattery_Voltage = 0xff;
uint8 zclBattery_PercentageRemainig = 0xff;
uint16 zclBattery_RawAdc = 0xff;

// Basic Cluster string attributes
const uint8 zclApp_ManufacturerName[] = {17, 'E', 'f', 'e', 'k', 't', 'a', 'L', 'a', 'b', '_', 'f', 'o', 'r', '_', 'y', 'o', 'u'};
const uint8 zclApp_ModelId[] = {13, 'M', 'O', 'D', 'K', 'A', 'M', '_', 'F', 'l', 'o', 'w', 'e', 'r'};
const uint8 zclApp_DateCode[] = {15, '2', '0', '2', '6', '0', '1', '0', '7', '.', '6', '4', '3', '.', '7', '7'};
const uint8 zclApp_SWBuildID[] = {5, '1', '.', '0', '.', '0'};
const uint8 zclApp_PowerSource = POWER_SOURCE_BATTERY;

/*********************************************************************
 * APPLICATION CONFIGURATION (Default Values)
 *********************************************************************/

application_config_t zclApp_Config = {
    .ReportDelay = APP_REPORT_DELAY,           // Default report delay: 3 minutes
    .mode1 = false,                            // Temperature compensation disabled
    .Resolution = 10,                          // DS18B20 resolution: 10-bit
    .localTime = 0,                            // Local time counter
    .ComparisonPreviousData3 = 1,              // Threshold-based reporting enabled
    .txPower = 4,                              // Transmit power: +4 dBm
    .illumCoof = 6                             // Illuminance correction coefficient
};

/*********************************************************************
 * ATTRIBUTE LIST - Endpoint 1 (Multi-Sensor)
 *********************************************************************/

CONST zclAttrRec_t zclApp_AttrsFirstEP[] = {
    // Basic Cluster
    {BASIC, {ATTRID_BASIC_ZCL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ZCLVersion}},
    {BASIC, {ATTRID_BASIC_APPL_VERSION, ZCL_UINT8, R, (void *)&zclApp_ApplicationVersion}},
    {BASIC, {ATTRID_BASIC_STACK_VERSION, ZCL_UINT8, R, (void *)&zclApp_StackVersion}},
    {BASIC, {ATTRID_BASIC_HW_VERSION, ZCL_UINT8, R, (void *)&zclApp_HWRevision}},
    {BASIC, {ATTRID_BASIC_MANUFACTURER_NAME, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ManufacturerName}},
    {BASIC, {ATTRID_BASIC_MODEL_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_ModelId}},
    {BASIC, {ATTRID_BASIC_DATE_CODE, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_DateCode}},
    {BASIC, {ATTRID_BASIC_POWER_SOURCE, ZCL_DATATYPE_ENUM8, R, (void *)&zclApp_PowerSource}},
    {BASIC, {ATTRID_BASIC_SW_BUILD_ID, ZCL_DATATYPE_CHAR_STR, R, (void *)zclApp_SWBuildID}},
    {BASIC, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclApp_clusterRevision_all}},
    
    // Identify Cluster
    {IDENTIFY, {ATTRID_CLUSTER_REVISION, ZCL_DATATYPE_UINT16, R, (void *)&zclApp_clusterRevision_all}},
    {IDENTIFY, {ATTRID_IDENTIFY_TIME, ZCL_DATATYPE_UINT16, RW, (void *)&zclApp_IdentifyTime}},
    
    // Time Cluster
    {GEN_TIME, {ATTRID_TIME_STANDARD_TIME, ZCL_UINT32, R, (void *)&zclApp_Config.localTime}},
    
    // Power Configuration Cluster
    {POWER_CFG, {ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING, ZCL_UINT8, R, (void *)&zclBattery_PercentageRemainig}},
    {POWER_CFG, {ATTRID_ReportDelay, ZCL_UINT16, RW, (void *)&zclApp_Config.ReportDelay}},
    {POWER_CFG, {ATTRID_ComparisonPreviousData3, ZCL_DATATYPE_BOOLEAN, RW, (void *)&zclApp_Config.ComparisonPreviousData3}},
    {POWER_CFG, {ATTRID_TXPOWER, ZCL_INT8, RW, (void *)&zclApp_Config.txPower}},
    
    // Temperature Measurement Cluster
    {TEMP, {ATTRID_MS_TEMPERATURE_MEASURED_VALUE, ZCL_INT16, R, (void *)&zclApp_Temperature_Sensor_MeasuredValue}},
    {TEMP, {ATTRID_TEMP_RES, ZCL_UINT8, RW, (void *)&zclApp_Config.Resolution}},
    {TEMP, {ATTRID_MODE1, ZCL_DATATYPE_BOOLEAN, RW, (void *)&zclApp_Config.mode1}},
    
    // Relative Humidity Cluster
    {HUMIDITY, {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, ZCL_UINT16, R, (void *)&zclApp_SoilHumiditySensor_MeasuredValue}},
    
    // Soil Humidity Cluster (Custom)
    {SOIL_HUMIDITY, {ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE, ZCL_UINT16, R, (void *)&zclApp_SoilHumiditySensor_MeasuredValue}},
    
    // Illuminance Measurement Cluster
    {ILLUMINANCE, {ATTRID_MS_ILLUMINANCE_MEASURED_VALUE, ZCL_UINT16, R, (void *)&zclApp_IlluminanceSensor_MeasuredValue}},
    {ILLUMINANCE, {ATTRID_MS_ILLUMINANCE_COOF, ZCL_INT16, RW, (void *)&zclApp_Config.illumCoof}},
};

uint8 CONST zclApp_AttrsFirstEPCount = (sizeof(zclApp_AttrsFirstEP) / sizeof(zclApp_AttrsFirstEP[0]));

/*********************************************************************
 * INPUT CLUSTER LIST
 *********************************************************************/

const cId_t zclApp_InClusterList[] = {
    ZCL_CLUSTER_ID_GEN_BASIC,
    POWER_CFG,
    IDENTIFY,
    GEN_TIME,
    HUMIDITY,
    TEMP,
    SOIL_HUMIDITY,
    ILLUMINANCE
};

#define APP_MAX_INCLUSTERS (sizeof(zclApp_InClusterList) / sizeof(zclApp_InClusterList[0]))

/*********************************************************************
 * SIMPLE DESCRIPTOR - Endpoint 1
 *********************************************************************/

SimpleDescriptionFormat_t zclApp_FirstEP = {
    1,                                              // Endpoint
    ZCL_HA_PROFILE_ID,                              // Application Profile ID
    ZCL_HA_DEVICEID_SIMPLE_SENSOR,                  // Device ID
    APP_DEVICE_VERSION,                             // Device Version
    APP_FLAGS,                                      // Flags
    APP_MAX_INCLUSTERS,                             // Number of Input Clusters
    (cId_t *)zclApp_InClusterList,                  // Input Cluster List
    0,                                              // Number of Output Clusters
    (cId_t *)NULL                                   // Output Cluster List
};

/*********************************************************************
 * FUNCTIONS
 *********************************************************************/

/*********************************************************************
 * @fn      zclResetAttributesToDefaultValues
 * @brief   Reset ZCL attributes to default values
 *********************************************************************/

void zclResetAttributesToDefaultValues(void)
{
    zclApp_Config.ReportDelay = APP_REPORT_DELAY;
    zclBS = BAT_SIZE_AA;
    zclBQ = BATTERY_QUANTITY;
    uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
}