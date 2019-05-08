/*
 * uart.c
 *
 *  Created on: May 7, 2019
 *      Author: Amberley
 */
#include "msp.h"

// Initialize the UART
void uart_init(void)
{
    EUSCI_A0 -> |= EUSCI_A_CTLW0_SWRST; // put serial device into reset state
    EUSCI_A0 -> CTLW0 = EUSCI_A_CTLW0_PEN //Enable parity - odd
                      | EUSCI_A_CTLW0_UCSSEL_2 // SMCLK
                      | EUSCI_A_CTLW0_MODE_0 // UART mode
                      | EUSCI_A_CTLW0_SWRST; // keep in reset
    EUSCI_A0 -> BRW = 0x01; // from baud rate calculation
    EUSCI_A0 -> MCTLW = (10 << EUSCI_A_MCTL_BRF_) //causing it to shift 4 bits as
                        |USSCI_A_MCTLW_OS16;      //offest from mctlw register
    P1 -> SEL0 |= (BIT2|BIT3);   // Configure EUSCI_A0
    P1 -> SEL0 &= ~(BIT2|BIT3); //TX and Rx pins
    EUSCI_A0 -> CLTW0 &= ~EUSCI_A_CTLW0_SWRST; // Activate serial device
}

// output  single character - note, we do not do this in interrupt,
// we just keep sending the bytes.
void uart_put_char(char chr)
{
    // wait for the Tx buffer to be ready
    while(!(IFG2 & UCA0TXIFG)) {
    }

    // buffer is free, write out data
    UCA0TXBUF = chr;

    return 0;
}

// output a string
void uart_put_str(char * p_str)
{
    // early out for bad pointer
    if (NULL == p_str) {
        return;
    }

    while (0 != *p_str) {
        uart_put_char(*p_str);

        p_str++;
    }
}

