#ifndef SENSORS_H_
#define SENSORS_H_

#define OPEN 1
#define CLOSED 0
#define RTC_ADDRESS 0x68
#define RTC_TEMPERATURE_ADDRESS 0x11
void PORT2_IRQHandler(void);
void Init_PIR();
int check_PIR();
void Init_hall();
int get_window_status();
int get_door_status();
void init_LED2();
void green();
void red();
void check_sensors();
void flashing_red();
void ADC_Init();
void display_trigger(int type);
void set_trigger_displayed(int display);
int get_trigger_displayed();

#endif
