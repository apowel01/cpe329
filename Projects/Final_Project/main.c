//******************************************************************************
// CPE 329 - Assignment 9
// The MSP432 acts as the master and the EEPROM is a slave.
// The EEPROM uses 3 external connections A2 A1 A0 to set the lower 3 bits of
// its bus address. This creates a bus address of "1 0 1 0 A2 A1 A0". The code
// below assumes those three connections are all connected to VSS (Ground) and
// are logic 0. This gives the EEPROM a bus address of 0x50.
//
//
//                               /|\   /|\
//              MSP432P401      10k    10k     24LC256 EEPROM
//                master         |     |           Slave
//            -----------------  |     |     -----------------
//           |    P1.6/UCB0SDA|<-|----| ->  |SDA (5)         |
//           |                   |    |     |                |
//           |                   |    |     |                |
//           |    P1.7/UCB0SCL|<-|------>  |SCL (6)          |
//           |                   |         |                 |
//
// Paul Hummel
// Cal Poly
// May 2017 (created)
// Built with CCSv8.1
//******************************************************************************

/*
 * main.c
 *
 *  Created on: May 31, 2019
 *      Author: Andy Hunt
 */
#include "msp.h"
#include "timer_a.h"
#include "delay.h"
#include "LCD.h"
#include "pmod_color.h"
#include "speech.h"
#include "buttons.h"

void main(void) {
    pmod_colors_t colors;
    pmod_result_t results;
    uint8_t wait;
    char * color_name;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz); // set DCO to 3MHz
//    timer_a_init(); // why are we doing this?
    // initialize the color sensor
    pmod_color_init();
    // initialize the speakjet text to speech module
    speech_init();
    // initialize the LCD
    lcd_init();
    // initialize buttons
    buttons_init();
    __enable_irq();


    P2->DIR |= BIT2 | BIT1 | BIT0;             //Configure Pins For output for LCD
    P2->OUT &= ~(BIT2 | BIT1 | BIT0);

    while(1) {
                // delay_sec(5);   // cloud replace this with a button push
        wait = buttons_get_push();
        if (wait == 0)
        {
            // read colors from the pmod sensor
            pmod_color_read(&colors);
            // convert the colors to text
            color_name = pmod_color_to_name(&colors, &results);
            // send it to the speech module
            speech_say(&results);
            // delay so not too spammed
            delay_sec(2);
        }
        else
        {
            delay_ms(300);
            Clear_LCD();
        }
    }
}

#if 0
void WritePmod(uint16_t MemAddress, uint8_t MemByte) {


    uint8_t HiAddress;
    uint8_t LoAddress;

    HiAddress = MemAddress >> 8;
    LoAddress = MemAddress & 0xFF;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;    // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

    while (!TransmitFlag);                  // Wait for Pmod address to transmit
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = HiAddress;          // Send the high byte of the memory address

    while (!TransmitFlag);                   // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = LoAddress;           // Send the low byte of the memory address

    while (!TransmitFlag);                   // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> TXBUF = MemByte;             // Send the byte to store in Pmod

    while (!TransmitFlag);                   // Wait for the transmit to complete
    TransmitFlag = 0;
    EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP; // I2C stop condition
}
#endif

#if 0
uint16_t ReadPmod(uint16_t RGBAddress) {
    uint8_t ReceiveByte;
    uint8_t HiAddress;
    uint8_t LoAddress;

    HiAddress = RGBAddress >> 8;
    LoAddress = RGBAddress & 0xFF;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR; // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

    while (!TransmitFlag); // Wait for Pmod address to transmit
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = HiAddress; // Send the high byte of the memory address

    while (!TransmitFlag); // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0 -> TXBUF = LoAddress; // Send the low byte of the memory address

    while (!TransmitFlag); // Wait for the transmit to complete
    TransmitFlag = 0;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR; // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    while (!TransmitFlag); // Wait to receive a byte
    TransmitFlag = 0;

    ReceiveByte = EUSCI_B0->RXBUF; // Read byte from the buffer

    return ReceiveByte;
}
#endif


