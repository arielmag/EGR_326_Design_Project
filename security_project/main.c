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

char default_password[] = {'a', 'a', 'a', 'a'}; // Default password for the system
char saved_password[4]; // 4-digit password for system
char enter_key = '#'; // Key to set as enter button

int main(void)
{
    MAP_WDT_A_holdTimer();      // Stop the Watchdog timer
    Init48MHz();                // Set MCLK to 48MHz
    Init_PIR();                 // Initialize ports for PIR sensor
    Init_RTC();                 // Initialize ports for RTC module
    Init_keypad();              // Initialize ports for keypad
    Init_hall();                // Initialize ports for the hall effect sensors
    Init_motor();               // Initialize ports for the motor

    reset_system(); // This function causes the system to be reset.
                    // It should be run the first time this program is being set up
                    // to trigger events for user to set up time and password.
                    // Otherwise, comment this out.

    display_home_screen();      // Display screen for home

    while(keypad_getkey() != enter_key);    // Wait for user to press enter to switch screens

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password

    }else{                      // If system already setup
        enter_password();       // Prompt user to enter password
    }

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
    // TODO
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
 * This function displays a screen to prompt the user to setup the timem
 * gets keypad input from user to set the time and displays
 * the keypad input as the user enters a valid number. The time is then
 * written to the RTC device.
 */
void setup_time(){

}

/*
 * This function displays the setup password screen and gets keypad input from the
 * user to set the four-digit password and displays *s as the user presses valid keys.
 * The function then prompts the user to enter the password a second time to verify
 * the two passwords match. When verified, the password is stored in password_array.
 */
void setup_password(){

    //TODO Will need to use verify_entry() to check if passwords match
}

/*
 * This function displays the screen for entering the password, including the
 * prompt to tell the user to enter a password
 */
void enter_password(){
    while(!verify_entry()){ // Check if the user entered the saved password
        // TODO: Display message on LCD saying the password entered did not match, try again
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
    for(i=0; i<4; i++){
        typed_password[i] = keypad_getkey();
        // TODO: Display '*'
        if(typed_password[i] != saved_password[i])
            is_verified = 0;
    }

    return is_verified;
}

/*
 * This function displays a menu with options:
 * Lock/Unlock Door
 * Arm/Disarm Alarm
 * Check Sensors (Door/Window/Temperature/Presence)
 * View Log
 * Change Time
 * It returns to the home screen if idle for more than 1 minute. The user can scroll
 * through the menu items using the up and down buttons and press an option by pressing
 * the enter button. Pressing the back button returns to the home screen.
 */
void display_menu(){
    // TODO
}
