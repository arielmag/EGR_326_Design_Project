#include <timers.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"
#include "RTC.h"
#include "alarm.h"
#include "LED.h"
#include "sensors.h"
#include "buzzer.h"

//volatile int user_timeout = 0;
extern volatile int count = 0;
static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;
int static buzzer_flag = 0;
void set_count(int value){
    count = value;
}

int get_count(){
    return count;
}

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
        SysTick_delay(333); //change 230 to 333 to be exact. 250 was experimentally find optimal for 1 sec change for LCD display
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

void init_user_input_timer32(){
    /* Configuring Timer32 to 128000 (1s) of MCLK in periodic mode */
     MAP_Timer32_initModule(TIMER32_BASE, TIMER32_PRESCALER_1, TIMER32_32BIT,
                            TIMER32_FREE_RUN_MODE);
     MAP_Timer32_setCount(TIMER32_BASE,48000000);
     MAP_Timer32_enableInterrupt(TIMER32_BASE);
     MAP_Timer32_startTimer(TIMER32_BASE, true);
     /* Enabling interrupts */
     MAP_Interrupt_enableInterrupt(INT_T32_INT1);
     MAP_Interrupt_enableMaster();

     set_temp_trig(0);
}

void get_clock(){
    aclk = CS_getACLK();
    mclk = CS_getMCLK();
    smclk = CS_getSMCLK();
    hsmclk = CS_getHSMCLK();
    bclk = CS_getBCLK();
}

void init_WDT()
{

    /* Setting ACLK to REFO at 128Khz*/
    MAP_CS_setReferenceOscillatorFrequency(CS_REFO_128KHZ);
    MAP_CS_initClockSignal(CS_ACLK, CS_REFOCLK_SELECT, CS_CLOCK_DIVIDER_128);
    //MAP_PCM_setPowerState(PCM_AM_LF_VCORE0); //low power mode will effect performance

    /* Configuring WDT to timeout after 512k iterations of SMCLK, at 128k,
     * this will roughly equal 4 seconds*/
    MAP_SysCtl_setWDTTimeoutResetType(SYSCTL_SOFT_RESET);
    MAP_WDT_A_initWatchdogTimer(WDT_A_CLOCKSOURCE_ACLK,
                                WDT_A_CLOCKITERATIONS_2G);

    MAP_WDT_A_startTimer(); //start watchdog timer

}




int get_temp_trig(){
    return temp_trig;
}

void set_temp_trig(int status){
    temp_trig = status;
}

void T32_INT1_IRQHandler(void) //idle state detector, trigger every second
{
    MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
    MAP_Timer32_setCount(TIMER32_BASE,48000000);
    MAP_ADC14_toggleConversionTrigger(); //every second trigger ADC conversion
    count++;
    if(count ==60 ){
        set_timeout(1);
        //user_timeout = 1;//when 60s is counted, telling main by setting user_timeout flag to 1.
        count=0;
    }

    RTC_read();

    //get_temperature();

    // Check for temperature trigger
    if(get_temperature() >= 90 && get_temp_trig() == 0){
        set_trigger_status(TEMPERATURE);
        log_trigger_time(TEMPERATURE);
        flashing_red();
        set_temp_trig(1);
    }

    if(get_temperature() < 90 && get_temp_trig() == 1){
        set_temp_trig(0);
    }

    // Make sure there are no triggers present already
    if(get_trigger_status() == NONE){
        // Only check door and window triggers if armed
        if(get_armed()){
            if(P2IN & BIT5){
                set_trigger_status(DOOR);
                log_trigger_time(DOOR);
                flashing_red();
            }

            if(P2IN & BIT4){
                set_trigger_status(WINDOW);
                log_trigger_time(WINDOW);
                flashing_red();
            }

        }

    }


    if( get_trigger_status() != NONE) //every second, check if the system is triggered and armed
        {
        P6->OUT &=~BIT7;
        toggle_red(); //if it is triggered, and the system is armed. Toggle red LED

             if (buzzer_flag)
            {
            pwm_buzzer_high();
             buzzer_flag =0;
             }
             else
            {
                pwm_buzzer_low();
                buzzer_flag =1;
            }

        }

    set_update_scroll(1);

    return;

}

