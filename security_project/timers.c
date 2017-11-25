#include <timers.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#define WDT_A_TIMEOUT RESET_SRC_1
#define SYSTICK_INTERRUPT_PERIOD_MS 250
volatile int flag_wdt=0;
volatile int pre_flag=0;
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

void Init_wdt()
{
    if(MAP_ResetCtl_getSoftResetSource() & WDT_A_TIMEOUT)
    {


        while(1)
        {
            go_home();
        }
    }
        /* Setting MCLK to REFO at 128Khz for LF mode and SMCLK to REFO */
        MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
        MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_1);
        MAP_PCM_setPowerState(PCM_AM_LF_VCORE0);

        /* Configuring WDT to timeout after 8192K iterations of SMCLK, at 128k,
         * this will roughly equal 64 seconds*/
        MAP_SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
        MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,
                                    WDT_A_CLOCKITERATIONS_512K); //8192K

        MAP_SysTick_enableModule();


        MAP_SysTick_setPeriod((uint32_t)((float)MAP_CS_getMCLK()*(float)SYSTICK_INTERRUPT_PERIOD_MS*0.001));
        MAP_SysTick_enableInterrupt();

       /* Enabling interrupts and starting the watchdog timer*/

       MAP_Interrupt_enableMaster();

       MAP_WDT_A_startTimer();
       while (1)
           {
       //        MAP_PCM_gotoLPM0();
       //        //print flag on LCD
       //        MAP_WDT_A_clearTimer(); // Call when a key is pressed
               keypad_getkey();
               flag_wdt=0;
               MAP_WDT_A_clearTimer(); // Call when a key is pressed
           }
       }



/* SysTick ISR - This ISR will fire every 60s. Having this ISR fire will wake
    up the device from LPM0 and the WDT_A will be serviced in the main loop */
void SysTick_Handler(void)
{
    static int count;
    count++;
    if(count ==4 ){
        flag_wdt++;
        count=0;
    }
    return;
}
