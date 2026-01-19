#include "AF.h"
#include "OSAL.h"
#include "OSAL_Clock.h"
#include "OSAL_PwrMgr.h"
#include "ZComDef.h"
#include "ZDApp.h"
#include "ZDNwkMgr.h"
#include "ZDObject.h"
#include "math.h"
#include "nwk_util.h"
#include "zcl.h"
#include "zcl_app.h"
#include "zcl_diagnostic.h"
#include "zcl_general.h"
#include "zcl_ms.h"
#include "bdb.h"
#include "bdb_interface.h"
#include "gp_interface.h"
#include "OnBoard.h"
#include "stdio.h"
#include "stdlib.h"

#include "hal_adc.h"
#include "hal_drivers.h"
#include "hal_key.h"
#include "hal_led.h"
#include "commissioning.h"
#include "factory_reset.h"
#include "utils.h"
#include "ds18b20.h"
#include "delay.h"

#define HAL_KEY_CODE_RELEASE_KEY                                                HAL_KEY_CODE_NOKEY
#define MULTI                                                                   (float)0.428
#define VOLTAGE_MIN                                                             2.1
#define VOLTAGE_MAX                                                             3.2
#define ZCL_BATTERY_REPORT_REPORT_CONVERTER(millivolts)                         getBatteryRemainingPercentageZCLCR2032(millivolts)

#define DS18B20_TEMP_9_BIT                                                      0x1F
#define DS18B20_TEMP_10_BIT                                                     0x3F
#define DS18B20_TEMP_11_BIT                                                     0x5F
#define DS18B20_TEMP_12_BIT                                                     0x7F
#define TEMPDEF                                                                 250

#define START_PWS()                                                                                                                 \
    do {                                                                                                                            \
        st(T3CTL |= BV(4););                                                                                                        \
    } while (0)
#define STOP_PWS()                                                                                                                  \
    do {                                                                                                                            \
        st(T3CTL &= ~BV(4); T3CTL |= BV(2););                                                                                       \
    } while (0)
      
      
#define START_SENS()                                                                                                                \
    do {                                                                                                                            \
        HAL_TURN_ON_LED4();                                                                                                         \
        IO_PUD_PORT(DS18B20_PORT, IO_PUP);                                                                                          \
    } while (0)
#define STOP_SENS()                                                                                                                 \
    do {                                                                                                                            \
        HAL_TURN_OFF_LED4();                                                                                                        \
        ONEWIRE2_SBIT = 0;                                                                                                          \
        IO_PUD_PORT(DS18B20_PORT, IO_PDN);                                                                                          \
    } while (0)

extern bool requestNewTrustCenterLinkKey;
byte zclApp_TaskID;

int16 sendInitReportCount = 0;
uint8 resolution = DS18B20_TEMP_12_BIT;
bool blockButton = false;
bool butt = false;

static bool pushBut = false;
static bool pushBat = false;
static bool initOut = false;
static bool mode1_old = false;
static bool comparisonPreviousData3_old = false;
static uint8 currentSensorsReadingPhase = 0;
static uint8 resolution_old = 0;
static int8 txPower_old = 0;
static int16 temp_old = 0;
static int16 tempTr = 33;
static int16_t tempreal;
static int16 lum_old = 0;
static int16 lumTr = 50;
static int16 illumCoof_old = 0;
static uint32 reportDelay_old = 0;
static uint32 readPause = 50;

uint8 SeqNum = 0;
afAddrType_t inderect_DstAddr = {.addrMode = (afAddrMode_t)Addr16Bit, .endPoint = 1, .addr.shortAddr = 0};

static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper);   
static void zclApp_ProcessIdentifyTimeChange( uint8 endpoint );
   
static void zclBattery_Report(void);
static void zclApp_HandleKeys(byte shift, byte keys);
static void zclApp_Report(void);
static void zclApp_BasicResetCB(void);
static void zclApp_RestoreAttributesFromNV(void);
static void zclApp_SaveAttributesToNV(void);
static void zclApp_ReadSensors(void);
static void zclApp_ReadSendBatt(void);
static void zclApp_ReadSoilHumidity(void);
static void zclApp_sendLocalTime(void);
static void zclApp_ReadLumosity(void);
static void zclApp_InitPWM(void);
static void zclApp_sendTemperature(void);
static void zclApp_sendLumi(void);
static void zclApp_sendSoilHumidity(void);
static void zclApp_sendSoilHumidity2(void);
static void zclApp_DS18B20_RS(void);
static void zclApp_LedOn(void);
static void zclApp_LedOff(void);
static uint16 getBatteryVoltage(void);
static uint8 getBatteryVoltageZCL(uint16 millivolts);
static uint8 getBatteryRemainingPercentageZCLCR2032(uint16 volt16);
static void zclBattery_Report(void);
static void zclBattery_Read(void);

/*********************************************************************
 * ZCL General Profile Callback table
 */
static zclGeneral_AppCallbacks_t zclApp_CmdCallbacks = {
    zclApp_BasicResetCB, // Basic Cluster Reset command
    NULL,                // Identify Trigger Effect command
    NULL,                // On/Off cluster commands
    NULL,                // On/Off cluster enhanced command Off with Effect
    NULL,                // On/Off cluster enhanced command On with Recall Global Scene
    NULL,                // On/Off cluster enhanced command On with Timed Off
    NULL,                // RSSI Location command
    NULL                 // RSSI Location Response command
};

void zclApp_Init(byte task_id) {
  
  reportDelay_old = zclApp_Config.ReportDelay;
  mode1_old = zclApp_Config.mode1;
  resolution_old = zclApp_Config.Resolution;
  comparisonPreviousData3_old = zclApp_Config.ComparisonPreviousData3;
  txPower_old = zclApp_Config.txPower;
  illumCoof_old = zclApp_Config.illumCoof;

    HAL_TURN_ON_LED1();
    delay_ms(800);
    HAL_TURN_OFF_LED1();
    
    zclApp_RestoreAttributesFromNV();
    requestNewTrustCenterLinkKey = FALSE;
    zclApp_TaskID = task_id;
    
    bdb_RegisterSimpleDescriptor(&zclApp_FirstEP);
    zclGeneral_RegisterCmdCallbacks(zclApp_FirstEP.EndPoint, &zclApp_CmdCallbacks);
    zcl_registerAttrList(zclApp_FirstEP.EndPoint, zclApp_AttrsFirstEPCount, zclApp_AttrsFirstEP);
    
    zcl_registerReadWriteCB(zclApp_FirstEP.EndPoint, NULL, zclApp_ReadWriteAuthCB);
    zcl_registerForMsg(zclApp_TaskID);
    RegisterForKeys(zclApp_TaskID);
    bdb_RegisterIdentifyTimeChangeCB( zclApp_ProcessIdentifyTimeChange );
    
    START_PWS();
    START_SENS();
    IO_IMODE_PORT_PIN(LUMOISITY_PORT, LUMOISITY_PIN, IO_TRI);
    IO_IMODE_PORT_PIN(SOIL_MOISTURE_PORT, SOIL_MOISTURE_PIN, IO_TRI);
    if (zclApp_Config.Resolution == 12) {
        resolution = DS18B20_TEMP_12_BIT;
    } else if (zclApp_Config.Resolution == 10) {
        resolution = DS18B20_TEMP_10_BIT;
    } else if (zclApp_Config.Resolution == 11) {
        resolution = DS18B20_TEMP_11_BIT;
    }else if (zclApp_Config.Resolution == 9) {
        resolution = DS18B20_TEMP_9_BIT;
    }
    ds18b20_set_resolution(resolution);
    STOP_PWS();
    STOP_SENS();

    zclApp_InitPWM();
    pushBut = true;
    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_EVT, 10000);
    osal_start_reload_timer(zclApp_TaskID, APP_REPORT_BATT_EVT, 10801234);
    osal_start_reload_timer(zclApp_TaskID, APP_TIME_EVT, 360054*6);
}

uint16 zclApp_event_loop(uint8 task_id, uint16 events) {
    afIncomingMSGPacket_t *MSGpkt;
    (void)task_id;
    if (events & SYS_EVENT_MSG) {
        while ((MSGpkt = (afIncomingMSGPacket_t *)osal_msg_receive(zclApp_TaskID))) {
            switch (MSGpkt->hdr.event) {
            case KEY_CHANGE:
                zclApp_HandleKeys(((keyChange_t *)MSGpkt)->state, ((keyChange_t *)MSGpkt)->keys);
                break;
            case ZCL_INCOMING_MSG:
                if (((zclIncomingMsg_t *)MSGpkt)->attrCmd) {
                    osal_mem_free(((zclIncomingMsg_t *)MSGpkt)->attrCmd);
                }
                break;

            default:
                break;
            }
            osal_msg_deallocate((uint8 *)MSGpkt);
        }
        return (events ^ SYS_EVENT_MSG);
    }

    if (events & APP_REPORT_EVT) {
        if (initOut == false){
          sendInitReportCount++;
          if(sendInitReportCount  >=  10){
            osal_stop_timerEx(zclApp_TaskID, APP_REPORT_EVT);
            osal_clear_event(zclApp_TaskID, APP_REPORT_EVT);
            osal_start_reload_timer(zclApp_TaskID, APP_REPORT_EVT, (uint32)zclApp_Config.ReportDelay*60000); 
            initOut = true;
          }
          pushBut = true;
          pushBat = true;
          zclApp_Report();
        }else{
         zclApp_Report(); 
        }
        return (events ^ APP_REPORT_EVT);
    }

    if (events & APP_READ_SENSORS_EVT) {
        zclApp_ReadSensors();
        return (events ^ APP_READ_SENSORS_EVT);
    }
    if (events & APP_REPORT_BATT_EVT) {
        pushBat = true;
        return (events ^ APP_REPORT_BATT_EVT);
    }
    if (events & APP_READ_SEND_BATT_EVT) {
        zclApp_ReadSendBatt();
        return (events ^ APP_READ_SEND_BATT_EVT);
    }
    if (events & APP_SAVE_ATTRS_EVT) {
        zclApp_SaveAttributesToNV();
        return (events ^ APP_SAVE_ATTRS_EVT);
    }
    if (events & APP_LED_EVT) {
           zclApp_LedOff();
        return (events ^ APP_LED_EVT);
    }
     if (events & APP_TIME_EVT) {
           zclApp_Config.localTime = zclApp_Config.localTime+6;
           uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
        return (events ^ APP_TIME_EVT);
    }
    return 0; 
}

static void zclApp_LedOn(void){
   HAL_TURN_ON_LED1();
  osal_start_timerEx(zclApp_TaskID, APP_LED_EVT, 30);
}

static void zclApp_LedOff(void){
   HAL_TURN_OFF_LED1();
}

static void zclApp_HandleKeys(byte portAndAction, byte keyCode) {
    
#if APP_COMMISSIONING_BY_LONG_PRESS == TRUE
  if (bdbAttributes.bdbNodeIsOnANetwork == 1){
    zclFactoryResetter_HandleKeys(portAndAction, keyCode);
  }
#else
  zclFactoryResetter_HandleKeys(portAndAction, keyCode);
#endif
    zclCommissioning_HandleKeys(portAndAction, keyCode);
    
     if (portAndAction & HAL_KEY_PRESS) {
    if(zclApp_Config.txPower == 4){
      ZMacSetTransmitPower(TX_PWR_PLUS_4);
    }else if(zclApp_Config.txPower == 0){
      ZMacSetTransmitPower(TX_PWR_ZERO);
    }else if(zclApp_Config.txPower == -4){
      ZMacSetTransmitPower(TX_PWR_MINUS_4);
    }
      butt = true;
    }
    if (portAndAction & HAL_KEY_RELEASE) {
      butt = false;
        if(blockButton == false){
        pushBut = true;
        pushBat = true;
        zclApp_Report();
        }
    } 
}

static void zclApp_BasicResetCB(void) {
    zclApp_SaveAttributesToNV();
}

static ZStatus_t zclApp_ReadWriteAuthCB(afAddrType_t *srcAddr, zclAttrRec_t *pAttr, uint8 oper) {
    osal_start_timerEx(zclApp_TaskID, APP_SAVE_ATTRS_EVT, 2000);
    return ZSuccess;
}

static void zclApp_SaveAttributesToNV(void) {
    uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    if(zclApp_Config.ReportDelay != reportDelay_old){
      reportDelay_old = zclApp_Config.ReportDelay;
      osal_stop_timerEx(zclApp_TaskID, APP_REPORT_EVT);
      osal_clear_event(zclApp_TaskID, APP_REPORT_EVT);
      osal_start_reload_timer(zclApp_TaskID, APP_REPORT_EVT, (uint32)zclApp_Config.ReportDelay*60000); 
      pushBut = true;
      zclApp_ReadSensors();
    }
    if(zclApp_Config.mode1 != mode1_old){
      mode1_old = zclApp_Config.mode1;
      pushBut = true;
      zclApp_ReadSensors();
    }
      if(zclApp_Config.illumCoof != illumCoof_old){
      illumCoof_old = zclApp_Config.illumCoof;
      pushBut = true;
      zclApp_ReadSensors();
    }
    if(zclApp_Config.ComparisonPreviousData3 != comparisonPreviousData3_old){
      comparisonPreviousData3_old = zclApp_Config.ComparisonPreviousData3;
    if(zclApp_Config.ComparisonPreviousData3 == true){
        NLME_SetPollRate(6500);   
      }else{
        NLME_SetPollRate(0);  
      }
    }
    if(zclApp_Config.txPower != txPower_old){
      txPower_old = zclApp_Config.txPower;
    if(zclApp_Config.txPower == 4){
      ZMacSetTransmitPower(TX_PWR_PLUS_4);
    }else if(zclApp_Config.txPower == 0){
      ZMacSetTransmitPower(TX_PWR_ZERO);
    }else if(zclApp_Config.txPower == -4){
      ZMacSetTransmitPower(TX_PWR_MINUS_4);
    }
    }
}

static void zclApp_RestoreAttributesFromNV(void) {
    uint8 status = osal_nv_item_init(NW_APP_CONFIG, sizeof(application_config_t), NULL);
    if (status == NV_ITEM_UNINIT) {
        uint8 writeStatus = osal_nv_write(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    }
    if (status == ZSUCCESS) {
        osal_nv_read(NW_APP_CONFIG, 0, sizeof(application_config_t), &zclApp_Config);
    }
}


static void zclApp_ReadSensors(void) {
 
    switch (currentSensorsReadingPhase++) {
    case 0:
      START_PWS();
      zclApp_ReadSoilHumidity();
      readPause = 50;
      STOP_PWS();
      zclApp_sendSoilHumidity();
      zclApp_sendSoilHumidity2();
      break;
  
    case 1:  
      START_SENS();
       zclApp_ReadLumosity();
       readPause = 50;
       zclApp_sendLumi();
        break;
        
    case 2:  
      blockButton = true;
    
    if(zclApp_Config.Resolution == 9){
      resolution = DS18B20_TEMP_9_BIT;
      readPause = 360;
    }else if(zclApp_Config.Resolution == 10){
      resolution = DS18B20_TEMP_10_BIT;
      readPause = 470;
    }else if(zclApp_Config.Resolution == 11){
      resolution = DS18B20_TEMP_11_BIT;
      readPause = 680;
    }else if(zclApp_Config.Resolution == 12){
      resolution = DS18B20_TEMP_12_BIT;
      readPause = 1000;
    }else{
     resolution = DS18B20_TEMP_9_BIT; 
     readPause = 360;
    }
    
    if(zclApp_Config.Resolution != resolution_old){
      resolution_old = zclApp_Config.Resolution;
      ds18b20_set_resolution(resolution);
    }
      
      reqTemperature();
        break;
        
    case 3:  
        zclApp_DS18B20_RS();
        readPause = 50;
       STOP_SENS();
        zclApp_sendTemperature();
        break;
  
    default:
        currentSensorsReadingPhase = 0;
        blockButton = false;
        if(pushBat == true){
          osal_start_timerEx(zclApp_TaskID, APP_READ_SEND_BATT_EVT, 500);
          pushBat = false;
        }

        break;
    }
    if (currentSensorsReadingPhase != 0) {
        osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, (uint32)readPause);
    }
}

static void zclApp_ReadSendBatt(void) {
    zclApp_sendLocalTime();
    zclBattery_Read();
    zclBattery_Report();
    pushBut = false;

}

static void zclApp_ReadSoilHumidity(void) {
    zclBattery_RawAdc = adcReadSampled(HAL_ADC_CHANNEL_VDD, 
                                      HAL_ADC_RESOLUTION_14, 
                                      HAL_ADC_REF_125V, 
                                      3);
    HalAdcSetReference(HAL_ADC_REF_AVDD);
    
    uint32 soilSum = 0;
    for (uint8 i = 0; i < 3; i++) {
        HalAdcRead(SOIL_MOISTURE_PIN, HAL_ADC_RESOLUTION_14);
        soilSum += HalAdcRead(SOIL_MOISTURE_PIN, HAL_ADC_RESOLUTION_14);
        if (i < 2) {
            delay_us(500);
        }
    }
    zclApp_SoilHumiditySensor_MeasuredValueRawAdc = (uint16)(soilSum / 3);
    // 0.198 * ADC + 3597.759 = (ADC * 198 + 3597759) / 1000
    uint32 dry_calc = (uint32)zclBattery_RawAdc * 198 + 3597759;
    // 0.171 * ADC + 2060.895 = (ADC * 171 + 2060895) / 1000
    uint32 wet_calc = (uint32)zclBattery_RawAdc * 171 + 2060895;
    
    uint16 dry = (uint16)(dry_calc / 1000); 
    uint16 wet = (uint16)(wet_calc / 1000);
    
    int32 compensatedAdc = zclApp_SoilHumiditySensor_MeasuredValueRawAdc;
    if (zclApp_Config.mode1 == true) {
        int16 tempDiff = tempreal - TEMPDEF;
        int16 tempComp = (tempDiff * 420) / 100;  // coefficient 0.42
        compensatedAdc -= tempComp;
        if (compensatedAdc < 0) compensatedAdc = 0;
    }
    
    if (dry > wet && compensatedAdc < dry && compensatedAdc > wet) {
        uint32 diff = dry - wet;
        uint32 value = dry - compensatedAdc;
        zclApp_SoilHumiditySensor_MeasuredValue = (uint16)((value * 10000UL) / diff);
    } else if (compensatedAdc >= dry) {
        zclApp_SoilHumiditySensor_MeasuredValue = 0;
    } else if (compensatedAdc <= wet) {
        zclApp_SoilHumiditySensor_MeasuredValue = 10000;
    } else {
        zclApp_SoilHumiditySensor_MeasuredValue = 5000;
    }
}

static void zclApp_DS18B20_RS(void){  
    int16 temp = readTemperature();
    if (temp != 15000) {  
        zclApp_Temperature_Sensor_MeasuredValue = temp;
        tempreal = temp/10;
    }
}

static void zclApp_ReadLumosity(void) {
uint32 temp;

  temp = adcReadSampled(LUMOISITY_PIN, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_AVDD, 5)* zclApp_Config.illumCoof;
  
  if(temp >= 65535)
  {
    temp = 65535;
  }
  zclApp_IlluminanceSensor_MeasuredValue = temp;
}

static void zclApp_sendTemperature(void) {
if(pushBut == true){
   zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 3;
     
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_INT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_Temperature_Sensor_MeasuredValue);
       pReportCmd->attrList[1].attrID = ATTRID_TEMP_RES;
       pReportCmd->attrList[1].dataType = ZCL_UINT8;
       pReportCmd->attrList[1].attrData = (void *)(&zclApp_Config.Resolution);
       pReportCmd->attrList[2].attrID = ATTRID_MODE1;
       pReportCmd->attrList[2].dataType = ZCL_DATATYPE_BOOLEAN;
       pReportCmd->attrList[2].attrData = (void *)(&zclApp_Config.mode1);
       zcl_SendReportCmd(1, &inderect_DstAddr, TEMP, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
    osal_mem_free(pReportCmd);
}else{
  if(zclApp_Config.ComparisonPreviousData3 == true){
       if (abs(zclApp_Temperature_Sensor_MeasuredValue - temp_old) >= tempTr) {
         temp_old = zclApp_Temperature_Sensor_MeasuredValue;
        zclReportCmd_t *pReportCmd;
        const uint8 NUM_ATTRIBUTES = 1;
  
        pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
        if (pReportCmd != NULL) {
         pReportCmd->numAttr = NUM_ATTRIBUTES;
    
         pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
         pReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
         pReportCmd->attrList[0].attrData = (void *)(&zclApp_Temperature_Sensor_MeasuredValue); 
         zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, TEMP, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
        }
        osal_mem_free(pReportCmd); 
       }
      }else{
      zclReportCmd_t *pReportCmd;
      const uint8 NUM_ATTRIBUTES = 1;
  
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
      pReportCmd->numAttr = NUM_ATTRIBUTES;
    
      pReportCmd->attrList[0].attrID = ATTRID_MS_TEMPERATURE_MEASURED_VALUE;
      pReportCmd->attrList[0].dataType = ZCL_DATATYPE_INT16;
      pReportCmd->attrList[0].attrData = (void *)(&zclApp_Temperature_Sensor_MeasuredValue); 
      zcl_SendReportCmd(zclApp_FirstEP.EndPoint, &inderect_DstAddr, TEMP, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
     osal_mem_free(pReportCmd);
      }
}
}

static void zclApp_sendLumi(void) {
if(pushBut == true){
    zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 2;
    
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_ILLUMINANCE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_IlluminanceSensor_MeasuredValue);
       
       pReportCmd->attrList[1].attrID = ATTRID_MS_ILLUMINANCE_COOF;
       pReportCmd->attrList[1].dataType = ZCL_INT16;
       pReportCmd->attrList[1].attrData = (void *)(&zclApp_Config.illumCoof);
       
       zcl_SendReportCmd(1, &inderect_DstAddr, ILLUMINANCE, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
    osal_mem_free(pReportCmd);
    }else{
       if(zclApp_Config.ComparisonPreviousData3 == true){
       if (abs(zclApp_IlluminanceSensor_MeasuredValue - lum_old) >= lumTr) {
         lum_old = zclApp_IlluminanceSensor_MeasuredValue;
        zclReportCmd_t *pReportCmd;
        const uint8 NUM_ATTRIBUTES = 1;
  
        pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
        if (pReportCmd != NULL) {
         pReportCmd->numAttr = NUM_ATTRIBUTES;
    
         pReportCmd->attrList[0].attrID = ATTRID_MS_ILLUMINANCE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_IlluminanceSensor_MeasuredValue);
        zcl_SendReportCmd(1, &inderect_DstAddr, ILLUMINANCE, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
        }
        osal_mem_free(pReportCmd); 
       }
      }else{
      zclReportCmd_t *pReportCmd;
      const uint8 NUM_ATTRIBUTES = 1;
  
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
      pReportCmd->numAttr = NUM_ATTRIBUTES;
    
      pReportCmd->attrList[0].attrID = ATTRID_MS_ILLUMINANCE_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_IlluminanceSensor_MeasuredValue);
        zcl_SendReportCmd(1, &inderect_DstAddr, ILLUMINANCE, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
     osal_mem_free(pReportCmd);
      }
}
}

static void zclApp_sendSoilHumidity(void) {
   zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 1;
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_SoilHumiditySensor_MeasuredValue);
       zcl_SendReportCmd(1, &inderect_DstAddr, SOIL_HUMIDITY, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
    osal_mem_free(pReportCmd);
}

static void zclApp_sendSoilHumidity2(void) {
if(pushBut == false){
    zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 1;
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       
       pReportCmd->attrList[0].attrID = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_SoilHumiditySensor_MeasuredValue);
       zcl_SendReportCmd(1, &inderect_DstAddr, HUMIDITY, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
    osal_mem_free(pReportCmd);
}else{

   zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 1;
     
     pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
     if (pReportCmd != NULL) {
       pReportCmd->numAttr = NUM_ATTRIBUTES;
       pReportCmd->attrList[0].attrID = ATTRID_MS_RELATIVE_HUMIDITY_MEASURED_VALUE;
       pReportCmd->attrList[0].dataType = ZCL_UINT16;
       pReportCmd->attrList[0].attrData = (void *)(&zclApp_SoilHumiditySensor_MeasuredValue);
       zcl_SendReportCmd(1, &inderect_DstAddr, HUMIDITY, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
     }
    osal_mem_free(pReportCmd);

   }
}

void zclBattery_Report(void) {
if(pushBut == false){
  zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 1;
    
    pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
    if (pReportCmd != NULL) {
        pReportCmd->numAttr = NUM_ATTRIBUTES;

        pReportCmd->attrList[0].attrID = ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING;
        pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
        pReportCmd->attrList[0].attrData = (void *)(&zclBattery_PercentageRemainig);
        
        zcl_SendReportCmd(1, &inderect_DstAddr, POWER_CFG, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
    }
    osal_mem_free(pReportCmd);
}else{
    zclReportCmd_t *pReportCmd;
    const uint8 NUM_ATTRIBUTES = 4;
    
    pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
    if (pReportCmd != NULL) {
        pReportCmd->numAttr = NUM_ATTRIBUTES;

        pReportCmd->attrList[0].attrID = ATTRID_POWER_CFG_BATTERY_PERCENTAGE_REMAINING;
        pReportCmd->attrList[0].dataType = ZCL_DATATYPE_UINT8;
        pReportCmd->attrList[0].attrData = (void *)(&zclBattery_PercentageRemainig);
        
        pReportCmd->attrList[1].attrID = ATTRID_ReportDelay;
        pReportCmd->attrList[1].dataType = ZCL_UINT16;
        pReportCmd->attrList[1].attrData = (void *)(&zclApp_Config.ReportDelay);
        
        pReportCmd->attrList[2].attrID = ATTRID_ComparisonPreviousData3;
        pReportCmd->attrList[2].dataType = ZCL_DATATYPE_BOOLEAN;
        pReportCmd->attrList[2].attrData = (void *)(&zclApp_Config.ComparisonPreviousData3);
        
        pReportCmd->attrList[3].attrID = ATTRID_TXPOWER;
        pReportCmd->attrList[3].dataType = ZCL_INT8;
        pReportCmd->attrList[3].attrData = (void *)(&zclApp_Config.txPower);
        
        zcl_SendReportCmd(1, &inderect_DstAddr, POWER_CFG, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
    }
    osal_mem_free(pReportCmd);
}
}

static void zclApp_Report(void) { 
  if(butt == false){
  if(devState == DEV_END_DEVICE && pushBut == true){
  zclApp_LedOn();
  }
  
  if(devState == DEV_END_DEVICE){
  osal_start_timerEx(zclApp_TaskID, APP_READ_SENSORS_EVT, 500); 
  }
  }
}

static void zclApp_InitPWM(void) {
    PERCFG &= ~(0x20);
    P2SEL |= 0x20;
    P2DIR |= 0xC0;
    P1SEL |= BV(4);
    P1DIR |= BV(4);

    T3CTL &= ~BV(4);
    T3CTL |= BV(2);
    T3CTL &= ~0x08;
    T3CTL |= 0x03;

    T3CCTL1 &= ~0x40;
    T3CCTL1 |= BV(2);
    T3CCTL1 |= BV(4);

    T3CTL &= ~(BV(7) | BV(6) | BV(5));
    T3CC0 = 4;
}

static void zclApp_sendLocalTime(void) {
 
  const uint8 NUM_ATTRIBUTES = 1;
  zclReportCmd_t *pReportCmd;
  
  pReportCmd = osal_mem_alloc(sizeof(zclReportCmd_t) + (NUM_ATTRIBUTES * sizeof(zclReport_t)));
  if (pReportCmd != NULL) {
    pReportCmd->numAttr = NUM_ATTRIBUTES;
    pReportCmd->attrList[0].attrID = ATTRID_TIME_STANDARD_TIME;
    pReportCmd->attrList[0].dataType = ZCL_UINT32;
    pReportCmd->attrList[0].attrData = (void *)(&zclApp_Config.localTime);

    zcl_SendReportCmd(1, &inderect_DstAddr, GEN_TIME, pReportCmd, ZCL_FRAME_SERVER_CLIENT_DIR, true, SeqNum++);
  }
  osal_mem_free(pReportCmd);
}


static void zclApp_ProcessIdentifyTimeChange( uint8 endpoint )
{
  (void) endpoint;
  if(initOut == true){
  if ( zclApp_IdentifyTime > 0 )
  {
    HAL_TURN_ON_LED1();
  }
  else
  {
    HAL_TURN_OFF_LED1();
  }
}
}

static uint8 getBatteryVoltageZCL(uint16 millivolts) {
    uint8 volt8 = (uint8)(millivolts / 100);
    if ((millivolts - (volt8 * 100)) > 50) {
        return volt8 + 1;
    } else {
        return volt8;
    }
}

static uint16 getBatteryVoltage(void) {
    HalAdcSetReference(HAL_ADC_REF_125V);
    zclBattery_RawAdc = adcReadSampled(HAL_ADC_CHANNEL_VDD, HAL_ADC_RESOLUTION_14, HAL_ADC_REF_125V, 10);
    return (uint16)(zclBattery_RawAdc * MULTI);
}

static uint8 getBatteryRemainingPercentageZCLCR2032(uint16 volt16) {
    float battery_level;
   if (volt16 >= 3000) { //for PA
        battery_level = 100;
    } else if (volt16 > 2500) {
        battery_level = 100 - ((3000 - volt16) * 90) / 500;
    } else if (volt16 > 2200) {
        battery_level = 10 - ((2500 - volt16) * 10) / 300;
    } else {
        battery_level = 0;
    }    

    return (uint8)(battery_level * 2);
}

static void zclBattery_Read(void) {
    uint16 millivolts = getBatteryVoltage();
    zclBattery_Voltage = getBatteryVoltageZCL(millivolts);
    zclBattery_PercentageRemainig = ZCL_BATTERY_REPORT_REPORT_CONVERTER(millivolts);
}