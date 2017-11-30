/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

/* Custom Includes */
#include "alarm.h"
#include "keypad.h"
#include "LCD.h"
#include "motor.h"
#include "RTC.h"
#include "sensors.h"
#include "ST7735.h"
#include "timers.h"
#include "buzzer.h"

void reset_system();
void display_home_screen();
int check_reset();
void setup_system();
void enter_password();
void display_menu();
void setup_password();
void setup_time();
int verify_entry();
void display_log();
void display_menu();
void display_unlocked();
void display_arm_disarm_log();
void display_trigger_log();
void go_home();
void check_sensors();
char default_password[] = {'a', 'a', 'a', 'a'}; // Default password for the system
char saved_password[4]= {'1', '2', '3', '1'}; // 4-digit password for system
char typed_password[4];
extern volatile int user_timeout;
extern int count;
int main(void)
{
    MAP_WDT_A_holdTimer();      // Stop the Watchdog timer
    Init48MHz();                // Set MCLK to 48MHz
    I2C_init();                 // Initialize I2C protocal for RTC Communication
    Init_PIR();                 // Initialize ports for PIR sensor
    Init_RTC();                 // Initialize ports for RTC module
    Init_keypad();              // Initialize ports for keypad
    Init_hall();                // Initialize ports for the hall effect sensors
    Init_motor();               // Initialize ports for the motor
    Init_LCD();                 // Initialize ports for the LCD
    Init_alarm();               // Initialize alarm
    SysTick_Init();             // Initialize SysTick timer
    init_user_input_WDT_timer();// Initialize WDT timer for idle state
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
    get_clock();
=======

/*Commented out by Don Nov. 22 for saving time to debug particular functions.
    reset_system(); // This function causes the system to be reset.
                    // It should be run the first time this program is being set up
                    // to trigger events for user to set up time and password.
                    // Otherwise, comment this out.

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password
    }
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log

    // If system already setup
    go_home();
*/
    while(1)
        {
            go_home();

<<<<<<< HEAD

/*Commented out by Don Nov. 22 for saving time to debug particular functions.
    reset_system(); // This function causes the system to be reset.
                    // It should be run the first time this program is being set up
                    // to trigger events for user to set up time and password.
                    // Otherwise, comment this out.

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password
=======
        }
}

/*
 * Display home screen and start system logic
 */
void go_home(){
/* commented out by Don Nov 22 due to stuck too in getkey result in screen can't be updated with 1 sec change
    display_home_screen();      // Display screen for home
    while(keypad_getkey() != ENTER_KEY );    // Wait for user to press enter # to switch screens
 */
    user_timeout = 0;
    clearScreen();
   while(check_pressed()==0)
   {
       display_home_screen();
   }

    enter_password();       // Prompt user to enter password

    while(1)
    {
        display_menu();         // Display the menu screen. Different options within
                                // this function will change the screen displayed.
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
    }
}

<<<<<<< HEAD
=======
=======
>>>>>>> parent of 7e165f4... LCD backlight activated

/*Commented out by Don Nov. 22 for saving time to debug particular functions.
    reset_system(); // This function causes the system to be reset.
                    // It should be run the first time this program is being set up
                    // to trigger events for user to set up time and password.
                    // Otherwise, comment this out.

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password
    }

>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
    // If system already setup
    go_home();
*/
    while(1)
        {
            go_home();

        }
}
<<<<<<< HEAD

/*
 * Display home screen and start system logic
 */
void go_home(){
/* commented out by Don Nov 22 due to stuck too in getkey result in screen can't be updated with 1 sec change
    display_home_screen();      // Display screen for home
    while(keypad_getkey() != ENTER_KEY );    // Wait for user to press enter # to switch screens
 */
    user_timeout = 0;
    clearScreen();
   while(check_pressed()==0)
   {
       display_home_screen();
   }

=======

/*
 * Display home screen and start system logic
 */
void go_home(){
/* commented out by Don Nov 22 due to stuck too in getkey result in screen can't be updated with 1 sec change
    display_home_screen();      // Display screen for home
    while(keypad_getkey() != ENTER_KEY );    // Wait for user to press enter # to switch screens
 */
    user_timeout = 0;
    clearScreen();
   while(check_pressed()==0)
   {
       display_home_screen();
   }

>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
    enter_password();       // Prompt user to enter password

    while(1)
    {
        display_menu();         // Display the menu screen. Different options within
                                // this function will change the screen displayed.
    }
}

/*
 * This function resets the system. It sets the password to a "default" value
 * which causes the user to setup a new password and time. This function should
 * be run the very first time the system is used or whenever a reset is desired.
 */
void reset_system(){

    // Set password to default
    int i;
    for(i=0; i<4; i++){
        saved_password[i] = default_password[i];
    }

    // TODO: Clear everything stored in memory
}

=======
/*
 * This function resets the system. It sets the password to a "default" value
 * which causes the user to setup a new password and time. This function should
 * be run the very first time the system is used or whenever a reset is desired.
 */
void reset_system(){

    // Set password to default
    int i;
    for(i=0; i<4; i++){
        saved_password[i] = default_password[i];
    }

    // TODO: Clear everything stored in memory
}

>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
/*
 * This function displays the home screen with the time, date,
 * alarm status, and temperature
 */
void display_home_screen(){
    display_home_screen_LCD();
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD

=======
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
    /*
    ST7735_FillScreen(0);    // set screen to black
    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;
    char titleString[] = "home";
    ST7735_DrawString2(x, y, titleString, textColor, bgColor);
    y += 3;
    char timeString[] = "12:00:00";
    ST7735_DrawString(x, y, timeString, textColor);
    y += 1;
    char dateString[] = "November 17, 2017";
    ST7735_DrawString(x, y, dateString, textColor);
    y += 1;
    char tempString[] = "75 F";
    ST7735_DrawString(x, y, tempString, textColor);
    y += 3;
    char statusString[] = "Alarm Status: Disarmed";
    ST7735_DrawString(x, y, statusString, textColor);


     */
<<<<<<< HEAD
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
    /*
    ST7735_FillScreen(0);    // set screen to black
    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;
    char titleString[] = "home";
    ST7735_DrawString2(x, y, titleString, textColor, bgColor);
    y += 3;
    char timeString[] = "12:00:00";
    ST7735_DrawString(x, y, timeString, textColor);
    y += 1;
    char dateString[] = "November 17, 2017";
    ST7735_DrawString(x, y, dateString, textColor);
    y += 1;
    char tempString[] = "75 F";
    ST7735_DrawString(x, y, tempString, textColor);
    y += 3;
    char statusString[] = "Alarm Status: Disarmed";
    ST7735_DrawString(x, y, statusString, textColor);


     */
>>>>>>> parent of 7e165f4... LCD backlight activated
}

/*
 * This function checks if the system is reset. This is done by checking if the
 * password is set to a "default" value.
 *
 * @return 1 if system is reset, 0 if not reset
 */
int check_reset(){

    // Check if password is default (assume setup already)
    int i, is_reset = 1;
    for(i=0; i<4; i++){
        if(saved_password[i] != default_password[i])
            is_reset = 0; // Set to 0 if any password value is not default
    }

    return is_reset;
}

/*
 * This function sets up a system when the time and password have not been stored.
 */
void setup_system(){
    setup_time();   // Set up the time
    RTC_write();    // Set up the input in RTC
    RTC_read();     // Clear out junk values for first read
    setup_password();   // Set up the password
}

/*
 * This function displays a screen to prompt the user to setup the time,
 * gets keypad input from user to set the time and displays
 * the keypad input as the user enters a valid number. The time is then
 * written to the RTC device.
 */
void setup_time(){
    set_time_date(); //the function allows user to enter info for the array date[sec, min, hour, day, date, month, year]
}

/*
 * This function displays the setup password screen and gets keypad input from the
 * user to set the four-digit password and displays *s as the user presses valid keys.
 * The function then prompts the user to enter the password a second time to verify
 * the two passwords match. When verified, the password is stored in password_array.
 */
void setup_password(){

    /*
     * Set up the security system by creating a four-digit password.
     * ****
     */
    ST7735_FillScreen(0);    // set screen to black
    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;
    ST7735_DrawString2(x, y, "Setup", textColor, bgColor);
    ST7735_DrawString2(x, y+=2, "Password", textColor, bgColor);
    ST7735_DrawString(x, y+=2, "Set up the system by", textColor);
    ST7735_DrawString(x, y+=1, "creating a 4-digit", textColor);
    ST7735_DrawString(x, y+=1, "password.", textColor);

    int i;
    x=20,y=80;
    for(i=0; i<4; i++){
        // Don't count * or # keypad entries as a digit
        do{
            saved_password[i] = keypad_getkey();

        }while(saved_password[i] == ENTER_KEY || saved_password[i] == HOME_KEY);

        // Display * as password characters are typed
        ST7735_DrawCharS(x, y, '*', textColor, bgColor, 2);
        x+=25;
    }

    ST7735_FillScreen(0);    // set screen to black
    x=0, y=0;
    ST7735_DrawString2(x, y, "Verify", textColor, bgColor);
    ST7735_DrawString2(x, y+=2, "Password", textColor, bgColor);
    ST7735_DrawString(x, y+=2, "Re-enter your 4-digit", textColor);
    ST7735_DrawString(x, y+=1, "password.", textColor);

    // Check for re-entry
    while(!verify_entry(1)){
        ST7735_FillScreen(0);    // set screen to black
        x=0, y=0;
        ST7735_DrawString2(x, y, "Verify", textColor, bgColor);
        ST7735_DrawString2(x, y+=2, "Password", textColor, bgColor);
        int16_t textColor = ST7735_RED;
        ST7735_DrawString(x, y+=2, "Password incorrect,", textColor);
        ST7735_DrawString(x, y+=1, "try again.", textColor);
    }
}

/*
 * This function displays the screen for entering the password, including the
 * prompt to tell the user to enter a password
 */
void enter_password(){
    ST7735_FillScreen(0);    // set screen to black
    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;
    ST7735_DrawString2(x, y, "Password", textColor, bgColor);
    ST7735_DrawString(x, y+=2, "Please enter your", textColor);
    ST7735_DrawString(x, y+=1, "4-digit password.", textColor);
    while(!verify_entry(0)){ // Check if the user entered the saved password
        x=0, y=0;
        ST7735_FillScreen(0);    // set screen to black
        ST7735_DrawString2(x, y, "Password", textColor, bgColor);
        int16_t textColor = ST7735_RED;
        ST7735_DrawString(x, y+=2, "Password incorrect,", textColor);
        ST7735_DrawString(x, y+=1, "try again.", textColor);
    }
}

/*
 * This function gets user entry for a 4-digit password and checks it against the
 * saved password.
 *
 * @param isFirstSetup 1 if it is the first time setting up the system, 0 if not
 * @return 1 if the saved password was entered, 0 if incorrect
 */
int verify_entry(int isFirstSetup){

    int i, is_verified = 1;
    int16_t x=20,y=80;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    for(i=0; i<4; i++){

        // Don't count * or # keypad entries as a digit
        do{
            typed_password[i] = keypad_getkey();

            // If the user presses home and this is not the first time setting up the system, go home
            if(typed_password[i] == HOME_KEY && !isFirstSetup)
                go_home();

        }while(typed_password[i] == ENTER_KEY || typed_password[i] == HOME_KEY);

        // Display * as password characters are typed
        ST7735_DrawCharS(x, y, '*', textColor, bgColor, 2);
        x+=25;

        // Check if character matches saved password
        if(typed_password[i] != saved_password[i]) //Nov 22 Saved password is default to 1231 for debug by Don
            is_verified = 0;

    }

    return is_verified;
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
    init_LED2(); //TODO HOW to implement hall sensor to change LED on/off status?
    char menu =keypad_getkey();
    count=0; //after every key pressed, reset the WDT idle timer and recount second
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
            setup_time();
            RTC_write();    // Set up the input in RTC
            RTC_read();     // Clear out junk values for first read
            break;

        // 6 to view log
        case '6':
            enter_password(); //verify the password first before reset
            setup_password();
            break;
        //7  to play alarm for debug
        case '7':
            tone1();
            break;
        // '*' to go back to home
        case '*':
            go_home();
    }
}

/*
 * Display screen for choosing logs in the format:
 *
 * View Log
 * 1. View Arm/Disarm Events
 * 2. View Trigger Events
 * 3. Return to Menu
 */
void display_log(){
    //TODO display on LCD

    switch(keypad_getkey()){
        // 1 to view arm/disarm log
        case '1':
            display_arm_disarm_log();
            break;

        // 2 to view trigger events
        case '2':
            display_trigger_log();
            break;

        // 3 to return to menu
        case '3':
            display_menu();
    }
}

/*
 * Display arm/disarm events log in the format:
 *
 * Arm/Disarm Events
 * DD/MM/YY HH:MM:SS ARMED
 * DD/MM/YY HH:MM:SS DISARMED
 * ...
 */
void display_arm_disarm_log(){

}

/*
 * Display trigger events log in the format:
 *
 * Alarm Trigger Events
 * DD/MM/YY HH:MM:SS TEMPERATURE
 * DD/MM/YY HH:MM:SS WINDOW
 * ...
 */
void display_trigger_log(){

}

/*
 * Display the status of each of the sensors in the format:
 *
 * Sensor Status
 * Door             OPEN
 * Window           CLOSED
 * Temperature      79 F
 * Presence         DETECTED
 */
void check_sensors(){
    ST7735_FillScreen(0);    // set screen to black
    uint16_t x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;
    char key;

    do{
        x=0, y=0;
        ST7735_DrawString2(x, y, "Sensor", textColor, bgColor);
        ST7735_DrawString2(x, y+=2, "Status", textColor, bgColor);
        ST7735_DrawString(x, y+=3, "Door", textColor);
        ST7735_DrawString(x+12, y, get_door_status() ? "OPEN  " : "CLOSED", textColor);

        ST7735_DrawString(x, y+=1, "Window", textColor);
        ST7735_DrawString(x+12, y, get_window_status() ? "OPEN  " : "CLOSED", textColor);

        ST7735_DrawString(x, y+=1, "Temperature", textColor);
        char array[10];
        sprintf(array, "%0.2f F", RTC_read_temperature());
        ST7735_DrawString(x+12, y, array, textColor);

        ST7735_DrawString(x, y+=1, "Presence", textColor);
        ST7735_DrawString(x+12, y, check_PIR() ? "DETECTED" : "NONE    ", textColor);

        ST7735_DrawString(x, y+=2, "Press any digit to", textColor);
        ST7735_DrawString(x, y+=1, "refresh.", textColor);

        key = keypad_getkey();
    }while(key != HOME_KEY && key != ENTER_KEY);

    if(key == HOME_KEY)
        go_home();

    if(key == ENTER_KEY)
        display_menu();
}
