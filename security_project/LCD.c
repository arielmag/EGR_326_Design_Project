#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "LCD.h"
#include "RTC.h"
#include "ST7735.h"
#include <string.h>
void printDayLCD(char day) //read from RTC[3]
{
    switch(day){
    case 01:
        strcpy(return_char_day, "Sun" );
        break;
    case 02:
        strcpy(return_char_day, "Mon" );
        break;
    case 03:
        strcpy(return_char_day, "Tue" );
        break;
    case 04:
        strcpy(return_char_day, "Wed" );
        break;
    case 05:
        strcpy(return_char_day, "Thr" );
        break;
    case 06:
        strcpy(return_char_day, "Fri" );
        break;
    case 07:
        strcpy(return_char_day, "Sat" );
        break;
    }

}

void printMonthLCD(char month)//read from RTC[5]
{

    switch(month){
        case 0x01:
            strcpy(return_char_month, "Jan" );
            break;
        case 0x02:
            strcpy(return_char_month, "Feb" );
            break;
        case 0x03:
            strcpy(return_char_month, "Mar" );
            break;
        case 0x04:
            strcpy(return_char_month, "Apr" );
            break;
        case 0x05:
            strcpy(return_char_month, "May" );
            break;
        case 0x06:
            strcpy(return_char_month, "Jun" );
            break;
        case 0x07:
            strcpy(return_char_month, "Jul" );
            break;
        case 0x08:
            strcpy(return_char_month, "Aug" );
            break;
        case 0x09:
            strcpy(return_char_month, "Sep" );
            break;
        case 0x10:
            strcpy(return_char_month, "Oct" );
            break;
        case 0x11:
            strcpy(return_char_month, "Nov" );
            break;
        case 0x12:
            strcpy(return_char_month, "Dec" );
            break;
    }
}

void printDateLCD(){

    printDayLCD(RTC_registers[3]); //get day format Sun/Mon..
    printMonthLCD(RTC_registers[5]); //get month format Jan

    char str_date[20];
    ST7735_FillScreen(0);
    sprintf(str_date, "%s. %s - %x, 20%x", return_char_day, return_char_month, RTC_registers[4] , RTC_registers[6]); //format string: day-month-year
    ST7735_DrawString(0, 0, "Date: ", ST7735_GREEN);
    ST7735_DrawString(0, 1, str_date, ST7735_GREEN);
}

void printTimeLCD(){

        char str_time[20];

       sprintf(str_time,"%x:%x:%x",RTC_registers[2],RTC_registers[1],RTC_registers[0]); //format string hour:min:sec

       ST7735_DrawString(0, 2, "Time: ", ST7735_GREEN);
       ST7735_DrawString(0, 3, str_time, ST7735_GREEN);
}

void printDateTimeStored_LCD(){
    int i;
    char str_stored_date[100];
    char str_stored_time[100];
    ST7735_FillScreen(0);
    for(i=0; i<num_dates_stored; i++){
        printDayLCD(read_back_data[3+(i*7)]);
        printMonthLCD(read_back_data[5+(i*7)]);


        sprintf(str_stored_date,"%s - %x, 20%x at",return_char_month,read_back_data[4+(i*7)],read_back_data[6+(i*7)]);
        sprintf(str_stored_time,"%x:%x:%x", read_back_data[2+(i*7)], read_back_data[1+(i*7)], read_back_data[0+(i*7)]);

        ST7735_DrawString(0, i*3, "Stored time: ", ST7735_GREEN);
        ST7735_DrawString(0, i*3+1, str_stored_date, ST7735_GREEN);
        ST7735_DrawString(0, i*3+2, str_stored_time, ST7735_GREEN);
    }

}

void display_home_screen()
{
       clearScreen();

       char string1[]={'T', 'E', 'M','P'}; //Eventually display temperature information
       int i;
       int x=5;
       int y=20;
       for(i=0; i<4; i++)
       {
           ST7735_DrawChar(x+(i*20), y, string1[i], ST7735_Color565(180, 240, 250), 0, 2);
       }

       y+=30;
       char string2[]={'T', 'I', 'M','E'};//Eventually display time information hour:min:sec, rtc[2] rtc[1] rtc[0]
       for(i=0; i<4; i++){
           ST7735_DrawChar(x+(i*20), y, string2[i], ST7735_Color565(180, 240, 250), 0, 2);
       }

       y+=30;
       char string3[]={'D','A','T','E'};    //Eventually display date information here
       for(i=0; i<4; i++)
       {
           ST7735_DrawChar(x+(i*20), y, string3[i], ST7735_Color565(180, 240, 250), 0, 2);
       }
}

void display_set_password()
{
    clearScreen();

    char string1[]={'S', 'E', 'T'}; //Eventually display temperature information
    int i;
    int x=5;
    int y=20;
    for(i=0; i<3; i++){
        ST7735_DrawChar(x+(i*12), y, string1[i], ST7735_Color565(180, 240, 250), 0, 2);
    }

    y+=20;
    char string2[]={'P', 'A', 'S','S','W','O','R','D'};//Eventually display time information hour:min:sec, rtc[2] rtc[1] rtc[0]
    for(i=0; i<8; i++){
        ST7735_DrawChar(x+(i*12), y, string2[i], ST7735_Color565(180, 240, 250), 0, 2);
    }

    ST7735_DrawString(0,7,"Use the keypad",ST7735_GREEN);
    ST7735_DrawString(0,8,"Enter 4 digits code:",ST7735_GREEN);

}
void display_enter_password()
{
    clearScreen();

    char string1[]={'E', 'N', 'T','E','R'}; //Eventually display temperature information
    int i;
    int x=5;
    int y=20;
    for(i=0; i<3; i++){
        ST7735_DrawChar(x+(i*12), y, string1[i], ST7735_Color565(180, 240, 250), 0, 2);
    }

    y+=20;
    char string2[]={'P', 'A', 'S','S','W','O','R','D'};//Eventually display time information hour:min:sec, rtc[2] rtc[1] rtc[0]
    for(i=0; i<8; i++){
        ST7735_DrawChar(x+(i*12), y, string2[i], ST7735_Color565(180, 240, 250), 0, 2);
    }

    ST7735_DrawString(0,7,"Use the keypad",ST7735_GREEN);
    ST7735_DrawString(0,8,"Enter 4 digits code:",ST7735_GREEN);

}

void display_set_time_date()
{
    clearScreen();

        char string1[]={'S', 'E', 'T'}; //Eventually display temperature information
        int i;
        int x=5;
        int y=5;
        for(i=0; i<3; i++){
            ST7735_DrawChar(x+(i*12), y, string1[i], ST7735_Color565(180, 240, 250), 0, 2);
        }

        y+=20;
        char string2[]={'T', 'I', 'M','E','&','D','A','T','E'};//Eventually display time information hour:min:sec, rtc[2] rtc[1] rtc[0]
        for(i=0; i<9; i++){
            ST7735_DrawChar(x+(i*12), y, string2[i], ST7735_Color565(180, 240, 250), 0, 2);
        }
        //set_time_date(); //Not ready todo
}
/*//todo
void set_time_date() //make sure delete the manually set date line in RTC_write
{

 /*   * After user inputs, unsigned char calendar[13] has form:
     * __________________________________
     * |M M D D Y Y DOW H H M M   S   S |
     * |0 1 2 3 4 5  6  7 8 9 10  11  12|
     * |________________________________|
     *
     *
     char str_set_month[20];
     char str_set_day[20];
     char str_set_year[20];
     char str_set_dow[20];
     char str_set_hour[20];
     char str_set_min[20];
     char str_set_sec[20];

int cur_row;
     ST7735_DrawString(0,6,"Use the keypad,",ST7735_GREEN);
//****************************************************************************************************************************************

    while(month_flag == 0){ //stay here unless flag is trigger, ck_valid will set mon. flag to 1 if it's valid input
        cur_row=7;
        ST7735_DrawString(0,cur_row++,"Enter month:",ST7735_GREEN); //Print Enter month:__
        calendar[0] = keypad_getkey();              //get first digit of month
        //draw character that was just entered at calendar[0] same row
        calendar[1] = keypad_getkey();              //get second digit of month
        //draw character that was just entered at calendar[1] same row
        ck_valid();
    }

    ST7735_DrawString(0,cur_row++,"You entered:",ST7735_GREEN);
    //calendar will be ASCII char - 48 to gain its decimal form
    // *16 is for converting to its hex form so the slave device can read
    date[5] = (calendar[0]-48)*16+(calendar[1]-48);         //date will be transmitted to slave device
    printMonthLCD(date[5]);                        //get month form Jan-Dec for what user just enetered instead from RTC_register
    //str_set_month use strcpy to store month info on it then print use drawstring to print on same row
    //check printMonthLCD for reference
//todo    //ST7735_DrawString(10,cur_row,str_set_month,ST7735_GREEN);
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for day
    reset_flag();
    while(day_flag ==0){
        ST7735_DrawString(0,cur_row++,"Enter Day:",ST7735_GREEN); //Print Enter day:__
        calendar[2] = keypad_getkey();              //get first digit of day
        //draw character that was just entered at calendar[2] same row
        calendar[3] = keypad_getkey();              //get second digit of day
        //draw character that was just entered at calendar[3] same row
        ck_valid();
    }

    ST7735_DrawString(0,cur_row++,"You entered:",ST7735_GREEN);
    date[4] = (calendar[2]-48)*16+(calendar[3]-48);         //date[4]- day info will be sent to RTC thru RTC_Write
    //store date [4] in to str_set_day and print it to inform user for what they entered
//todo    //ST7735_DrawString(10,cur_row,str_set_day,ST7735_GREEN);
//    printf(" Press # to confirm");             //prompt user to press # to confirm
//    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for year
    reset_flag();
    while(year_flag==0)
    {
        ST7735_DrawString(0,cur_row++,"Enter Year:",ST7735_GREEN); //Print Enter year:__
        calendar[4] = keypad_getkey();              //get first digit of year
        //draw character that was just entered at calendar[4] same row
        calendar[5] = keypad_getkey();              //get second digit of year
        //draw character that was just entered at calendar[5] same row
        ck_valid();
    }
    ST7735_DrawString(0,cur_row++,"You entered:",ST7735_GREEN);
    date[6] = (calendar[4]-48)*16+(calendar[5]-48); //date[6] will be sent to RTC through RTC_write
    //store date[6] in to str_set_year and print it to inform user for what they entered
//todo    //ST7735_DrawString(10,cur_row,str_set_year,ST7735_GREEN);
//    printf(" Press # to confirm");             //prompt user to press # to confirm
//    while(keypad_getkey()!='#');                             //stay here until confirmed
//****************************************************************************************************************************************
//Gather info for day of the week
    reset_flag();
    while(DOW_flag ==0)
    {
    ST7735_DrawString(0,cur_row++,"Enter Day of week:",ST7735_GREEN); //Print Enter year:__
    calendar[6] = keypad_getkey();
    //draw character that was just entered at calendar[6] same row

    ck_valid();
    }
    ST7735_DrawString(0,cur_row++,"You entered:",ST7735_GREEN);
    date[3] = (calendar[6]-48);
    printMonthLCD(date[3]);                        //get month form Sun-Sat for what user just enetered instead from RTC_register
    //str_set_dow use strcpy to store dow info on it then print use drawstring to print on same row
//todo    //ST7735_DrawString(10,cur_row,str_set_month,ST7735_GREEN);
       //    printf(" Press # to confirm");             //prompt user to press # to confirm
       //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for hour
    reset_flag();
    while(hour_flag == 0)
    {
        ST7735_DrawString(0,cur_row++,"Enter Hour:",ST7735_GREEN); //Print Enter hour:__
        calendar[7] = keypad_getkey();              //get first digit of year
        //draw character that was just entered at calendar[7] same row
        calendar[8] = keypad_getkey();              //get second digit of year
        //draw character that was just entered at calendar[8] same row
        ck_valid();
    }

    date[2] = (calendar[7]-48)*16+(calendar[8]-48);
    //store date [2] in to str_set_hour and print it to inform user for what they entered
//todo    //ST7735_DrawString(10,cur_row,str_set_hour,ST7735_GREEN);
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed
//****************************************************************************************************************************************
//Gather info for minute
    reset_flag();
    while(min_flag ==0)
    {
        ST7735_DrawString(0,cur_row++,"Enter Minutes:",ST7735_GREEN); //Print Enter Minutes:__
        calendar[9] = keypad_getkey();              //get first digit of year
        //draw character that was just entered at calendar[9] same row
        calendar[10] = keypad_getkey();              //get second digit of year
        //draw character that was just entered at calendar[10] same row
        ck_valid();
    }

    date[1] = (calendar[9]-48)*16+(calendar[10]-48);
    //store date [1] in to str_set_min and print it to inform user for what they entered
//todo    //ST7735_DrawString(10,cur_row,str_set_min,ST7735_GREEN);
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for second
    reset_flag();
    while(sec_flag==0)
    {
        ST7735_DrawString(0,cur_row++,"Enter Seconds:",ST7735_GREEN); //Print Enter Seconds:__
        calendar[11] = keypad_getkey();              //get first digit of year
        //draw character that was just entered at calendar[11] same row
        calendar[12] = keypad_getkey();              //get second digit of year
        //draw character that was just entered at calendar[12] same row
        ck_valid();
    }

    date[0] = (calendar[11]-48)*16+(calendar[12]-48);
    //store date [0] in to str_set_sec and print it to inform user for what they entered
//todo    //ST7735_DrawString(10,cur_row,str_set_sec,ST7735_GREEN);
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');

}
*/

void display_menu()
{
    clearScreen();

          char string1[]={'A', 'L', 'A','R','M'}; //Eventually display temperature information
          int i;
          int x=5;
          int y=5;
          for(i=0; i<5; i++)
          {
              ST7735_DrawChar(x+(i*10), y, string1[i], ST7735_Color565(180, 240, 250), 0, 1);
          }

          y+=20;
          char string2[]={'C', 'H', 'E','C','K',' ','S','E','N','S','O','R'};//Eventually display time information hour:min:sec, rtc[2] rtc[1] rtc[0]
          for(i=0; i<12; i++){
              ST7735_DrawChar(x+(i*10), y, string2[i], ST7735_Color565(180, 240, 250), 0, 1);
          }

          y+=20;
          char string3[]={'U','N','L','O','C','K'};    //Eventually display date information here
          for(i=0; i<6; i++)
          {
              ST7735_DrawChar(x+(i*10), y, string3[i], ST7735_Color565(180, 240, 250), 0, 1);
          }
          y+=20;
          char string4[]={'L','O','C','K'};    //Eventually display date information here
          for(i=0; i<4; i++)
          {
              ST7735_DrawChar(x+(i*10), y, string4[i], ST7735_Color565(180, 240, 250), 0, 1);
          }
          y+=20;
          char string5[]={'L','O','G',' ','H','I','S','T','O','R','Y'};    //Eventually display date information here
          for(i=0; i<11; i++)
          {
              ST7735_DrawChar(x+(i*10), y, string5[i], ST7735_Color565(180, 240, 250), 0, 1);
          }
}
void clearScreen(){
    ST7735_FillScreen(0x0000);            // set screen to black
}
