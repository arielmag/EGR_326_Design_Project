#ifndef _RTCH_
#define _RTCH_

#define RTC_ADDRESS 0x68
#define RTC_TEMPERATURE_ADDRESS 0x11

char RTC_registers[7]; //return read values from slave device
uint8_t read_back_data[36]; // array to hold values read back from flash
unsigned char date[15]; //manually set at RTC_write for debug
unsigned char calendar[13];

void I2C_init();
void Init_RTC();
void Port4_Initb();
void RTC_write();
float RTC_read_temperature();
float convertTemperature(uint8_t tUBYTE, uint8_t tLBYTE);
void RTC_read();
float get_temperature();
static float temp_f;
#endif
