#include <alarm.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"

// Flag for if alarm is armed/diarmed
int isArmed = 0;

int isTriggered = 0;

/*
 * Initialize the alarm by setting the system to disarmed and clearing triggers.
 */
void Init_alarm(){
    isArmed = 0; // Disarm system
    set_triggered(0); // Remove triggers
}

/*
 * This function arms/disarms the alarm system. If any triggers are present,
 * the system will not be allowed to be armed.
 * @return 1 if system armed/disarmed successfully, 0 if trigger present so
 *           system was not armed
 */
int arm_disarm_alarm(){
    // Arm the system if currently disarmed, disarm the system if armed and not triggered
    if(get_armed() == 0 || (get_armed() == 1) && (get_triggered() == NONE)){
        set_armed(!get_armed());
        log_arm_disarm_time();
        // TODO print "System is armed/disarmed."
        // delay, then return to menu
        return 1;
    }

    // If trying to arm the system but there is a trigger
    // TODO print "Unable to arm alarm. Please "
    switch(get_triggered()){
        case DOOR:
            // TODO print "close door"
            break;
        case WINDOW:
            // TODO print "close window"
            break;
        case TEMPERATURE:
            // TODO print "lower temperature"
            break;
        case PRESENCE:
            // TODO print "move away from presence detector"
            break;
    }
    // TODO print "then try again. Press any button to continue."
    while(keypad_getkey());
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
 * This function sets the status of the alarm trigger
 * @param triggerType 0 not triggered
 *                    1 door open
 *                    2 window open
 *                    3 temperature >100 F
 *                    4 presence
 */
void set_triggered(int triggerType){
    isTriggered = triggerType;
    if(triggerType != NONE)
        log_trigger_time();
}

/*
 * This function gets the status of the alarm trigger
 * @return 0 not triggered
 *         1 door open
 *         2 window open
 *         3 temperature >100 F
 */
int get_triggered(){
    return isTriggered;
}
