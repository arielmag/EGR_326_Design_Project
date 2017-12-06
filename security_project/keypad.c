#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "keypad.h"
#include "timers.h"
#include "LCD.h"
#include "alarm.h"
#include "sensors.h"

int user_timeout;

void set_timeout(int value){
    user_timeout = value;
}

int get_timeout(){
    return user_timeout;
}


/*
 * This function sets up ports for use of the keypad.
 * P4.0, 4.1, 4.2, 4.3 Inputs
 */
void Init_keypad(){
    int i;
    for (i=0;i<4;i++)
    {
        P4SEL0 &= ~BIT(i);
        P4SEL1 &= ~BIT(i); // 1) configure P4.0 4.1 4.2 4.3 as GPIO
        P4DIR &= ~BIT(i); // 2) make P4.0 4.1 4.2 4.3 inputs
        P4REN |= BIT(i); // 3) enable pull resistors on P4.0 4.1 4.2 4.3
        P4OUT |= BIT(i); // P4.0 4.1 4.2 4.3 are pull-up
    }
}

char keypad_getkey()
{
    do{
                     // assumes port 4 bits 0-3 are connected to rows
       int row, col; // bits 4,5,6 are connected to columns

           const char column_select[] = {0x10, 0x20, 0x40}; // one column is active
           // Activates one column at a time, read the input to see which column

           for (col = 0; col < 3; col++) {
           P4->DIR &= ~0xF0; // disable all columns, aka set them as input
           P4->DIR |= column_select[col]; // enable one column at a time
           P4->OUT &= ~column_select[col]; // drive the active output column to low
           __delay_cycles(10); // wait for signal to settle
           row = P4->IN & 0x0F; // read all rows

           if(row != 0x0F){ // if one input is pressed, go into debounce sequence
               while(!debounce(row)); // loop until read 4 new consecutive inputs (0)
               P4->OUT |= column_select[col]; // drive active column high
               break;
           }
           P4->OUT |= column_select[col]; // drve the active column high
           }

           P4->OUT |= 0xF0; // drive all columns high before disable
           P4->DIR &= ~0xF0; // disable all columns

           if (row == 0x0E) return  (48+col + 1); // key in row 0  +48 for converting int to ACII char
           if (row == 0x0D) return (48+3 + col + 1); // key in row 1
           if (row == 0x0B) return (48+6 + col + 1); // key in row 2
           if (row == 0x07)
           {
               if(col==0)                return '*';

               if(col==1)                return '0';

               if(col==2)                return '#';
           }

           if(user_timeout == 1){ // for idle state detection
               // idle state, go back to home screen
               go_home();
           }

           // Check for trigger if not already in the process of displaying one
           if(!get_trigger_displayed())
               display_trigger(get_trigger_status());
           if(check_bluetooth())
               go_home();
       }while(1);

}

int debounce(int row)
{
    static uint16_t State = 0;  // Current debounce status
    int select = 4; // Initialize as unused value for debugging

    if (row == 0x0E) select = 0;
    if (row == 0x0D) select = 1;
    if (row == 0x0B) select = 2;
    if (row == 0x07) select = 3;

    State = (State << 1) | (P4IN & BIT(select))>>select | 0xf000;    // Read switch
    return (State == 0xffff); // Return true if button released for 8 consecutive calls
}

char check_pressed() // no debounce
{

        // assumes port 4 bits 0-3 are connected to rows
    int row, col; // bits 4,5,6 are connected to columns

    const char column_select[] = {0x10, 0x20, 0x40}; // one column is active
    // Activates one column at a time, read the input to see which column

    for (col = 0; col < 3; col++) {
        P4->DIR &= ~0xF0; // disable all columns, aka set them as input
        P4->DIR |= column_select[col]; // enable one column at a time
        P4->OUT &= ~column_select[col]; // drive the active output column to low
        __delay_cycles(10); // wait for signal to settle
        row = P4->IN & 0x0F; // read all rows

        if(row != 0x0F){ // if one input is pressed, go into debounce sequence
            while(!debounce(row)); // loop until read 4 new consecutive inputs (0)
            P4->OUT |= column_select[col]; // drive active column high
            break;
        }
        P4->OUT |= column_select[col]; // drve the active column high
    }

    P4->OUT |= 0xF0; // drive all columns high before disable
    P4->DIR &= ~0xF0; // disable all columns

    if (row == 0x0E) return  (48+col + 1); // key in row 0  +48 for converting int to ACII char
    if (row == 0x0D) return (48+3 + col + 1); // key in row 1
    if (row == 0x0B) return (48+6 + col + 1); // key in row 2
    if (row == 0x07)
    {
        if(col==0)                return '*';

        if(col==1)                return '0';

        if(col==2)                return '#';
    }

    // Check for trigger if not already in the process of displaying one
    if(!get_trigger_displayed())
        display_trigger(get_trigger_status());

    if(check_bluetooth())
        go_home();

    return 0; // return 0 if nothing pressed
}
