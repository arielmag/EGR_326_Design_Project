/*
 * buzzer.c
 *
 *  Created on: Nov 23, 2017
 *      Author: Dongyao
 */


/* DriverLib Includes */
#include "driverlib.h"
#include <stdio.h>
#include<string.h>

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "LCD.h"
#include "RTC.h"
#include "ST7735.h"
#include "timers.h"
#include <string.h>
#include "keypad.h"
#include "alarm.h"
#include "buzzer.h"

void sounder_off();
/*
*       timeHigh = 1/(2 * toneFrequency) = period / 2
*
* where the different tones are described as in the table:
*
* note         frequency       period  PW (timeHigh)
* c            261 Hz          3830    1915
* d            294 Hz          3400    1700
* e            329 Hz          3038    1519
* f            349 Hz          2864    1432
* g            392 Hz          2550    1275
* a            440 Hz          2272    1136
* b            493 Hz          2028    1014
* C            523 Hz          1912    956
*
*              550 Hz          1818
*              600 Hz          1667
*              700 Hz          1429
*              800 Hz          1250
*              900 Hz          1111
*/
//              c      d     e     f     g     a    b    C
int tone[] = {3830, 3400, 3038, 2864, 2550, 2272, 2028, 1912, 1818, 1667, 1429, 1250, 1111};
int low = 3830;
int high = 3038;

void sounder_off()
{
    pwm_buzzer(0);
}

void pwm_buzzer(int period)
{
    P2->DIR |=BIT6;
    P2->SEL0 |= BIT6;
    P2 -> SEL1 &= ~(BIT6);

    int duty = period/2;
    TIMER_A0 ->CCR[0] = period; //PWM Period
    TIMER_A0 ->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; //CCR1 Reset\set
    TIMER_A0 ->CCR[3] = duty; //duty cycle
    TIMER_A0->CTL = TIMER_A_CTL_SSEL__SMCLK | TIMER_A_CTL_MC__UP
            |TIMER_A_CTL_CLR;

}

void tone1()
{
        pwm_buzzer(tone[5]);
        SysTick_delay(333);
        SysTick_delay(333);
        pwm_buzzer(tone[5]);
        SysTick_delay(333);
        SysTick_delay(333);
        pwm_buzzer(tone[5]);
        SysTick_delay(333);
        SysTick_delay(333);
        pwm_buzzer(tone[3]);
        SysTick_delay(250);
        SysTick_delay(250);
        pwm_buzzer(tone[0]);
        SysTick_delay(250);
        SysTick_delay(250);
        pwm_buzzer(tone[5]);
        SysTick_delay(250);
        SysTick_delay(250);
        pwm_buzzer(tone[3]);
        SysTick_delay(250);
        SysTick_delay(250);
        pwm_buzzer(tone[0]);
        SysTick_delay(250);
        SysTick_delay(250);
        pwm_buzzer(tone[5]);
        SysTick_delay(250);
        SysTick_delay(250);

        sounder_off();
        cas_sysDelay(1);
}

