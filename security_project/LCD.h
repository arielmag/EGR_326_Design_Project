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
void go_home();
void display_menu();
void print_temperature();
void pwm_lcd();

char return_char_day[4];
char return_char_month[4];
int num_dates_stored;
#endif
