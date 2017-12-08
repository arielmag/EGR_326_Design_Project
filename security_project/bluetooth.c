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

int bufIdx, isSent;

char print_all[70];
uint8_t read_data[3];

volatile uint8_t receivedData = 0;

volatile uint8_t U2RXData = 0;
volatile uint8_t ESPbuffer[ESPbufSize];

const eUSCI_UART_Config uartConfig =
{
        EUSCI_A_UART_CLOCKSOURCE_SMCLK,       // SMCLK Clock Source at 12 MHz (divided down from high frequency crystal)
        19,                                   // BRDIV (clock prescaler) = INT(((12000000/9600)=1250)/16)=78.125)=78
        8,                                    // UCxBRF = INT([(1250/16)–INT(1250/16)] × 16))=2
        0,                                    // UCxBRS = 0x10 = 8 (second modulation stage, from table on page 721)
        EUSCI_A_UART_NO_PARITY,                  // No Parity
        EUSCI_A_UART_LSB_FIRST,                  // LSB First
        EUSCI_A_UART_ONE_STOP_BIT,               // One stop bit
        EUSCI_A_UART_MODE,                       // UART mode
        EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION  // Oversampling
};

void Init_bluetooth(){
    /* Selecting P1.2 and P1.3 in UART mode */
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P1,
             GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

    // Changed to P3.2 and 3.3
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P3,
            GPIO_PIN1 | GPIO_PIN2 | GPIO_PIN3, GPIO_PRIMARY_MODULE_FUNCTION);

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

    bufIdx=0;
    isSent = 1;

//    RTC_read();
//    RTC_write();
//    RTC_write();
//    RTC_read();
}

void set_sent(int x){
    isSent = x;
}

int get_sent(){
    return isSent;
}

/* EUSCI A2 UART ISR - Echoes data back to PC host */
void EUSCIA2_IRQHandler(void)
{

    uint32_t status = MAP_UART_getEnabledInterruptStatus(EUSCI_A2_BASE);

    MAP_UART_clearInterruptFlag(EUSCI_A2_BASE, status);

    if(status & EUSCI_A_UART_RECEIVE_INTERRUPT)
    {
        P1OUT ^= BIT0;

        U2RXData = MAP_UART_receiveData(EUSCI_A2_BASE);

        if(bufIdx>ESPbufSize-1) {
            bufIdx=0;  // implement circular buffer to prevent overflow
        }

        ESPbuffer[bufIdx++]= U2RXData; // put in circular buffer for program analysis

        MAP_UART_transmitData(EUSCI_A2_BASE, U2RXData);  // send byte out UART0 port

        isSent =0;


    }
}

/*
 * Check if a keyword was transmitted over bluetooth
 * @return 1 keyword detected and screen changed, 0 no screen change
 */
int check_bluetooth(){

    // Check if data was sent
    if(bufIdx > 2 && isSent == 0){

        int i, j=2;

        // Clear print all
        for(i=0;i<58;i++){
           print_all[i] = 0;
        }

        // Check last 3 characters transmitted
        for(i=0; i<3; i++){
            read_data[j] = ESPbuffer[bufIdx-1-i];
            j--;
        }


        // Send status if received STA
        if(read_data[0] == 'S' && read_data[1] == 'T' && read_data[2] == 'A'){

            // Get time from RTC
            RTC_read();
            RTC_read();

            // Setting to Sunday, November 11, 2011 at 11:11:11
            //unsigned char RTC_registers[15]={0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x11, 0};

            char print_date[27];
            sprintf(print_date,"\nDATE: %02x/%02x/%02x %02x:%02x:%02x\n", RTC_registers[5], RTC_registers[4], RTC_registers[6], RTC_registers[2], RTC_registers[1], RTC_registers[0]);

            strcat(print_all, print_date);

            //char print_arm_disarm[17];
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

            isSent = 1;
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

            isSent = 1;
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

            isSent = 1;
            return 1;
        }
    }



    return 0;
}


