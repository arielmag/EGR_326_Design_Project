#include <motor.h>
#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "sensors.h"
#include "timers.h"
#include "LCD.h"
#include "ST7735.h"
#define LOCK_SPEED 2 // Speed that the door locks/unlocks

int lock = 0;

/*
 * Get the locked/unlocked status of the door
 * @return 1 locked, 0 unlocked
 */
int get_lock(){
    return lock;
}

/*
 * Set the locked/unlocked status of the door
 * @param status 1 locked, 0 unlocked
 */
void set_lock(int status){
    lock = status;
}

/*
 * Lock or unlock the door by turning the motor and setting the lock flag. If the door is
 * open, it will not lock and a warning message will display.
 */
void lock_unlock_door(){
    if(get_lock()){ // If door is locked, unlock
        clearScreen();
        ST7735_DrawString(8, 8, "Unlocking",ST7735_Color565(180, 240, 250));
        full_step_reverse(LOCK_SPEED);
        set_lock(!get_lock()); // Set flag to unlocked

    }else{ // Otherwise, turn the other way to lock
        if(get_door_status() == CLOSED){ // Only lock if the door is closed
            clearScreen();
            ST7735_DrawString(8, 8, "Locking",ST7735_Color565(180, 240, 250));
            full_step(LOCK_SPEED);
            set_lock(!get_lock()); // Set flag to locked
        }else{
            clearScreen();
            ST7735_DrawString(0, 7, "Door is open. ",ST7735_Color565(180, 240, 250));
            ST7735_DrawString(0, 8, "Close door and try.",ST7735_Color565(180, 240, 250));
            cas_sysDelay(2);
                       // delay, and then go back to menu
        }
    }
}

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
