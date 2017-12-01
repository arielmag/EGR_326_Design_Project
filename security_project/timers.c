#include <timers.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"

//volatile int user_timeout = 0;
extern volatile int count = 0;
static volatile uint32_t aclk, mclk, smclk, hsmclk, bclk;

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
}

void get_clock(){
    aclk = CS_getACLK();
    mclk = CS_getMCLK();
    smclk = CS_getSMCLK();
    hsmclk = CS_getHSMCLK();
    bclk = CS_getBCLK();
}


void T32_INT1_IRQHandler(void)
{
    MAP_Timer32_clearInterruptFlag(TIMER32_BASE);
    MAP_Timer32_setCount(TIMER32_BASE,48000000);
    count++;
    if(count ==60 ){
        set_timeout(1);
        //user_timeout = 1;//when 60s is counted, telling main by setting user_timeout flag to 1.
        count=0;
    }
    return;

}
