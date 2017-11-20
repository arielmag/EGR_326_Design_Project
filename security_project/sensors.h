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

#endif
