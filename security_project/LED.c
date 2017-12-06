/*
 * LED.c
 *
 *  Created on: Dec 5, 2017
 *      Author: Donald
 */

/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "timers.h"

void Init_LED() //6.6 red, 6.7 green
{
    P6->DIR |= BIT6; //GPIO 6.6, 6.7 as output
    P6->DIR |= BIT7;
    P6->OUT &= ~BIT6; //GPIO 6.6, 6.7 initially low output
    P6->OUT &= ~BIT7;

}

void toggle_red() //P6.6
{
    P6->OUT ^= BIT6;

}

void on_green()
{
    P6->OUT |= BIT7;
}

void on_red()
{
    P6->OUT |= BIT6; //GPIO
}
