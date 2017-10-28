#include <sensors.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>

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
 * Initialize pins for the hall effect sensor.
 */
void Init_hall(){

}
