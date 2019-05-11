/*
 * uart.c
 *
 *  Created on: May 7, 2019
 *      Author: Amberley
 */
#include "msp.h"

#define RXBUF_CHAR_MASK 0xFF
// ascii values used
#define ASCII_ZERO 0x30
#define ASCII_NINE 0x39
#define ASCII_CR   0x0D
#define ASCII_LF   0x0A
// flag values
#define READY_FLAG_READY 1
#define READY_FLAG_NOT_READY 0
// baud rate values
#define BRW_FOR_3_MHZ 0x01
#define BRF_FOR_3_MHZ 10
// Initialize the UART
void uart_init(void)
{
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // put serial device into reset state
    // Set 8-bit no parity
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_UCSSEL_2 // SMCLK
            | EUSCI_A_CTLW0_MODE_0 // UART mode
            | EUSCI_A_CTLW0_SWRST; // keep in reset
    EUSCI_A0->BRW = BRW_FOR_3_MHZ; // from baud rate calculation
    EUSCI_A0->MCTLW = (BRF_FOR_3_MHZ << EUSCI_A_MCTLW_BRF_OFS) //causing it to shift 4 bits as
                        |EUSCI_A_MCTLW_OS16;      //offest from mctlw register
    P1->SEL0 |= (BIT2|BIT3);   // Configure EUSCI_A0
    P1->SEL1 &= ~(BIT2|BIT3); //TX and Rx pins
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Activate serial device

    EUSCI_A0->IE |= EUSCI_A_IE_RXIE; // enable interrupts
    NVIC->ISER[0] = 1 << ((EUSCIA0_IRQn) & 31);  // NVIC interrupt
    __enable_irq(); // enable global interrupts
}

// output  single character - note, we do not do this in interrupt,
// we just keep sending the bytes.
void uart_put_char(char chr)
{
    // wait for the Tx buffer to be ready
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)) {
    }
    // buffer is free, write out data
    EUSCI_A0->TXBUF = chr;
}

// output a string
void uart_put_str(char * p_str)
{
    // early out for bad pointer
    if (0 == p_str) {
        return;
    }

    while (0 != *p_str) {
        uart_put_char(*p_str);
        p_str++;
    }
}

static uint16_t read_value = 0;
static uint8_t ready_to_read = READY_FLAG_NOT_READY;
uint16_t uart_get_value(void)
{
    uint16_t tvalue;
    // wait for the RX buffer to be ready
    while(ready_to_read != READY_FLAG_READY) {
    }
    tvalue = read_value;
    ready_to_read = READY_FLAG_NOT_READY;
    return tvalue; // cast receive buffer as char
}

// Basic receive IRQ handler
void EUSCIA0_IRQHandler(void)
{
    static uint16_t isr_value = 0;
    char last_char;
    EUSCI_A0->IFG &= ~EUSCI_A_IFG_RXIFG;  // Clear the RX interrupt flag
    // read the character
    last_char = (char)(EUSCI_A0->RXBUF & RXBUF_CHAR_MASK);
    // echo it back
    // wait for the Tx buffer to be ready
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)) {
    }
    // buffer is free, write out data
    EUSCI_A0->TXBUF = last_char;
    // parse character
    if ((last_char >= ASCII_ZERO) && (last_char <= ASCII_NINE)) {
        isr_value = (isr_value * 10) + (last_char - ASCII_ZERO);
    }
    else if (last_char == ASCII_CR) {
        // wait for the Tx buffer to be ready
        while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)) {
        }
        // buffer is free, write out data
        EUSCI_A0->TXBUF = ASCII_LF;
        if (isr_value != 0) {
            read_value = isr_value;
        }
        isr_value = 0;
        ready_to_read = READY_FLAG_READY;
    }
}

void uart_main(void)
{
    uint16_t inValue;
    delay_set_dco(FREQ_3_0_MHz);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    dac_init();
    uart_init();

    while(1) {
        //while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        //EUSCI_A0->TXBUF = 'A';
        inValue = uart_get_value();
        if (inValue < 4096) {
            dac_send(inValue);
        }
        else {
            uart_put_str("ERROR: number out of range");
        }
    }
}
