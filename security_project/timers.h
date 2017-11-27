#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
void SysTick_Init();
void Init48MHz();
void SysTick_delay(uint16_t delay);
void cas_sysDelay (uint16_t sec);
void SysTick_delay(uint16_t delay);
void init_user_input_WDT_timer();
#endif
