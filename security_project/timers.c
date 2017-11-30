#include <timers.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
<<<<<<< HEAD
volatile int user_timeout=0;
int count=0;
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 7e165f4... LCD backlight activated
=======

volatile int user_timeout = 0;
int count = 0;

>>>>>>> parent of a9580e5... Merged both changes, finished arm/disarm
/*
 * Initialize the SysTick timer.
 */
void SysTick_Init(){
    SysTick -> CTRL = 0; // disable SysTick during setup
    SysTick -> LOAD = 0x00FFFFFF; // maximum reload value
    SysTick -> VAL = 0; // any write to current value clears it
    SysTick -> CTRL = 0x00000005; // enable SysTick, CPU clk, no interrupts
}


void SysTick_delay(uint16_t delay){
    SysTick -> LOAD = ((delay*48000) - 1); // every 48000 counts take 1ms, extra one cycle for keeping the counts
    SysTick -> VAL = 0; // any write to CVR clears it
    // and COUNTFLAG in CSR
    // Wait for flag to be SET (Timeout happened)
    while((SysTick -> CTRL & 0x00010000) == 0);
}

/*
 * Configurable Systick delay to use with 48MHz clock
 *
 * @param sec Time to delay in seconds
 */
void cas_sysDelay (uint16_t sec)
{   volatile int i;
    for(i=0; i<sec*3; i++)
    {
        SysTick_delay(230); //change 230 to 333 to be exact. 250 was experimentally find optimal for 1 sec change for LCD display
    }
}

/*
 * Set the clock module to use the external 48 MHz crystal.
 */
void Init48MHz(){
    /* Configuring pins for peripheral/crystal usage */
    MAP_GPIO_setAsPeripheralModuleFunctionOutputPin(GPIO_PORT_PJ,GPIO_PIN3 | GPIO_PIN2, GPIO_PRIMARY_MODULE_FUNCTION);
    CS_setExternalClockSourceFrequency(32000,48000000); // enables getMCLK, getSMCLK to know externally set frequencies
    /* Starting HFXT in non-bypass mode without a timeout. Before we start
     *
    * we have to change VCORE to 1 to support the 48MHz frequency */
    MAP_PCM_setCoreVoltageLevel(PCM_VCORE1);
    MAP_FlashCtl_setWaitState(FLASH_BANK0, 2);
    MAP_FlashCtl_setWaitState(FLASH_BANK1, 2);
    CS_startHFXT(false); // false means that there are no timeouts set,will return when stable
    /* Initializing MCLK to HFXT (effectively 48MHz) */
    MAP_CS_initClockSignal(CS_MCLK, CS_HFXTCLK_SELECT, CS_CLOCK_DIVIDER_1);
}

<<<<<<< HEAD

<<<<<<< HEAD
<<<<<<< HEAD
void init_user_input_WDT_timer()
{
<<<<<<< HEAD
    /*WDT in interval mdoe is used as a timer counter for trigger idle status*/
=======
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log

void init_user_input_WDT_timer()
{
    /*WDT in interval mdoe is used as a timer counter for trigger idle status
<<<<<<< HEAD
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
    /*WDT in interval mdoe is used as a timer counter for trigger idle status
>>>>>>> parent of 7e165f4... LCD backlight activated

    /* Setting ACLK to REFO at 128Khz for LF mode @ 128KHz*/
       MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
       MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
       MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

       /* Configuring WDT in interval mode to trigger every 128K clock iterations.
        * This comes out to roughly every 4 seconds */
       MAP_WDT_A_initIntervalTimer(WDT_A_CLOCKSOURCE_ACLK,
                                   WDT_A_CLOCKITERATIONS_512K); //4s

       /* Enabling interrupts and starting the watchdog timer */
       MAP_Interrupt_enableInterrupt(INT_WDT_A);
       MAP_Interrupt_enableMaster();
       MAP_WDT_A_startTimer();

}
void WDT_A_IRQHandler(void)
{
<<<<<<< HEAD
<<<<<<< HEAD

    count++;
    if(count ==15 )
    {
        user_timeout = 1;//when 60s is counted, telling main by setting user_timeout flag to 1.
        count=0;
    }
//     char string[50]; //debug purpose
//     sprintf(string, "IRQ %i", count);
//     ST7735_DrawString(0,1,string,ST7735_GREEN);

=======
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log

    count++;
    if(count ==15 )
    {
        user_timeout = 1;//when 60s is counted, telling main by setting user_timeout flag to 1.
        count=0;
    }
//     char string[50]; //debug purpose
//     sprintf(string, "IRQ %i", count);
//     ST7735_DrawString(0,1,string,ST7735_GREEN);

<<<<<<< HEAD
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
    return;
}
=======
//void init_user_input_WDT_timer(){
//
//        // WDT in interval mode is used as a timer counter for trigger idle status
//
//        /* Setting ACLK to REFO at 128Khz for LF mode @ 128KHz*/
//        MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
//        MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
//        MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);
//
//        /* Configuring WDT in interval mode to trigger every 128K clock iterations.
//        * This comes out to roughly every 4 seconds */
//        MAP_WDT_A_initIntervalTimer(WDT_A_CLOCKSOURCE_ACLK,
//                                WDT_A_CLOCKITERATIONS_512K); //4s
//
//        /* Enabling interrupts and starting the watchdog timer*/
//        MAP_Interrupt_enableInterrupt(INT_WDT_A);
//        MAP_Interrupt_enableMaster();
//
//        MAP_WDT_A_startTimer();
//}
//
//
//
//void WDT_A_IRQHandler(void)
// {
//     count++;
//     if(count ==15 ){
//         user_timeout = 1;//when 60s is counted, telling main by setting user_timeout flag to 1.
//         count=0;
//     }
//     return;
// }
>>>>>>> parent of a9580e5... Merged both changes, finished arm/disarm
