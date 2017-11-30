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
<<<<<<< HEAD
<<<<<<< HEAD
void ADC_Init();
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 7e165f4... LCD backlight activated
#endif
