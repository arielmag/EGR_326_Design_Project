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
        get_armed() ? arm_success_LCD() : disarm_success_LCD();
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
    return (check_PIR() || get_door_status() || get_window_status() || abs(RTC_read_temperature() - 110) > 0.01 );
}
