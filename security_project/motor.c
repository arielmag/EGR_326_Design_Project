#include <motor.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>

/*
 * Initialize pins for the motor
 * P5.2, P3.5, P3.6, P3.7 Set as output
 */
void Init_motor(){

    P5-> DIR |= BIT2;
    P3-> DIR |= BIT5 | BIT6 |BIT7;

}

/*
 * Turn the motor a half rotation clockwise using the full step technique
 *
 * @param speed Time to delay in ms
 */
void full_step(uint16_t speed)
{
    //Blue pink yellow orange
    //P3.5 P5.2 P3.7   P3.6
    //IN 1  IN2  IN3    IN4

   // int full_rotation =512; //64*32/4
    int half_rotation =256; //64*32/4
    int j;

    for (j=0;j<half_rotation;j++)
    {
            P3-> OUT |= BIT5;
            P5-> OUT |= BIT2;
            SysTick_delay(speed);   //1 1 0 0

            P3-> OUT &= ~BIT5;
            P3-> OUT |= BIT7;
            SysTick_delay(speed);   //0 1 1 0

            P5-> OUT &= ~BIT2;
            P3-> OUT |= BIT6;
            SysTick_delay(speed);   //0 0 1 1

            P3-> OUT &= ~BIT7;
            P3-> OUT |= BIT5;
            SysTick_delay(speed);   //1 0 0 1
            P3-> OUT &= ~BIT6;
       }
    clear_motor();
}

/*
 * Turn the motor a half rotation counter-clockwise using the full step technique
 *
 * @param speed Time to delay in ms
 */
void full_step_reverse(uint16_t speed)
{
    //Blue pink yellow orange
     //P3.5 P5.2 P3.7   P3.6
     //IN1  IN2  IN3    IN4


   // int full_rotation =512; //64*32/4
    int half_rotation =256; //64*32/4
     int j;
     P3->OUT &=~BIT5 | BIT7 | BIT6; //TURN ALL off
     P5->OUT &=~BIT2;

     for (j=0;j<half_rotation;j++)
     {

       P3-> OUT |= BIT5;
       P3-> OUT |= BIT6;
       SysTick_delay(speed);   //1 0 0 1

       P3-> OUT &= ~BIT5;
       P3-> OUT |= BIT7;
       SysTick_delay(speed);   //0 0 1 1

       P3-> OUT &= ~BIT6;
       P5-> OUT |= BIT2;
       SysTick_delay(speed);   //0 1 1 0

       P3-> OUT &= ~BIT7;
       P3-> OUT |= BIT5;
       SysTick_delay(speed);   //1  1 0 0
       P5-> OUT &= ~BIT2;
        }
     clear_motor();
}

/*
 * Turn all motor pins off
 */
void clear_motor()
{
    P3->OUT &=~BIT5 | BIT7 | BIT6; //TURN ALL off
    P5->OUT &=~BIT2;
}
