#ifndef _LCDH_
#define _LCDH_


void printDateLCD();
void printMonthLCD(char month);
void printDayLCD(char day);
void display_home_screen_LCD();
void printTimeLCD();
void printDateTimeStored_LCD();
void display_set_password();
void display_enter_password();
void display_set_time_date();
void set_time_date();
void display_menu_LCD();
void clearScreen();
void Init_LCD();
void reset_flag();
void ck_valid();
void arm_error_LCD();
void arm_success_LCD();
void disarm_success_LCD();
void print_temperature();
<<<<<<< HEAD
void pwm_lcd();
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
char return_char_day[4];
char return_char_month[4];
int num_dates_stored;
#endif
