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

//extern volatile int user_timeout;
//extern int count;

void setup_system();

static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;

int main(void)
{
    MAP_WDT_A_holdTimer();      // Stop the Watchdog timer
    ADC_Init();                 // Initialize ADC
    Init48MHz();                // Set MCLK to 48MHz
    I2C_init();                 // Initialize I2C protocal for RTC Communication
    Init_PIR();                 // Initialize ports for PIR sensor
    Init_RTC();                 // Initialize ports for RTC module
    Init_keypad();              // Initialize ports for keypad
    Init_hall();                // Initialize ports for the hall effect sensors
    Init_motor();               // Initialize ports for the motor
    Init_LCD();                 // Initialize ports for the LCD
    init_user_input_WDT_timer();// Initialize WDT timer for idle state

    get_clock();

    init_LED2();

    SysTick_Init();             // Initialize the SysTick timer
    Init_alarm();               // Initialize alarm



         aclk = CS_getACLK();
         mclk = CS_getMCLK();
         smclk = CS_getSMCLK();
         hsmclk = CS_getHSMCLK();
         bclk = CS_getBCLK();


    // Note: Hall effect sensor detection is commented out for testing, test with PIR only

    // Hold down P1.1 on startup to reset system
    if(!(P1IN&BIT1)){
        reset_system();
    }

    if(check_reset()){          // Check if system is reset
        setup_system();         // Let the user setup time and password
    }

    // When system is set up
    while(1){

        go_home();
    }
}

/*
 * This function sets up a system when the time and password have not been stored.
 */
void setup_system(){
    set_time_date();   // Set up the time
    RTC_write();    // Set up the input in RTC
    RTC_read();     // Clear out junk values for first read
    setup_password();   // Set up the password
}
