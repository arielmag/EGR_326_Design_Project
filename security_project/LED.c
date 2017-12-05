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

void Init_LED()
{
    P6->DIR |= BIT6;
    P6->DIR |= BIT7;

}

void toggle_LED()
{
    P6->OUT ^= BIT6;
    P6->OUT ^= BIT7;
    cas_sysDelay(1);
}
