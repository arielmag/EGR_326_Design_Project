#ifndef _RTCH_
#define _RTCH_

#define RTC_ADDRESS 0x68
#define RTC_TEMPERATURE_ADDRESS 0x11
char RTC_registers[7]; //return read values from slave device
uint8_t read_back_data[36]; // array to hold values read back from flash
unsigned char date[15]; //manually set at RTC_write for debug
void uart_init();
void I2C_init();
void date_set();
void ck_valid();
void printDay(char day);
void printMonth(char month);
void reset_flag();
float RTC_read_temperature();
float convertTemperature(uint8_t tUBYTE, uint8_t tLBYTE);
void RTC_read();
void printDate();
void printTime();
void RTC_write();
void printDateTimeStored(int num_dates_stored);
void Init_RTC();

#endif
