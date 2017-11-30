#include <alarm.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"
#include "sensors.h"
#include "RTC.h"
#include "LCD.h"
#include "timers.h"

// Flag for if alarm is armed/diarmed
int isArmed = 0;

/*
 * Initialize the alarm by setting the system to disarmed.
 */
void Init_alarm(){
    isArmed = 0; // Disarm system
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
<<<<<<< HEAD
        get_armed() ? arm_success_LCD() : disarm_success_LCD();
=======

        if(get_armed()){
            arm_success_LCD();
            red();

        }else{
            disarm_success_LCD();
            green();
            set_trigger_status(0);
        }
>>>>>>> parent of a9580e5... Merged both changes, finished arm/disarm
        cas_sysDelay(2);
        return 1;
    }

    arm_error_LCD();
    cas_sysDelay(2);
    return 0;
}

/*
 * This function is called when the alarm is armed or disarmed. It puts the time
 * in the log of arm and disarm events.
 */
void log_arm_disarm_time(){
    // getArmed();
    // TODO put the time of arm/disarm in log with appropriate flag for arm/disarm
}

/*
 * This function is called once when the system is armed and triggered. It puts
 * the time in the log of trigger events.
 */
void log_trigger_time(){

}

/*
 * This function is called when the system is armed and triggered.
 * The alarm makes noises continuously until alarm_off() is called
 * or power is removed from the system.
 */
void sound_alarm(){


}

/*
 * This function is called turns the alarm off after a system trigger.
 */
void alarm_off(){

}

/*
 * This function sets the status of the alarm
 * @param isArmed 1 armed, 0 disarmed
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
 * This function gets the status of the alarm trigger
 * @return 1 triggered 0 not triggered
 */
int get_triggered(){
<<<<<<< HEAD
    return (check_PIR() || get_door_status() || get_window_status() || abs(RTC_read_temperature() - 110) > 0.01 );
=======
    return (check_PIR() || get_door_status() || get_window_status() || RTC_read_temperature() > 110 );
}

/*
 * Display screen for choosing logs in the format:
 *
 * View Log
 * 1. View Arm/Disarm Events
 * 2. View Trigger Events
 */
void display_log(){
    //TODO display on LCD

    int x=0, y=0;
    int16_t textColor = ST7735_WHITE;
    int16_t bgColor = ST7735_BLACK;

    ST7735_FillScreen(0);    // set screen to black

    ST7735_DrawString2(x, y, "View Log", textColor, bgColor);

    ST7735_DrawString(0, y+=3, "1. View Arm/Disarm", textColor);
    ST7735_DrawString(0, y+=1, "   Events", textColor);

    ST7735_DrawString(0, y+=2, "2. View Trigger", textColor);
    ST7735_DrawString(0, y+=1, "   Events", textColor);


    switch(keypad_getkey()){
        // 1 to view arm/disarm log
        case '1':
            display_arm_disarm_log();
            break;

        // 2 to view trigger events
        case '2':
            display_trigger_log();
            break;

        // Enter key to return to menu
        case ENTER_KEY:
            display_menu();
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
>>>>>>> parent of a9580e5... Merged both changes, finished arm/disarm
}
