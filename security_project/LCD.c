#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "LCD.h"
#include "RTC.h"
#include "ST7735.h"
#include "timers.h"
#include <string.h>
#include "keypad.h"
#include "alarm.h"
#include "sensors.h"
#include "motor.h"
#include "buzzer.h"
#include "keypad.h"
#include "sensors.h"
#include "bluetooth.h"

extern int month_flag;
extern int day_flag;
extern int year_flag;
extern int DOW_flag;
extern int hour_flag;
extern int min_flag;
extern int sec_flag;

extern volatile uint16_t curADCResult;
volatile int DC_LCD; // duty cyclt for lcd pwm

void Init_LCD(){
    ST7735_InitR(INITR_REDTAB);
}

static char armedArray[18];
static char disarmedArray[21];


/*
 * Display home screen and start system logic
 */
void go_home(){

    int16_t bgColor = ST7735_Color565(82, 121, 214);

    //user_timeout = 0;
    set_timeout(0);
    ST7735_FillScreen(bgColor);
    //clearScreen();
    get_trigger_status();

    strcpy(armedArray, "ALARM IS ARMED  ");
    strcpy(disarmedArray, "ALARM IS DISARMED  ");

    update_scroll = 0;
    while(check_pressed() == 0){
        display_home_screen_LCD();
//        display_trigger(get_trigger_status());
//        if(check_bluetooth())
//            go_home();
    }

    set_count(0);
    enter_password();       // Prompt user to enter password


    while(1){
        display_menu();         // Display the menu screen. Different options within
                                    // this function will change the screen displayed.
    }
}

/*
 * This function displays a menu with options:
 *
 * Main Menu
 * 1. Lock/Unlock Door
 * 2. Arm/Disarm Alarm
 * 3. Check Sensors (Door/Window/Temperature/Presence)
 * 4. View Log
 * 5. Change Time
 * 6. Change Password
 * It returns to the home screen if idle for more than 1 minute. The user can scroll
 * through the menu items using the up and down buttons and press an option by pressing
 * the enter button. Pressing the back button returns to the home screen.
 */
void display_menu(){
    display_menu_LCD();

    char menu = keypad_getkey();
    set_count(0);
    //count = 0; // after every key pressed, reset the WDT idle timer and recount second
    MAP_WDT_A_clearTimer();

    switch(menu){
        // 1 to lock/unlock door
        case '1':
            lock_unlock_door();
            break;

        // 2 to arm/disarm alarm
        case '2':
            arm_disarm_alarm();
            display_menu();
            break;

        // 3 to check sensors
        case '3':
            check_sensors();
            break;

        // 4 to view log
        case '4':
            display_log();
            break;

        // 5 to change time
        case '5':
            set_time_date();
            RTC_write();    // Set up the input in RTC
            RTC_read();     // Clear out junk values for first read
            break;

        // 6 to change password
        case '6':
            enter_password(); //verify the password first before reset
            setup_password();
            break;

        case HOME_KEY:
            go_home();
            break;

        case BACK_KEY:
            go_home();
            break;
    }
}

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
   // ST7735_FillScreen(0);
    sprintf(str_date, "%s. %s - %x, 20%x", return_char_day, return_char_month, RTC_registers[4] , RTC_registers[6]); //format string: day-month-year
    //ST7735_DrawString(0, 3, "Date: ", ST7735_GREEN);
    ST7735_DrawString(0, 8, str_date, ST7735_GREEN);
}

void printTimeLCD(){

        char str_time[20];
       sprintf(str_time,"%x : %x : %x ",RTC_registers[2],RTC_registers[1],RTC_registers[0]); //format string hour:min:sec

       //ST7735_DrawString(0, 5, "Time: ", ST7735_GREEN);
       ST7735_DrawString(0, 12, str_time, ST7735_GREEN);
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

int get_update_scroll(){
    return update_scroll;
}

void set_update_scroll(int x){
    update_scroll = x;
}

/*
 * This function displays the home screen with the time, date,
 * alarm status, and temperature
 */
void display_home_screen_LCD()
{

    int16_t bgColor = ST7735_Color565(82, 121, 214);
    int16_t textColor = ST7735_WHITE;

    int y=0;
    char str_date[20];
    char str_time[20];
    char str_temperature[20];

    int i;
    for(i=0;i<20;i++){
        str_date[i] = 0;
        str_time[i] = 0;
        str_temperature[i] = 0;
    }

       ST7735_DrawString2(0, y, "Security", textColor, bgColor);
       ST7735_DrawString2(0, y+=2, "System", textColor, bgColor);

       printDayLCD(RTC_registers[3]); //get day format Sun/Mon..
       printMonthLCD(RTC_registers[5]); //get month format Jan

       sprintf(str_date, "%s. %s %x, 20%02x", return_char_day, return_char_month, RTC_registers[4] , RTC_registers[6]); //format string: day-month-year
       ST7735_DrawString_bg(2, y+=3, str_date, textColor, bgColor);

       sprintf(str_time,"%02x:%02x:%02x", RTC_registers[2], RTC_registers[1], RTC_registers[0]);
       ST7735_DrawString_bg(2, y+=2, str_time, textColor, bgColor);

       float t = get_temperature();

       sprintf(str_temperature, "Temp: %.2f F", get_temperature());
       ST7735_DrawString_bg(2, y+=2, str_temperature, textColor, bgColor);

       y+=3;

       // Shift armed/disarmed message arrays every second for scrolling text appearance
       if(update_scroll == 1){
           if(get_armed()){
               char temp = armedArray[0];
               for(i=0;i<17;i++){
                   armedArray[i] = armedArray[i+1];
               }
               armedArray[17] = temp;
               set_update_scroll(0);

           }else{
               char temp = disarmedArray[0];
               for(i=0;i<20;i++){
                   disarmedArray[i] = disarmedArray[i+1];
               }
               disarmedArray[20] = temp;
               set_update_scroll(0);
           }
       }

       // Scrolling text for armed/disarmed
       if(get_armed()){
           for(i=0;i<18;i++){
               ST7735_DrawCharS(i*12, y*10, armedArray[i], ST7735_BLACK, ST7735_RED, 2);
           }
       }else{
           for(i=0;i<20;i++){
               ST7735_DrawCharS(i*12, y*10, disarmedArray[i], ST7735_BLACK, ST7735_GREEN, 2);
           }
       }
}

void print_temperature()
{
    char str_temperature[40];
   // ST7735_FillScreen(0);
    int x=0, y=5;

    sprintf(str_temperature, "%.2f F", RTC_read_temperature());

    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLUE;

    ST7735_DrawString2(x, y, str_temperature, textColor, bgColor);
}

void init_pwm_lcd() //using timerA1
{   DC_LCD = 750; //default duty cycle

    P5->DIR |=BIT6;
    P5->SEL0 |= BIT6;
    P5 -> SEL1 &= ~(BIT6);

    TIMER_A2 ->CCR[0] = 1000 - 1; //PWM Period
    TIMER_A2 ->CCTL[1] = TIMER_A_CCTLN_OUTMOD_7; //CCR1 Reset\set
    TIMER_A2 ->CCR[1] = DC_LCD; //duty cycle
    //TIMER_A1 ->CCTL[2] =TIMER_A_CCTLN_OUTMOD_7; //CCR2 reset/set
    //TIMER_A1 ->CCR[2] = 250;// CCR2 duty cycle
    TIMER_A2->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP |TIMER_A_CTL_CLR;
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


}

/*
 * This function displays a screen to prompt the user to setup the time,
 * gets keypad input from user to set the time and displays
 * the keypad input as the user enters a valid number. The time is then
 * written to the RTC device.
 */
void set_time_date() //make sure delete the manually set date line in RTC_write
{

 /*   * After user inputs, unsigned char calendar[13] has form:
     * __________________________________
     * |M M D D Y Y DOW H H M M   S   S |
     * |0 1 2 3 4 5  6  7 8 9 10  11  12|
     * |________________________________|
     *
     */
     char str_set_month[20];
     char str_set_day[20];
     char str_set_year[20];
     char str_set_dow[20];
     char str_set_hour[20];
     char str_set_min[20];
     char str_set_sec[20];
     char temp [1];

int cur_row=8;
display_set_time_date();
ST7735_DrawString(0,5,"Enter two digits for", ST7735_GREEN);
ST7735_DrawString(0,6,"Month. Example:", ST7735_GREEN);
ST7735_DrawString(0,7,"01->Jan / 12->Dec", ST7735_GREEN);
reset_flag();
//****************************************************************************************************************************************

    while(month_flag == 0){ //stay here unless flag is trigger, ck_valid will set mon. flag to 1 if it's valid input

        ST7735_DrawString(0,++cur_row,"Enter month:",ST7735_GREEN); //Print Enter month:__
        calendar[0] = keypad_getkey();              //get first digit in char form of month
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[0]);
        ST7735_DrawString(15,cur_row,temp,ST7735_GREEN);
        calendar[1] = keypad_getkey();              //get second digit in char form of month
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[1]);
        ST7735_DrawString(16,cur_row++,temp,ST7735_GREEN);
        ck_valid();
        if(month_flag == 0)
        {   display_set_time_date();
            ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
            cas_sysDelay(1);
            ST7735_DrawString(0,cur_row++,"Re-enter Month",ST7735_RED);
            cas_sysDelay(1);
            clearScreen();
            cur_row=7;
        }
    }
    //calendar will be ASCII char - 48 to gain its decimal form
    // *16 is for converting to its hex form so the slave device can read
    date[5] = (calendar[0]-48)*16+(calendar[1]-48);         //date will be transmitted to slave device
    printMonthLCD(date[5]);                        //get month form Jan-Dec for what user just enetered instead from RTC_register
    sprintf(str_set_month, "You entered: %s ",return_char_month);
    ST7735_DrawString(0,++cur_row,str_set_month,ST7735_GREEN);
    cas_sysDelay(1);
    //str_set_month use strcpy to store month info to it then print use drawstring to print on lcd
    //check printMonthLCD for reference

    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for day
    reset_flag();
    clearScreen();
    cur_row=8;
    display_set_time_date();
    ST7735_DrawString(0,5,"Enter two digits for", ST7735_GREEN);
    ST7735_DrawString(0,6,"Day. EXAMPLE:", ST7735_GREEN);
    ST7735_DrawString(0,7,"01-> 1st day of month", ST7735_GREEN);

    while(day_flag ==0){
        ST7735_DrawString(0,cur_row,"Enter Day:",ST7735_GREEN); //Print Enter day:__
        calendar[2] = keypad_getkey();              //get first digit of day
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[2]);
              ST7735_DrawString(15,cur_row,temp,ST7735_GREEN);
        calendar[3] = keypad_getkey();              //get second digit of day
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[3]);
        ST7735_DrawString(16,cur_row++,temp,ST7735_GREEN);
        ck_valid();
        if(day_flag == 0)
        {   display_set_time_date();
            ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
            cas_sysDelay(1);
            ST7735_DrawString(0,cur_row++,"Re-enter Day",ST7735_RED);
            cas_sysDelay(1);
            clearScreen();
            display_set_time_date();
            cur_row=7;
        }
    }


    date[4] = (calendar[2]-48)*16+(calendar[3]-48);         //date[4]- day info will be sent to RTC thru RTC_Write
    //store date [4] in to str_set_day and print it to inform user for what they entered
    sprintf(str_set_day, "You entered: %x ", date[4]);
    ST7735_DrawString(0,++cur_row,str_set_day,ST7735_GREEN);
    cas_sysDelay(1);

//    printf(" Press # to confirm");             //prompt user to press # to confirm
//    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for year
    reset_flag();
    clearScreen();
    display_set_time_date();
    cur_row=8;
    ST7735_DrawString(0,5,"Enter two digits for :", ST7735_GREEN);
    ST7735_DrawString(0,6,"Year. Example:", ST7735_GREEN);
    ST7735_DrawString(0,7,"17-> year:2017", ST7735_GREEN);
    while(year_flag==0)
    {
        ST7735_DrawString(0,cur_row,"Enter Year:",ST7735_GREEN); //Print Enter year:__
        calendar[4] = keypad_getkey();              //get first digit of year
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[4]);
            ST7735_DrawString(15,cur_row,temp,ST7735_GREEN);
        calendar[5] = keypad_getkey();              //get second digit of year
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[5]);
        ST7735_DrawString(16,cur_row++,temp,ST7735_GREEN);
        ck_valid();

    if(year_flag == 0)
    {   display_set_time_date();
        ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
        cas_sysDelay(1);
        ST7735_DrawString(0,cur_row++,"Re-enter year",ST7735_RED);
        cas_sysDelay(1);
        clearScreen();
        display_set_time_date();
        cur_row=7;
    }
    }

    date[6] = (calendar[4]-48)*16+(calendar[5]-48); //date[6] will be sent to RTC through RTC_write
    sprintf(str_set_year, "You entered: 20%x ", date[6]);

    ST7735_DrawString(0,++cur_row,str_set_year,ST7735_GREEN);
    cas_sysDelay(1);

//    printf(" Press # to confirm");             //prompt user to press # to confirm
//    while(keypad_getkey()!='#');                             //stay here until confirmed
//****************************************************************************************************************************************

//Gather info for day of the week
    reset_flag();
    clearScreen();
    display_set_time_date();
    ST7735_DrawString(0,5,"Enter one digits for:", ST7735_GREEN);
    ST7735_DrawString(0,6,"day of week. Example:", ST7735_GREEN);
    ST7735_DrawString(0,7,"1-> Sun./ 7->Sat.", ST7735_GREEN);

    cur_row=8;
    while(DOW_flag ==0)
    {
    ST7735_DrawString(0,cur_row,"Enter Day of week:",ST7735_GREEN); //Print Enter year:__
    calendar[6] = keypad_getkey();
    set_count(0);                               //reset idle count
    sprintf(temp, "%c",calendar[6]);
                  ST7735_DrawString(19,cur_row,temp,ST7735_GREEN);
    //draw character that was just entered at calendar[6] same row
    ck_valid();
    if(DOW_flag == 0)
    {      display_set_time_date();
           ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
           cas_sysDelay(1);
           ST7735_DrawString(0,cur_row++,"Re-enter the day",ST7735_RED);
           cas_sysDelay(1);
           clearScreen();
           display_set_time_date();
           cur_row=7;
       }

    }

    date[3] = (calendar[6]-48);
    printDayLCD(date[3]);                        //get month form Sun-Sat for what user just enetered instead from RTC_register
    sprintf(str_set_dow, "You entered: %s ",return_char_day);
    ST7735_DrawString(0,++cur_row,str_set_dow,ST7735_GREEN);
     cas_sysDelay(1);

    //str_set_dow use strcpy to store dow info on it then print use drawstring to print on same row
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for hour

     reset_flag();
     clearScreen();
     display_set_time_date();
     ST7735_DrawString(0,5,"Enter two digits for", ST7735_GREEN);
     ST7735_DrawString(0,6,"Hour. ", ST7735_GREEN);
     cur_row=7;
    while(hour_flag == 0)
    {
        ST7735_DrawString(0,cur_row,"Enter Hour:",ST7735_GREEN); //Print Enter hour:__
        calendar[7] = keypad_getkey();              //get first digit of hour
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[7]);
              ST7735_DrawString(11,cur_row,temp,ST7735_GREEN);
        calendar[8] = keypad_getkey();              //get second digit of hour
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[8]);
        ST7735_DrawString(12,cur_row++,temp,ST7735_GREEN);
        ck_valid();
        if(hour_flag == 0)
        {   display_set_time_date();
            ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
            cas_sysDelay(1);
            ST7735_DrawString(0,cur_row++,"Re-enter hour",ST7735_RED);
            cas_sysDelay(1);
            clearScreen();
            display_set_time_date();
            cur_row=7;
        }
    }

    date[2] = (calendar[7]-48)*16+(calendar[8]-48);
    sprintf(str_set_hour, "You entered: %x ", date[2]);
       ST7735_DrawString(0,++cur_row,str_set_hour,ST7735_GREEN);
       cas_sysDelay(1);
    //store date [2] in to str_set_hour and print it to inform user for what they entered
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed
//****************************************************************************************************************************************
//Gather info for minute
       reset_flag();
       clearScreen();
       display_set_time_date();
       ST7735_DrawString(0,5,"Enter two digits for", ST7735_GREEN);
       ST7735_DrawString(0,6,"Minute. ", ST7735_GREEN);

       cur_row=7;
    while(min_flag ==0)
    {

        ST7735_DrawString(0,cur_row,"Enter min:",ST7735_GREEN); //Print Enter min:__
        calendar[9] = keypad_getkey();              //get first digit of min
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[9]);
            ST7735_DrawString(11,cur_row,temp,ST7735_GREEN);
        calendar[10] = keypad_getkey();              //get second digit of min
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[10]);
            ST7735_DrawString(12,cur_row++,temp,ST7735_GREEN);
        ck_valid();
        if(min_flag == 0)
        {   display_set_time_date();
            ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
            cas_sysDelay(1);
            ST7735_DrawString(0,cur_row++,"Re-enter min",ST7735_RED);
            cas_sysDelay(1);
            clearScreen();
            display_set_time_date();
            cur_row=7;
        }
    }

    date[1] = (calendar[9]-48)*16+(calendar[10]-48);
    sprintf(str_set_min, "You entered: %x ", date[1]);
          ST7735_DrawString(0,++cur_row,str_set_min,ST7735_GREEN);
          cas_sysDelay(1);
    //store date [1] in to str_set_min and print it to inform user for what they entered
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');                             //stay here until confirmed

//****************************************************************************************************************************************
//Gather info for second
          reset_flag();
          clearScreen();
          display_set_time_date();
          ST7735_DrawString(0,5,"Enter two digits.", ST7735_GREEN);
          ST7735_DrawString(0,6,"Second. ", ST7735_GREEN);
          cur_row=7;
    while(sec_flag==0)
    {
        ST7735_DrawString(0,cur_row,"Enter sec:",ST7735_GREEN); //Print Enter sec:__
        calendar[11] = keypad_getkey();              //get first digit of min
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[11]);
            ST7735_DrawString(11,cur_row,temp,ST7735_GREEN);
        calendar[12] = keypad_getkey();              //get second digit of min
        set_count(0);                               //reset idle count
        sprintf(temp, "%c",calendar[12]);
            ST7735_DrawString(12,cur_row++,temp,ST7735_GREEN);
        ck_valid();
        if(sec_flag == 0)
        {   display_set_time_date();
            ST7735_DrawString(0,++cur_row,"Invalid input",ST7735_RED);
            cas_sysDelay(1);
            ST7735_DrawString(0,cur_row++,"Re-enter sec",ST7735_RED);
            cas_sysDelay(1);
            clearScreen();
            display_set_time_date();
            cur_row=7;
        }
    }

    date[0] = (calendar[11]-48)*16+(calendar[12]-48);
    sprintf(str_set_sec, "You entered: %x ", date[0]);
              ST7735_DrawString(0,++cur_row,str_set_sec,ST7735_GREEN);
              cas_sysDelay(1);
    //store date [0] in to str_set_sec and print it to inform user for what they entered
    //    printf(" Press # to confirm");             //prompt user to press # to confirm
    //    while(keypad_getkey()!='#');

}


void display_menu_LCD()
{
    clearScreen();

    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    ST7735_DrawString2(x, y, "Main", textColor, bgColor);
    ST7735_DrawString2(x, y+=2, "Menu", textColor, bgColor);

    ST7735_DrawString(x, y+=3, get_lock()? "1. Unlock Door" : "1. Lock Door", textColor);

    ST7735_DrawString(x, y+=1,  get_armed()? "2. Disarm Alarm" : "2. Arm Alarm", textColor);

    ST7735_DrawString(x, y+=1, "3. Check Sensors", textColor);

    ST7735_DrawString(x, y+=1, "4. View Logs", textColor);

    ST7735_DrawString(x, y+=1, "5. Change Time", textColor);

    ST7735_DrawString(x, y+=1, "6. Change Password", textColor);
}
void clearScreen(){
    ST7735_FillScreen(0x0000);            // set screen to black
}

void ck_valid()
{ //Maximum calendar: 1 2 3 1 9 9  7  2 3 5   9    5    9
  //                  M M D D Y Y DOW H H min min  sec  sec
    if (((calendar[0] - 48) * 10 + (calendar[1] - 48)) <= 12 && calendar[0]!='*' &&calendar[1]!='*'&& calendar[0]!='#' &&calendar[1]!='#')
    {
        month_flag = 1; //valid
    }
    if (((calendar[2] - 48) * 10 + (calendar[3] - 48)) <= 31&& calendar[2]!='*' &&calendar[3]!='*'&& calendar[2]!='#' &&calendar[3]!='#')
    {
        day_flag = 1;
    }

    if (((calendar[4] - 48) * 10 + (calendar[5] - 48)) <= 99 && calendar[4]!='*' &&calendar[5]!='*'&& calendar[4]!='#' &&calendar[5]!='#')
    {
        year_flag = 1;
    }
    if (((calendar[6] - 48)) <= 7 && ((calendar[6]-48))>= 1&& calendar[6]!='*'&& calendar[6]!='#')
    {
        DOW_flag = 1;
    }

    if (((calendar[7] - 48) * 10 + (calendar[8] - 48)) < 24&& calendar[7]!='*' &&calendar[8]!='*'&& calendar[7]!='#' &&calendar[8]!='#')
    {
        hour_flag = 1;
    }
    if (((calendar[9] - 48) * 10 + (calendar[10] - 48)) < 60 && calendar[9]!='*' &&calendar[10]!='*'&& calendar[9]!='#' &&calendar[10]!='#')
    {
        min_flag = 1;
    }

    if (((calendar[11] - 48) * 10 + (calendar[12] - 48)) < 60 && calendar[11]!='*' &&calendar[12]!='*'&& calendar[11]!='#' &&calendar[12]!='#')
    {
        sec_flag = 1;
    }

}

void reset_flag() //reset input checking flags
{
     month_flag=0;
     day_flag=0;
     year_flag=0;
     DOW_flag=0;
     hour_flag=0;
     min_flag=0;
     sec_flag=0;
}

void arm_error_LCD(){
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_RED;
    ST7735_FillScreen(bgColor);
    int y=5;
    ST7735_DrawString_bg(2,y, "Cannot arm system.", textColor, bgColor);
    ST7735_DrawString_bg(2,y+=2, "Clear all triggers", textColor, bgColor);
    ST7735_DrawString_bg(3,y+=1, "then try again.", textColor, bgColor);
}

void arm_success_LCD(){
    int16_t textColor = ST7735_RED;
    int16_t bgColor = ST7735_WHITE;
    ST7735_FillScreen(bgColor);
    int y=5;
    ST7735_DrawString2(5,y, "System", textColor, bgColor);
    ST7735_DrawString2(6,y+3, "Armed", textColor, bgColor);
}

void disarm_success_LCD(){
    int16_t textColor = ST7735_GREEN;
    int16_t bgColor = ST7735_WHITE;
    ST7735_FillScreen(bgColor);
    int y=5;
    ST7735_DrawString2(5,y, "System", textColor, bgColor);
    ST7735_DrawString2(3,y+3, "Disarmed", textColor, bgColor);
}

void trigger_LCD(int trigger){
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_RED;
    ST7735_FillScreen(bgColor);
    int y=2;
    ST7735_DrawString2(3,y, "WARNING!", textColor, bgColor);
    switch(trigger){
        case DOOR:
            ST7735_DrawString2(2,y+=3, "Door Open", textColor, bgColor);
            break;
        case WINDOW:
            ST7735_DrawString2(4,y+=3, "Window", textColor, bgColor);
            ST7735_DrawString2(6,y+=3, "Open", textColor, bgColor);
            break;
        case TEMPERATURE:
            ST7735_DrawString2(6,y+=3, "Fire", textColor, bgColor);
            ST7735_DrawString2(2,y+=3, "Detected", textColor, bgColor);
            break;
        case PRESENCE:
            ST7735_DrawString2(2,y+=3, "Presence", textColor, bgColor);
            ST7735_DrawString2(2,y+=3, "Detected", textColor, bgColor);
            break;
    }
    ST7735_DrawString_bg(2, 12, "Press any key to", textColor, bgColor);
    ST7735_DrawString_bg(2, 13, "continue.", textColor, bgColor);
}
