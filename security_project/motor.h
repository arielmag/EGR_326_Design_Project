#ifndef MOTOR_H_
#define MOTOR_H_

#include <stdint.h>
void Init_motor();
void full_step_reverse(uint16_t speed);
void full_step(uint16_t speed);
int lock_unlock_door();
void clear_motor();
int get_lock();
void set_lock(int status);

#endif
