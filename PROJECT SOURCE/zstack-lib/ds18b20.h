#ifndef DS18B20_H
#define DS18B20_H

/*********************************************************************
 * FUNCTION PROTOTYPES
 *********************************************************************/

// Start temperature conversion
void reqTemperature(void);

// Read temperature from DS18B20 (returns temperature * 100)
int16 readTemperature(void);

// Check if DS18B20 is present on the bus
uint8 ds18b20_check_presence(void);

// Set DS18B20 resolution (9-12 bits)
void ds18b20_set_resolution(uint8 resolution);

#endif /* DS18B20_H */