/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>

/* Custom Includes */
#include "alarm.h"
#include "keypad.h"
#include "LCD.h"
#include "motor.h"
#include "RTC.h"
#include "sensors.h"
#include "ST7735.h"
#include "timers.h"

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

char default_password[] = {'a', 'a', 'a', 'a'}; // Default password for the system
char saved_password[4]; // 4-digit password for system
char enter_key = '#'; // Key to set as enter button
char home_key = '*'; // Key to set as home key

int main(void)
{
    MAP_WDT_A_holdTimer();      // Stop the Watchdog timer
    Init48MHz();                // Set MCLK to 48MHz
    Init_PIR();                 // Initialize ports for PIR sensor
    Init_RTC();                 // Initialize ports for RTC module
    Init_keypad();              // Initialize ports for keypad
    Init_hall();                // Initialize ports for the hall effect sensors
    Init_motor();               // Initialize ports for the motor
    Init_LCD();                 // Initialize ports for the LCD
    Init_alarm();               // Initialize alarm
    SysTick_Init();

/*
    reset_system(); // This function causes the system to be reset.
                    // It should be run the first time this program is being set up
                    // to trigger events for user to set up time and password.
                    // Otherwise, comment this out.

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password
    }
*/



    // If system already setup
    go_home();

}

/*
 * Display home screen and start system logic
 */
void go_home(){
    display_home_screen();      // Display screen for home
    while(keypad_getkey() != enter_key);    // Wait for user to press enter to switch screens
    enter_password();       // Prompt user to enter password

    while(1){
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

/*
 * This function displays the home screen with the time, date,
 * alarm status, and temperature
 */
void display_home_screen(){
    display_home_screen_LCD();
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
    // TODO

     */
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
    setup_password();   // Set up the password
}

/*
 * This function displays a screen to prompt the user to setup the time,
 * gets keypad input from user to set the time and displays
 * the keypad input as the user enters a valid number. The time is then
 * written to the RTC device.
 */
void setup_time(){
    set_time_date();
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
    int i;

    for(i=0; i<4; i++){
        // Don't count * or # keypad entries as a digit
        do{
            saved_password[i] = keypad_getkey();
        }while(saved_password[i] == '*' || saved_password[i] == '#');
        // print '*'
    }

    //TODO
    /*
     * Re-Enter your password
     */

    // Check for re-entry
    while(!verify_entry());

    /*
     * Password saved
     */
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
    char titleString[] = "Enter ";
    ST7735_DrawString2(x, y, titleString, textColor, bgColor);
    x=0, y=2;
    char titleString2[] = "Password";
    ST7735_DrawString2(x, y, titleString2, textColor, bgColor);
    y+=10;
    /*
    char descrptionString[] = "Enter your 4-digit password.";
    ST7735_DrawString(x, y, descriptionString, textColor);
    */
    while(!verify_entry()){ // Check if the user entered the saved password
        // TODO: Display message on LCD saying the password entered did not match, try again
        /*
         * Password incorrect, try again.
         */
    }
}

/*
 * This function gets user entry for a 4-digit password and checks it against the
 * saved password.
 *
 * @return 1 if the saved password was entered, 0 if incorrect
 */
int verify_entry(){
    char typed_password[4];
    int i, is_verified = 1;
    int16_t x=20,y=60;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    for(i=0; i<4; i++){

        // Don't count * or # keypad entries as a digit
        do{
            typed_password[i] = keypad_getkey();
        }while(typed_password[i] == '*' || typed_password[i] == '#');

        // TODO: Display '*'

        ST7735_DrawCharS(x, y, '*', textColor, bgColor, 2);
        x+=25;
        if(typed_password[i] != saved_password[i])
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

    switch(keypad_getkey()){
        // 1 to lock/unlock door
        case '1':
            lock_unlock_door();
            break;

        // 2 to arm/disarm alarm
        case '2':
            arm_disarm_alarm();
            break;

        // 3 to check sensors
        case '3':
            arm_disarm_alarm();
            break;

        // 4 to view log
        case '4':
            display_log();
            break;

        // 5 to change time
        case '5':
            setup_time();
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

