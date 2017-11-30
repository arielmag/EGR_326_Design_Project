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
<<<<<<< HEAD
void get_clock(void);
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
#endif
