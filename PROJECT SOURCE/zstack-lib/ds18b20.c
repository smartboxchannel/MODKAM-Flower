/*********************************************************************
 * INCLUDES
 *********************************************************************/

#include "ds18b20.h"
#include "OnBoard.h"
#include "delay.h"

/*********************************************************************
 * CONSTANTS
 *********************************************************************/

// DS18B20 Commands
#define DS18B20_SKIP_ROM          0xCC
#define DS18B20_CONVERT_T         0x44
#define DS18B20_READ_SCRATCHPAD   0xBE
#define DS18B20_WRITE_SCRATCHPAD  0x4E
#define DS18B20_COPY_SCRATCHPAD   0x48

// Resolution settings
#define DS18B20_TEMP_9_BIT        0x1F   // 9-bit resolution
#define DS18B20_TEMP_10_BIT       0x3F   // 10-bit resolution
#define DS18B20_TEMP_11_BIT       0x5F   // 11-bit resolution
#define DS18B20_TEMP_12_BIT       0x7F   // 12-bit resolution

// Default configuration
#ifndef DS18B20_RESOLUTION
#define DS18B20_RESOLUTION        DS18B20_TEMP_12_BIT
#endif

#ifndef DS18B20_RETRY_COUNT
#define DS18B20_RETRY_COUNT       10
#endif

// Maximum conversion time for 12-bit resolution (750ms * 1.2 margin)
#define MAX_CONVERSION_TIME        900

#define DS18B20_RETRY_DELAY        ((uint16)(MAX_CONVERSION_TIME / DS18B20_RETRY_COUNT))

// Temperature scaling: multiply by 100 for 2 decimal places
#define TEMP_SCALE                 100
// Error temperature: 150.00°C * 100 = 15000
#define ERROR_TEMP_SCALED          15000

/*********************************************************************
 * LOCAL VARIABLES
 *********************************************************************/

static uint8 ds18b20_current_resolution = DS18B20_RESOLUTION;

/*********************************************************************
 * LOCAL FUNCTIONS PROTOTYPES
 *********************************************************************/

static void ds18b20_send(uint8 bit);
static uint8 ds18b20_read(void);
static void ds18b20_send_byte(uint8 data);
static uint8 ds18b20_read_byte(void);
static uint8 ds18b20_Reset(void);
static void ds18b20_GroudPins(void);
static uint8 ds18b20_crc8(const uint8 *data, uint8 len);

/*********************************************************************
 * @fn      ds18b20_crc8
 * @brief   Calculate 8-bit CRC for DS18B20 data
 *********************************************************************/

static uint8 ds18b20_crc8(const uint8 *data, uint8 len)
{
    uint8 crc = 0;
    uint8 i, j;
    
    for (i = 0; i < len; i++) {
        crc ^= data[i];
        for (j = 0; j < 8; j++) {
            if (crc & 0x01) {
                crc = (crc >> 1) ^ 0x8C;  // Polynomial: x^8 + x^5 + x^4 + 1
            } else {
                crc >>= 1;
            }
        }
    }
    
    return crc;
}

/*********************************************************************
 * @fn      ds18b20_send
 * @brief   Send a single bit on the 1-Wire bus
 *********************************************************************/

static void ds18b20_send(uint8 bit)
{
    ONEWIRE2_SBIT = 1;
    ONEWIRE2_DIR |= ONEWIRE2_BV;
    ONEWIRE2_SBIT = 0;
    
    if (bit != 0) {
        delay_us(8);
        ONEWIRE2_SBIT = 1;
        ONEWIRE2_DIR &= ~ONEWIRE2_BV;
        delay_us(80);
    } else {
        delay_us(80);
        ONEWIRE2_SBIT = 1;
        ONEWIRE2_DIR &= ~ONEWIRE2_BV;
        delay_us(10);
    }
    
    delay_us(2);
}

/*********************************************************************
 * @fn      ds18b20_read
 * @brief   Read a single bit from the 1-Wire bus
 *********************************************************************/

static uint8 ds18b20_read(void)
{
    uint8 bit_value;
    
    ONEWIRE2_SBIT = 1;
    ONEWIRE2_DIR |= ONEWIRE2_BV;
    ONEWIRE2_SBIT = 0;
    delay_us(2);
    
    ONEWIRE2_DIR &= ~ONEWIRE2_BV;
    delay_us(8);
    
    bit_value = ONEWIRE2_SBIT;
    delay_us(60);
    
    return bit_value;
}

/*********************************************************************
 * @fn      ds18b20_send_byte
 * @brief   Send a byte on the 1-Wire bus
 *********************************************************************/

static void ds18b20_send_byte(uint8 data)
{
    uint8 i;
    for (i = 0; i < 8; i++) {
        ds18b20_send(data & 0x01);
        data >>= 1;
    }
}

/*********************************************************************
 * @fn      ds18b20_read_byte
 * @brief   Read a byte from the 1-Wire bus
 *********************************************************************/

static uint8 ds18b20_read_byte(void)
{
    uint8 i;
    uint8 data = 0;
    
    for (i = 0; i < 8; i++) {
        if (ds18b20_read()) {
            data |= (1 << i);
        }
    }
    
    return data;
}

/*********************************************************************
 * @fn      ds18b20_Reset
 * @brief   Send reset pulse and check for presence
 * @return  1 if device present, 0 otherwise
 *********************************************************************/

static uint8 ds18b20_Reset(void)
{
    uint8 presence;
    
    ONEWIRE2_DIR |= ONEWIRE2_BV;
    ONEWIRE2_SBIT = 0;
    delay_us(500);
    
    ONEWIRE2_DIR &= ~ONEWIRE2_BV;
    delay_us(70);
    
    presence = ONEWIRE2_SBIT;
    delay_us(200);
    
    ONEWIRE2_SBIT = 1;
    ONEWIRE2_DIR |= ONEWIRE2_BV;
    
    return (presence == 0);
}

/*********************************************************************
 * @fn      ds18b20_GroudPins
 * @brief   Release the 1-Wire bus
 *********************************************************************/

static void ds18b20_GroudPins(void)
{
    ONEWIRE2_DIR &= ~ONEWIRE2_BV;
    ONEWIRE2_SBIT = 1;
}

/*********************************************************************
 * @fn      ds18b20_set_resolution
 * @brief   Set DS18B20 measurement resolution
 * @param   resolution - DS18B20_TEMP_9_BIT through DS18B20_TEMP_12_BIT
 *********************************************************************/

void ds18b20_set_resolution(uint8 resolution)
{
    if (resolution == ds18b20_current_resolution) {
        return;
    }
    
    if (resolution != DS18B20_TEMP_9_BIT &&
        resolution != DS18B20_TEMP_10_BIT &&
        resolution != DS18B20_TEMP_11_BIT &&
        resolution != DS18B20_TEMP_12_BIT) {
        return;
    }
    
    if (!ds18b20_Reset()) {
        ds18b20_GroudPins();
        return;
    }
    
    ds18b20_send_byte(DS18B20_SKIP_ROM);
    ds18b20_send_byte(DS18B20_WRITE_SCRATCHPAD);
    
    ds18b20_send_byte(0x00);  // TH register (unused)
    ds18b20_send_byte(0x00);  // TL register (unused)
    ds18b20_send_byte(resolution);
    
    ds18b20_Reset();
    
    if (!ds18b20_Reset()) {
        ds18b20_GroudPins();
        return;
    }
    
    ds18b20_send_byte(DS18B20_SKIP_ROM);
    ds18b20_send_byte(DS18B20_COPY_SCRATCHPAD);
    
    delay_ms(10);
    
    ds18b20_current_resolution = resolution;
    ds18b20_GroudPins();
}

/*********************************************************************
 * @fn      reqTemperature
 * @brief   Start temperature conversion on all DS18B20 devices
 *********************************************************************/

void reqTemperature(void)
{
    if (ds18b20_Reset()) {
        ds18b20_send_byte(DS18B20_SKIP_ROM);
        ds18b20_send_byte(DS18B20_CONVERT_T);
    }
    ds18b20_GroudPins();
}

/*********************************************************************
 * @fn      readTemperature
 * @brief   Read temperature from DS18B20
 * @return  Temperature * 100 (e.g., 2532 = 25.32°C)
 *********************************************************************/

int16 readTemperature(void)
{
    uint8 temppp1, temp2, retry_count = DS18B20_RETRY_COUNT;
    uint8 scratchpad[9];
    int16 raw_temp;
    int16 temperature_scaled;
    uint8 crc_calculated, crc_received;
    static uint8 consecutive_25c_count = 0;
    
    while (retry_count) {
        if (!ds18b20_Reset()) {
            ds18b20_GroudPins();
            consecutive_25c_count = 0;
            return ERROR_TEMP_SCALED;
        }
        
        ds18b20_send_byte(DS18B20_SKIP_ROM);
        ds18b20_send_byte(DS18B20_READ_SCRATCHPAD);
        
        for (uint8 i = 0; i < 9; i++) {
            scratchpad[i] = ds18b20_read_byte();
        }
        
        temppp1 = scratchpad[0];
        temp2 = scratchpad[1];
        
        crc_calculated = ds18b20_crc8(scratchpad, 8);
        crc_received = scratchpad[8];
        
        ds18b20_Reset();
        
        // Check for read error (all bits = 1)
        if (temppp1 == 0xFF && temp2 == 0xFF) {
            ds18b20_GroudPins();
            consecutive_25c_count = 0;
            return ERROR_TEMP_SCALED;
        }
        
        // Check for 85°C value (0x0550) - power-on reset default
        if (temppp1 == 0x50 && temp2 == 0x05) {
            retry_count--;
            delay_ms(50);
            continue;
        }
        
        // Check CRC
        if (crc_calculated != crc_received) {
            retry_count--;
            delay_ms(50);
            continue;
        }
        
        // Check for 25.0625°C value (0x0190) - often indicates no device
        if (temppp1 == 0x90 && temp2 == 0x01) {
            consecutive_25c_count++;
            
            if (consecutive_25c_count >= 3) {
                ds18b20_GroudPins();
                return ERROR_TEMP_SCALED;
            }
            
            retry_count--;
            delay_ms(50);
            continue;
        } else {
            consecutive_25c_count = 0;
        }
        
        // Form raw temperature value
        raw_temp = (temp2 << 8) | temppp1;
        
        // Handle negative temperatures (two's complement)
        if (temp2 & 0xF8) {
            raw_temp = ~raw_temp + 1;
            raw_temp = -raw_temp;
        }
        
        // Convert to scaled integer: (raw_temp * 100) / 16
        // Optimized calculation: (raw_temp * 25) / 4
        temperature_scaled = (raw_temp * 25) / 4;
        
        // Check range (-55.00°C to +125.00°C) in scaled form
        if (temperature_scaled < -5500 || temperature_scaled > 12500) {
            retry_count--;
            delay_ms(50);
            continue;
        }
        
        ds18b20_GroudPins();
        return temperature_scaled;
    }
    
    ds18b20_GroudPins();
    consecutive_25c_count = 0;
    return ERROR_TEMP_SCALED;
}

/*********************************************************************
 * @fn      ds18b20_check_presence
 * @brief   Check if DS18B20 is present on the bus
 * @return  1 if device present, 0 otherwise
 *********************************************************************/

uint8 ds18b20_check_presence(void)
{
    uint8 present = ds18b20_Reset();
    ds18b20_GroudPins();
    return present;
}