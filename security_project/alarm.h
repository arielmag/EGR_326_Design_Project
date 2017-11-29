#ifndef ALARM_H_
#define ALARM_H_

#define NONE 0
#define DOOR 1
#define WINDOW 2
#define TEMPERATURE 3
#define PRESENCE 4

#define CALIBRATION_START 0x000200000

void Init_alarm();
int arm_disarm_alarm();
void log_arm_disarm_time();
void log_trigger_time();
void sound_alarm();
void set_armed(int arm_type);
int get_armed();
int get_triggered();
void get_saved_password();
void get_saved_arm_disarm_log();
void get_saved_trigger_log();
void write_to_memory();
void clear_arm_disarm_log();
void save_password();
void reset_system();
int check_reset();
void setup_system();
void enter_password();
void setup_password();
int verify_entry();
void write_to_memory();
void display_arm_disarm_log();
void display_trigger_log();
void display_log();
void clear_trigger_log();
int get_trigger_status();
void set_trigger_status();

#endif
