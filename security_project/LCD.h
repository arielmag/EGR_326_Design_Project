#ifndef _LCDH_
#define _LCDH_

void printDateLCD();
void printMonthLCD(char month);
void printDayLCD(char day);
//void printTimeLCD();
//void printDateTimeStored(int num_dates_stored);
char return_char_day[4];
char return_char_month[4];
int num_dates_stored;

void printTimeLCD();
void printDateTimeStored_LCD();
void clearScreen();

#endif
