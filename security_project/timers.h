#ifndef TIMERS_H_
#define TIMERS_H_

#include <stdint.h>
void SysTick_Init();
void Init48MHz();
void SysTick_delay(uint16_t delay);
void cas_sysDelay (uint16_t sec);
void SysTick_delay(uint16_t delay);
void init_user_input_timer32();
void set_count(int value);
int get_count();
void get_clock(void);
void init_WDT(void);

#endif
