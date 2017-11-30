#ifndef ALARM_H_
#define ALARM_H_

#define NONE 0
#define DOOR 1
#define WINDOW 2
#define TEMPERATURE 3
#define PRESENCE 4

void Init_alarm();
int arm_disarm_alarm();
void log_arm_disarm_time();
void log_trigger_time();
void sound_alarm();
void set_armed(int arm_type);
int get_armed();
void set_triggered(int triggerType);
int get_triggered();

#endif
