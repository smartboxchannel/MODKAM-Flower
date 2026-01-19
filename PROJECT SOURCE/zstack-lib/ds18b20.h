#ifndef ds18b20_h
void reqTemperature(void);
int16 readTemperature(void);
uint8 ds18b20_verify_crc(uint8 *scratchpad);
float readTemperature3_with_crc(void);
uint8 ds18b20_check_presence(void);
void ds18b20_set_resolution(uint8 resolution);
#define ds18b20_h
#endif


