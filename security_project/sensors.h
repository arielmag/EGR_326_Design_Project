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
<<<<<<< HEAD
void ADC_Init();
=======
void check_sensors();
void flashing_red();

>>>>>>> ce347f13da3620c5d2c256b622f934adb58fa88d
#endif
