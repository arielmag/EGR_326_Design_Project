#include <alarm.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"
#include "sensors.h"
#include "RTC.h"
#include "LCD.h"
#include "timers.h"
#include "ST7735.h"
#include "string.h"
#include "buzzer.h"

char default_password[] = {'a', 'a', 'a', 'a'}; // Default password for the system

char saved_password[4];

char read_back_data_1[86];

char typed_password[4];

// Flag for if system is triggered
int isTriggered;

// Flag for if alarm is armed/diarmed
int isArmed = 0;

// Pointer to address in flash for reading back values
uint8_t* addr_pointer;

// Log of arm/disarm events.
// 0 Number of events (0 to 5)
// 1-8  Event 1 TYPE MM DD YY DOW HH:MM:SS
// 9-16 Event 2 TYPE MM DD YY DOW HH:MM:SS
// 17-24 Event 3 TYPE MM DD YY DOW HH:MM:SS
// 25-32 Event 4 TYPE MM DD YY DOW HH:MM:SS
// 33-40 Event 5 TYPE MM DD YY DOW HH:MM:SS
char arm_disarm_log[41];

// Log of trigger events.
// 0 Number of events (0 to 5)
// 1-8  Event 1 TYPE MM DD YY DOW HH:MM:SS
// 9-16 Event 2 TYPE MM DD YY DOW HH:MM:SS
// 17-24 Event 3 TYPE MM DD YY DOW HH:MM:SS
// 25-32 Event 4 TYPE MM DD YY DOW HH:MM:SS
// 33-40 Event 5 TYPE MM DD YY DOW HH:MM:SS
char trigger_log[41];

char data_to_store[86];

/*
 * Initialize the alarm by setting the system to disarmed.
 */
void Init_alarm(){
    P1SEL0 &= ~BIT1;
    P1SEL1 &= ~BIT1; // 1) configure P1.0 as GPIO
    P1DIR &= ~BIT1; // 2) make P1.0 inputs
    P1REN |= BIT1; // 3) enable pull resistors on P1.0
    P1OUT |= BIT1; // P1.0 are pull-up

    set_armed(0); // Disarm system and no triggers
    set_trigger_status(NONE);
    green();

    get_saved_password();
    get_saved_arm_disarm_log();
    get_saved_trigger_log();
}

/*
 * This function arms/disarms the alarm system. If any triggers are present,
 * the system will not be allowed to be armed.
 * @return 1 if system armed/disarmed successfully, 0 if trigger present so
 *           system was not armed
 */
int arm_disarm_alarm(){

    // Arm the system if currently disarmed and not triggered, disarm the system if armed
    if(get_armed() || (!get_armed() && !get_triggered())){
        set_armed(!get_armed());

        log_arm_disarm_time();

        if(get_armed()){
            arm_success_LCD();
            red();
            set_trigger_status(NONE);

        }else{
            sounder_off();
            disarm_success_LCD();
            green();
            set_trigger_status(NONE);
        }
        cas_sysDelay(2);
        return 1;
    }

    arm_error_LCD();
    cas_sysDelay(2);
    return 0;
}

/*
 * Clear the arm/disarm log in memory and array
 */
void clear_arm_disarm_log(){
    int i;

    for(i=0; i<41; i++){
        arm_disarm_log[i] = 0;
    }

    get_saved_trigger_log();

    get_saved_password();

    write_to_memory();

}

/*
 * Clear the trigger log in memory and array
 */
void clear_trigger_log(){
    int i;

    for(i=0; i<41; i++){
        trigger_log[i] = 0;
    }

    get_saved_arm_disarm_log();

    get_saved_password();

    write_to_memory();
}

/*
 * This function is called when the alarm is armed or disarmed. It puts the time
 * in the log of arm and disarm events.
 */
void log_arm_disarm_time(){

    int i;

    // Get the time and put in rtc_registers
    RTC_read();
    RTC_read();

    // Get previous log
    get_saved_arm_disarm_log();

    // Check number of logs stored
    if(arm_disarm_log[0]<5){

        for(i=0;i<8;i++){

            // Store event type
            if(i==0){
                arm_disarm_log[i+(arm_disarm_log[0]*8)+1] = isArmed;

            // Put read date on end
            }else{
                arm_disarm_log[i+(arm_disarm_log[0]*8)+1] = RTC_registers[i-1];
            }
        }

        // Increment number of logs stored
        arm_disarm_log[0] += 1;

    // Need to shift dates and put new one in first 7 spots if 5 logs stored already
    }else{

        // Shift info up 7 spots
        for(i=1;i<(40-8);i++){
            arm_disarm_log[i] = arm_disarm_log[i+8];
        }

        for(i=0;i<8;i++){ // Add new date

            if(i==0){
                arm_disarm_log[i+33] = isArmed;
            }else{
                arm_disarm_log[i+33] = RTC_registers[i-1];
            }
        }
    }

    get_saved_password();
    get_saved_trigger_log();

    write_to_memory();
}

/*
 * This function is called once when the system is armed and triggered. It puts
 * the time in the log of trigger events.
 */
void log_trigger_time(int triggerType){
    int i;

    // Get the time and put in rtc_registers
    RTC_read();
    RTC_read();

    // Get previous log
    get_saved_trigger_log();

    // Check number of logs stored
    if(trigger_log[0]<5){

        for(i=0;i<8;i++){

            // Store event type
            if(i==0){
                trigger_log[i+(trigger_log[0]*8)+1] = triggerType;

            // Put read date on end
            }else{
                trigger_log[i+(trigger_log[0]*8)+1] = RTC_registers[i-1];
            }
        }

        // Increment number of logs stored
        trigger_log[0] += 1;

    // Need to shift dates and put new one in first 7 spots if 5 logs stored already
    }else{

        // Shift info up 7 spots
        for(i=1;i<(40-8);i++){
            trigger_log[i] = trigger_log[i+8];
        }

        for(i=0;i<8;i++){ // Add new date

            if(i==0){
                trigger_log[i+33] = triggerType;
            }else{
                trigger_log[i+33] = RTC_registers[i-1];
            }
        }
    }

    get_saved_password();
    get_saved_arm_disarm_log();
    write_to_memory();
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

    // Save password in memory
    save_password();

    // Clear logs
    clear_arm_disarm_log();
    clear_trigger_log();
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
 * This function saves the password to non-volatile memory
 */
void save_password(){
    get_saved_arm_disarm_log();
    get_saved_trigger_log();

    write_to_memory();
}


/*
 * Get 4-digit password from memory and store in char saved_password[4]
 */
void get_saved_password(){
    addr_pointer = CALIBRATION_START+4; // point to address in flash for saved data

    int i;
    for(i=0; i<4; i++) {
        saved_password[i] = *addr_pointer++;
    }
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

        }while(saved_password[i] == BACK_KEY || saved_password[i] == HOME_KEY);

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

    // Save password in memory
    save_password();
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
            set_count(0); //each time a value is entered, reset count

            // If the user presses home and this is not the first time setting up the system, go home
            if(typed_password[i] == HOME_KEY || typed_password[i] == BACK_KEY && !isFirstSetup)
                go_home();

        }while(typed_password[i] == BACK_KEY || typed_password[i] == HOME_KEY);

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
 * Get the log of arm/disarm events from memory and store in char trigger_log[66]
 */
void get_saved_arm_disarm_log(){
    addr_pointer = CALIBRATION_START+4+4; // point to address in flash for saved data

    int i;
    for(i=0; i<41; i++) {
        arm_disarm_log[i] = *addr_pointer++;
    }
}

/*
 * Get the log of trigger events from memory and store in char trigger_log[66]
 */
void get_saved_trigger_log(){
    addr_pointer = CALIBRATION_START+4+45; // point to address in flash for saved data

    int i;
    for(i=0; i<41; i++) {
        trigger_log[i] = *addr_pointer++;
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

    char key;

    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    ST7735_FillScreen(0);    // set screen to black

    do{
        get_saved_arm_disarm_log();

        int x=0, y=0, i;
        ST7735_DrawString2(x, y, "Arm/Disarm", textColor, bgColor);
        ST7735_DrawString2(x, y+=2, "Events", textColor, bgColor);

        char print_str[50];

        y+=2;

        if(arm_disarm_log[0] == 0){
            ST7735_DrawString(x, y+=2, "There are no arm/", textColor);
            ST7735_DrawString(x, y+=1, "disarm events to", textColor);
            ST7735_DrawString(x, y+=1, "display.", textColor);

        }else if(arm_disarm_log[0] <6){
            for(i=0;i<arm_disarm_log[0];i++){
                    sprintf(print_str,"%02x/%02x/%02x %02x:%02x:%02x", arm_disarm_log[7+(i*8)], arm_disarm_log[6+(i*8)], arm_disarm_log[8+(i*8)], arm_disarm_log[4+(i*8)], arm_disarm_log[3+(i*8)], arm_disarm_log[2+(i*8)]);
                    ST7735_DrawString(0, y+=1, print_str, ST7735_WHITE);

                    switch(arm_disarm_log[1+(i*8)]){
                        case 1:
                            ST7735_DrawString(0, y+=1, "    Type: ARM", ST7735_WHITE);
                        break;

                        case 0:
                            ST7735_DrawString(0, y+=1, "    Type: DISARM", ST7735_WHITE);
                        break;
                    }
            }
        }

        key = keypad_getkey();
        set_count(0);

    }while(key != HOME_KEY && key != BACK_KEY);

    if(key == HOME_KEY)
        go_home();

    if(key == BACK_KEY)
        display_log();
}

/*
 * Display trigger events log in the format:
 *
 * Alarm Trigger Events
 * DD/MM/YY HH:MM:SS
 * DD/MM/YY HH:MM:SS
 * ...
 */
void display_trigger_log(){

    char key;

    do{
        int x=0, y=0, i;
        int16_t textColor = ST7735_WHITE;
        int16_t bgColor = ST7735_BLACK;

        ST7735_FillScreen(0);    // set screen to black

        get_saved_trigger_log();

        ST7735_DrawString2(x, y, "Trigger", textColor, bgColor);
        ST7735_DrawString2(x, y+=2, "Events", textColor, bgColor);

        char print_str[50];

        y+=2;

        if(trigger_log[0] == 0){
            ST7735_DrawString(x, y+=2, "There are no trigger", textColor);
            ST7735_DrawString(x, y+=1, "events to display.", textColor);
        }else if(trigger_log[0] <6){
            for(i=0;i<trigger_log[0];i++){
                    sprintf(print_str,"%02x/%02x/%02x %02x:%02x:%02x", trigger_log[7+(i*8)], trigger_log[6+(i*8)], trigger_log[8+(i*8)], trigger_log[4+(i*8)], trigger_log[3+(i*8)], trigger_log[2+(i*8)]);
                    ST7735_DrawString(0, y+=1, print_str, ST7735_WHITE);

                    switch(trigger_log[1+(i*8)]){
                        case DOOR:
                            ST7735_DrawString(0, y+=1, "    Type: DOOR", ST7735_WHITE);
                        break;

                        case WINDOW:
                            ST7735_DrawString(0, y+=1, "    Type: WINDOW", ST7735_WHITE);
                        break;

                        case TEMPERATURE:
                            ST7735_DrawString(0, y+=1, "    Type: TEMPERATURE", ST7735_WHITE);
                        break;

                        case PRESENCE:
                            ST7735_DrawString(0, y+=1, "    Type: PRESENCE", ST7735_WHITE);
                        break;
                    }
            }
        }

        ST7735_DrawString(x, y+=2, "Press any digit to", textColor);
        ST7735_DrawString(x, y+=1, "refresh.", textColor);

        key = keypad_getkey();
        set_count(0);

    }while(key != HOME_KEY && key != BACK_KEY);

    if(key == HOME_KEY)
        go_home();

    if(key == BACK_KEY)
        display_log();
}

/*
 * This function sets the status of the alarm
 * @param arm_type 1 armed, 0 disarmed
 */
void set_armed(int arm_type){
    isArmed = arm_type;
}

/*
 * This function returns the status of the alarm
 * @return 1 armed, 0 disarmed
 */
int get_armed(){
    return isArmed;
}

/*
 * This function sets the status of the trigger
 * @param trigger_type 1 triggered, 0 not triggered
 */
void set_trigger_status(int trigger_type){
    isTriggered = trigger_type;
}

/*
 * This function returns the status of the trigger
 * @return 1 triggered, 0 not triggered
 */
int get_trigger_status(){
    return isTriggered;
}

/*
 * This function gets the status of the alarm trigger
 * @return 1 triggered 0 not triggered
 */
int get_triggered(){
    return (check_PIR() || RTC_read_temperature() > 110 );
    //return (check_PIR() || get_door_status() || get_window_status() || RTC_read_temperature() > 110 );
}

/*
 * Display screen for choosing logs in the format:
 *
 * View Log
 * 1. View Arm/Disarm Events
 * 2. View Trigger Events
 */
void display_log(){

    int x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    ST7735_FillScreen(0);    // set screen to black

    while(1){

        ST7735_DrawString2(x, y, "View Log", textColor, bgColor);

        ST7735_DrawString(0, y+=3, "1. View Arm/Disarm", textColor);
        ST7735_DrawString(0, y+=1, "   Events", textColor);

        ST7735_DrawString(0, y+=2, "2. View Trigger", textColor);
        ST7735_DrawString(0, y+=1, "   Events", textColor);

    //    ST7735_DrawString(0, y+=2, "Note: Any other key", textColor);
    //    ST7735_DrawString(0, y+=1, "      return to menu", textColor);
        char log_select = keypad_getkey();
        set_count(0); //each option will clear

        switch(log_select){
            // 1 to view arm/disarm log
            case '1':
                display_arm_disarm_log();
                break;

            // 2 to view trigger events
            case '2':
                display_trigger_log();
                break;

            // Enter key to return to menu
            case BACK_KEY:
                display_menu();
        }
    }
}

/*
 * Write contents of char data_to_store[] to memory
 */
void write_to_memory(){
    uint8_t i; // index

    addr_pointer = CALIBRATION_START+4; // point to address in flash for saved data

    for(i=0; i<86; i++){
        read_back_data_1[i] = *addr_pointer++;
    }

    // Put items for trigger log, arm/disarm log, and password in data_to_store array
    for(i=0; i<86; i++){

        // Put trigger log in array
        if(i>44){
            data_to_store[i] = trigger_log[i-45];

        // Put arm/disarm log in array
        }else if(i>3){
            data_to_store[i] = arm_disarm_log[i-4];

        // Put password in array
        }else{
            data_to_store[i] = saved_password[i];
        }
    }

    /* Unprotecting Info Bank 0, Sector 0 */
    MAP_FlashCtl_unprotectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);

    /* Erase the flash sector starting CALIBRATION_START. */
    while(!MAP_FlashCtl_eraseSector(CALIBRATION_START));

    /* Program the flash with the new data. */
    while (!MAP_FlashCtl_programMemory(data_to_store, (void*) CALIBRATION_START+4, 86 )); // leave first 4 bytes unprogrammed

    /* Setting the sector back to protected */
    MAP_FlashCtl_protectSector(FLASH_INFO_MEMORY_SPACE_BANK0,FLASH_SECTOR0);

    addr_pointer = CALIBRATION_START+4; // point to address in flash for saved data

    for(i=0; i<86; i++) {// read values in flash after programming
        read_back_data_1[i] = *addr_pointer++;
    }

    get_saved_arm_disarm_log();

    get_saved_trigger_log();

    get_saved_password();

    //display_arm_disarm_log();
}
