/* DriverLib Includes */
#include "driverlib.h"

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "bluetooth.h"
#include "RTC.h"
#include "alarm.h"
#include "motor.h"
#include "sensors.h"

int buffer_index; // Current place in buffer

/*
 * Flag to indicate if new unprocessed data has been sent by phone to
 * prevent responding to same command twice.
 */
int data_used;

/*
 * Message to send out
 */
char print_all[70];

/*
 * Used to store last 3 bytes of received data
 */
uint8_t read_data[3];

volatile uint8_t received_data = 0;
volatile uint8_t buffer[BUFFER_SIZE];

/* UART Configuration Parameter. These are the configuration parameters to
 * make the eUSCI A UART module to operate with a 9600 baud rate. These
 * values were calculated using the online calculator that TI provides
 * at:
 *http://software-dl.ti.com/msp430/msp430_public_sw/mcu/msp430/MSP430BaudRateConverter/index.html
 *
 * Configured for a 9600 baud rate (specified by bluetooth module) and 3MHz
 * SMCLK speed (default speed)
 */
const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,           // SMCLK Clock Source
        19,                                       // BRDIV
        8,                                        // UCxBRF
        0,                                        // UCxBRS
        EUSCI_A_UART_NO_PARITY,                   // No Parity
        EUSCI_A_UART_LSB_FIRST,                   // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,                // One stop bit
        EUSCI_A_UART_MODE,                        // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

/*
 * Initialize settings for bluetooth module
 */
void Init_bluetooth(){
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    /* Selecting P3.2 and P3.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // Use LED on P1.0 to indicate when data is received. Switch off at start.
    MAP_GPIO_setAsOutputPin(GPIO_PORT_P1, GPIO_PIN0);
    MAP_GPIO_setOutputLowOnPin(GPIO_PORT_P1, GPIO_PIN0);

    /* Configuring UART Module */
    MAP_UART_initModule(EUSCI_A2_BASE, &uartConfig);

    /* Enable UART module */
    MAP_UART_enableModule(EUSCI_A2_BASE);

    /* Enabling interrupts */
    MAP_UART_enableInterrupt(EUSCI_A2_BASE, EUSCI_A_UART_RECEIVE_INTERRUPT);
    MAP_Interrupt_enableInterrupt(INT_EUSCIA2);
    //MAP_Interrupt_enableSleepOnIsrExit();
    MAP_Interrupt_enableMaster();

    // Start buffer index at beginning of array
    buffer_index=0;


    data_used = 1;
}

/* EUSCI A2 UART ISR - Echoes data back to PC host */
// Use UART2 because using P3.2 and P3.3 for RX and TX
void EUSCIA2_IRQHandler(void)
{

    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
        // Toggle LED to show data was received
        P1OUT ^= BIT0;

        received_data = MAP_UART_receiveData(EUSCI_A2_BASE);

        // Restart buffer if reached maximum
        if(buffer_index>BUFFER_SIZE-1) {
            buffer_index=0;
        }

        // Store data in buffer.
        // Not using a buffer causes problems with data being received too quickly.
        buffer[buffer_index++]= received_data;

        MAP_UART_transmitData(EUSCI_A2_BASE, received_data);  // send byte out UART2 port

        data_used = 0;
    }
}

/*
 * Check if a keyword was transmitted over bluetooth
 * @return 1 keyword detected and screen changed, 0 no screen change
 *
 * (need to change screen back to home if screen changed)
 */
int check_bluetooth(){

    // Check if data was sent
    if(buffer_index > 2 && data_used == 0){

        int i, j=2;

        // Clear print all
        for(i=0;i<58;i++){
           print_all[i] = 0;
        }

        // Check last 3 characters transmitted
        for(i=0; i<3; i++){
            read_data[j] = buffer[buffer_index-1-i];
            j--;
        }


        // Send status if received STA
        if(read_data[0] == 'S' && read_data[1] == 'T' && read_data[2] == 'A'){

            // Get time from RTC
            RTC_read();
            RTC_read();

            char print_date[27];
            sprintf(print_date,"\nDATE: %02x/%02x/%02x %02x:%02x:%02x\n", RTC_registers[5], RTC_registers[4], RTC_registers[6], RTC_registers[2], RTC_registers[1], RTC_registers[0]);

            strcat(print_all, print_date);

            if(get_armed()){
                char print_arm_disarm[] = "ALARM: ARMED\n";
                strcat(print_all, print_arm_disarm);
            }else{
                char print_arm_disarm[] = "ALARM: DISARMED\n";
                strcat(print_all, print_arm_disarm);
            }

            if(get_lock()){
                char print_lock_unlock[] = "LOCK: LOCKED\n\n";
                strcat(print_all, print_lock_unlock);
            }else{
                char print_lock_unlock[] = "LOCK: UNLOCKED\n\n";
                strcat(print_all, print_lock_unlock);
            }

            for(i=0;i<70;i++){
                MAP_UART_transmitData(EUSCI_A2_BASE, print_all[i]);
            }

            data_used = 1;
            return 0;
        }

        // Lock/unlock if received LCK
        if(read_data[0] == 'L' && read_data[1] == 'C' && read_data[2] == 'K'){
            int success = lock_unlock_door();

            if(success){
                if(get_lock()){
                    char print[] = "\nDOOR LOCKED\n\n";
                    strcat(print_all, print);
                }else{
                    char print[] = "\nDOOR UNLOCKED\n\n";
                    strcat(print_all, print);
                }

            }else{
                char print[] = "\nDOOR CANNOT BE LOCKED. CLOSE DOOR AND TRY AGAIN.\n\n";
                strcat(print_all, print);
            }

            for(i=0;i<70;i++){
                MAP_UART_transmitData(EUSCI_A2_BASE, print_all[i]);
            }

            data_used = 1;
            return 1;
        }

        // Arm/disarm if received ARM
        if(read_data[0] == 'A' && read_data[1] == 'R' && read_data[2] == 'M'){
            int success = arm_disarm_alarm();

            if(success){
                if(get_armed()){
                    char print[] = "\nALARM ARMED\n\n";
                    strcat(print_all, print);
                }else{
                    char print[] = "\nALARM DISARMED\n\n";
                    strcat(print_all, print);

                    // If disarming after a trigger
                    if(get_trigger_status() != NONE){
                        set_trigger_displayed(0);
                    }
                }
            }else{
                char print[] = "\nALARM CANNOT BE ARMED. CLEAR TRIGGERS AND TRY AGAIN.\n\n";
                strcat(print_all, print);
            }

            for(i=0;i<70;i++){
                MAP_UART_transmitData(EUSCI_A2_BASE, print_all[i]);
            }

            data_used = 1;
            return 1;
        }
    }

    return 0;
}


