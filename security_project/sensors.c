#include <sensors.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "alarm.h"
<<<<<<< HEAD
<<<<<<< HEAD
<<<<<<< HEAD
volatile uint16_t curADCResult;
volatile float normalizedADCRes;
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 7e165f4... LCD backlight activated

/*
 * Initialize pins for the the PIR sensor.
 * P2.7 Input to read sensor
 */
void Init_PIR(){
    P2SEL0 &= ~BIT(7);
    P2SEL1 &= ~BIT(7); //conf as GPIO
    P2DIR &= ~BIT(7); // make inputs
}

/*
 * Checks the status of the PIR sensor on P2.7.
 *
 * @return 1 presence detected, 0 no detection
 */
int check_PIR(){
    return P2->IN&0x80;
}

/*
 * Initialize pins for the hall effect sensors.
 * P2.5 Door hall effect sensor
 * P2.4 Window hall effect sensor
 */
void Init_hall(){
    /* Configuring P2.5 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN5);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN5);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN5);

    /* Configuring P2.4 as an input and enabling interrupts */
    MAP_GPIO_setAsInputPinWithPullUpResistor(GPIO_PORT_P2, GPIO_PIN4);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, GPIO_PIN4);
    MAP_GPIO_enableInterrupt(GPIO_PORT_P2, GPIO_PIN4);

    /* Enable port 2 interrupts */
    MAP_Interrupt_enableInterrupt(INT_PORT2);

    /* Enabling MASTER interrupts */
    MAP_Interrupt_enableMaster();
}

/*
 * Get the status for the hall effect sensor on the door
 * @return 1 open, 0 closed
 */
int get_door_status(){

    if (!(P2->IN&BIT5))
    {
        return CLOSED;
    }
    else
    {
    return OPEN; }

}

/*
 * Get the status for the hall effect sensor on the window
 * @return 1 open, 0 closed
 */
int get_window_status(){
    return P2->IN&BIT4;
}

/*
 * GPIO ISR for port 2 (door and window hall effect sensors)
*/
void PORT2_IRQHandler(void)
{
    uint32_t status;

    status = MAP_GPIO_getEnabledInterruptStatus(GPIO_PORT_P2);
    MAP_GPIO_clearInterruptFlag(GPIO_PORT_P2, status);

<<<<<<< HEAD
    if(status & GPIO_PIN5){ // if interrupt came from pin 2.5 (door)
        //set_triggered(DOOR);
        //TODO Don: How can we use interrupt to change LED status? on/off

    }else if(status & GPIO_PIN4){ // if interrupt came from pin 2.4 (window)
        //set_triggered(WINDOW);
=======
    // If armed and not triggered, check for triggers
    if( !get_trigger_status() && get_armed()){

        if(status & GPIO_PIN5){ // if interrupt came from pin 2.5 (door)
                //TODO Don: How can we use interrupt to change LED status? on/off
                flashing_red();
                set_trigger_status(1);
                log_trigger_time(DOOR);

        }else if(status & GPIO_PIN4){ // if interrupt came from pin 2.4 (window)
                flashing_red();
                set_trigger_status(1);
                log_trigger_time(WINDOW);

        }else if(status & GPIO_PIN7){ // if interrupt came from pin 2.7 (PIR)
                flashing_red();
                set_trigger_status(1);
                log_trigger_time(PRESENCE);

        }
>>>>>>> parent of a9580e5... Merged both changes, finished arm/disarm
    }
}
void green()
{
P2->OUT &= ~(BIT0|BIT1|BIT2); //turn off the bits for LED control
P2->OUT ^= BIT1;
}
void red()
{
P2->OUT &= ~(BIT0|BIT1|BIT2); //turn off the bits for LED control
P2->OUT ^= BIT0;
}
void init_LED2()
{
    P2->DIR |= BIT0 | BIT1 | BIT2; //initialize on board LED2
    P2 ->OUT = 0;
<<<<<<< HEAD
<<<<<<< HEAD
}
<<<<<<< HEAD

void ADC_Init() //credit: ADC14 MSP432ware
{
    //Initializing ADC (MCLK/1/4)
MAP_ADC14_enableModule();
MAP_ADC14_initModule(ADC_CLOCKSOURCE_MCLK, ADC_PREDIVIDER_1, ADC_DIVIDER_4,0);

    //Configuring GPIOs (5.5,A0) Analog input A0 (one of 32) is on P5.5
MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P5, GPIO_PIN5, GPIO_TERTIARY_MODULE_FUNCTION);

    //Configure ADC memory
MAP_ADC14_configureSingleSampleMode(ADC_MEM0,true); //repeat mode continuous sampling
MAP_ADC14_configureConversionMemory(ADC_MEM0,ADC_VREFPOS_AVCC_VREFNEG_VSS, ADC_INPUT_A0, false);
    //false indicates differential mode, if true, uses 2 consecutive channels

    //Configure sample timer
MAP_ADC14_enableSampleTimer(ADC_MANUAL_ITERATION); //requires a trigger for each sample * changed to automatic

    //Enabling/Toggling conversion
MAP_ADC14_enableConversion();
MAP_ADC14_toggleConversionTrigger();

    //Enabling Interrupts
MAP_ADC14_enableInterrupt(ADC_INT0); //interrupt when conversion is complete
MAP_Interrupt_enableInterrupt(INT_ADC14);
MAP_Interrupt_enableMaster();
}
void ADC14_IRQHandler(void)
{
    uint64_t status = MAP_ADC14_getEnabledInterruptStatus();
    //load the bits indicating which ADC channel requested interrupt
    MAP_ADC14_clearInterruptFlag(status);//clear interrupt flag

    if(ADC_INT0 & status)
    {
        curADCResult = MAP_ADC14_getResult(ADC_MEM0);
        normalizedADCRes = (curADCResult * 3.3 ) / 16384; //assume 3.3 V ref. 14 bits
//        MAP_ADC14_toggleConversionTrigger(); //start the next ADC conversion

    }
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
=======
>>>>>>> parent of 4da6ed5... Merge pull request #1 from arielmag/Log
}
=======
>>>>>>> parent of 7e165f4... LCD backlight activated
