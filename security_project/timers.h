#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
void SysTick_Init();
void Init48MHz();
void SysTick_delay(uint16_t delay);
void cas_sysDelay (uint16_t sec);
void SysTick_delay(uint16_t delay);
void init_user_input_WDT_timer();
<<<<<<< HEAD
void get_clock(void);
=======
void set_count(int value);
int get_count();

>>>>>>> ce347f13da3620c5d2c256b622f934adb58fa88d
#endif
