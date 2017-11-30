#include "driverlib.h"
#include <stdio.h>
#include <stdint.h>
#include "RTC.h"
int month_flag=0;
int day_flag=0;
int year_flag=0;
int DOW_flag=0;
int hour_flag=0;
int min_flag=0;
int sec_flag=0;
short temp=0;

const eUSCI_I2C_MasterConfig i2cConfig = {
EUSCI_B_I2C_CLOCKSOURCE_SMCLK,          // SMCLK Clock Source
        3000000,                                // SMCLK = 3MHz
        EUSCI_B_I2C_SET_DATA_RATE_100KBPS,      // Desired I2C Clock of 400khz
        0,                                      // No byte counter threshold
        EUSCI_B_I2C_NO_AUTO_STOP                // No Autostop
        };

/*
 * Initialize I2C communication with RTC
 * From EGR 326 lecture slide.
 *
 * P6.4 UCB1SDA
 * P6.5 UCB1SCL
 */
void I2C_init()
{
    // For example, select Port 6 for I2C
    // Set Pin 4, 5 to input Primary Module Function,
    // P6.4 is UCB1SDA, P6.5 is UCB1SCL
    MAP_GPIO_setAsPeripheralModuleFunctionInputPin(GPIO_PORT_P6,
    GPIO_PIN4 + GPIO_PIN5, GPIO_PRIMARY_MODULE_FUNCTION);
    // Initializing I2C Master (see description in Driver Lib for
    // proper configuration options)
    MAP_I2C_initMaster(EUSCI_B1_BASE, &i2cConfig);
    // Specify slave address
    MAP_I2C_setSlaveAddress(EUSCI_B1_BASE, RTC_ADDRESS);
    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Enable I2C Module to start operations
    MAP_I2C_enableModule(EUSCI_B1_BASE);
}

/*
 * This function sets up ports for use of the RTC module.
 * P6.4 SDA
 * P6.5 SCL
 */
void Init_RTC(){
    I2C_init();
}

void Port4_Initb() // Initiate port pin 1.0 as input with internal pull up resistor
{
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

void RTC_write() //EGR 326 lecture slide
{
    // Setting to Sunday, November 11, 2011 at 11:11:11 so I don't have to enter it every time for testing
    //unsigned char date[15]={0x11, 0x11, 0x11, 0x01, 0x11, 0x11, 0x11, 0}; 
    /* Set Master in transmit mode */
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // set pointer to beginning of RTC registers
    MAP_I2C_masterSendMultiByteStart(EUSCI_B1_BASE,0);
    // and write to seconds register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE,date[0]);
    // write to minutes register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, date[1]);
    // write to hours register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, date[2]);
    // write to day register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, date[3]);
    // write to date register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, date[4]);
    // write to months register
    MAP_I2C_masterSendMultiByteNext(EUSCI_B1_BASE, date[5]);
    // write to year register and send stop
    MAP_I2C_masterSendMultiByteFinish(EUSCI_B1_BASE, date[6]);
}

/*
 * Read temperature from RTC module.
 * @return Temperature in Fahrenheit
 */
float RTC_read_temperature()
{
    uint8_t tUBYTE = 0;
    uint8_t tLBYTE = 0;

    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // set pointer to beginning of RTC registers
    MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE,RTC_TEMPERATURE_ADDRESS);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));

    // read from RTC registers (pointer auto increments after each read)
    tUBYTE = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    tLBYTE = MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);

    return convertTemperature(tUBYTE,tLBYTE);
}

/*
 * Convert temperature form Celsius to Fahrenheit.
 * Code from //https://github.com/adafruit/RTClib/issues/72
 * @param tUBYTE upper byte of temperature in C
 * @param tLBYTE lower byte of temperature in C
 * @return Temperature in Fahrenheit
 */
float convertTemperature(uint8_t tUBYTE, uint8_t tLBYTE){
    float fTemperatureCelsius;
    float fTemperatureFarenheit;

    if(tUBYTE & 0b10000000) //check if negative temperature
    {
       // perform conversion for 2s compliment
       tUBYTE  ^= 0b11111111;
       tUBYTE  += 0x1;
       fTemperatureCelsius = tUBYTE + ((tLBYTE >> 6) * 0.25);
       fTemperatureCelsius = fTemperatureCelsius * -1;
    }
    else
    {
        fTemperatureCelsius = tUBYTE + ((tLBYTE >> 6) * 0.25);
    }

    fTemperatureFarenheit = fTemperatureCelsius*1.8 + 32;

    return (fTemperatureFarenheit);
}

void RTC_read() //EGR 326 lecture slide
{
    // Set Master in transmit mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_TRANSMIT_MODE);
    // Wait for bus release, ready to write
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // set pointer to beginning of RTC registers
    MAP_I2C_masterSendSingleByte(EUSCI_B1_BASE,0);
    // Wait for bus release
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // Set Master in receive mode
    MAP_I2C_setMode(EUSCI_B1_BASE, EUSCI_B_I2C_RECEIVE_MODE);
    // Wait for bus release, ready to receive
    while (MAP_I2C_isBusBusy(EUSCI_B1_BASE));
    // read from RTC registers (pointer auto increments after each read)
    RTC_registers[0]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[1]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[2]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[3]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[4]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[5]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
    RTC_registers[6]=MAP_I2C_masterReceiveSingleByte(EUSCI_B1_BASE);
}
