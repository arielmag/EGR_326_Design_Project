#include <sensors.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "alarm.h"

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
    return P2->IN&BIT5;
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

    if(status & GPIO_PIN5){ // if interrupt came from pin 2.5 (door)
        set_triggered(DOOR);

    }else if(status & GPIO_PIN4){ // if interrupt came from pin 2.4 (window)
        set_triggered(WINDOW);
    }
}
