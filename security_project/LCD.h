#ifndef _LCDH_
#define _LCDH_


void printDateLCD();
void printMonthLCD(char month);
void printDayLCD(char day);
void display_home_screen();
void printTimeLCD();
void printDateTimeStored_LCD();
void display_set_password();
void display_enter_password();
void display_set_time_date();
void set_time_date();
void display_menu();
void clearScreen();

char return_char_day[4];
char return_char_month[4];
int num_dates_stored;
#endif
