/*
 * uart.c
 *
 *  Created on: May 7, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"

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

// output number to UART 
void uart_put_num(uint32_t value)
{
    int num_digits;
    char string[20];
    int str_pos = 0;
    //int digits = 0;
    int next_digit;
    int mult = 0;

    for (str_pos = 0; str_pos < 20; str_pos++) {
        string[str_pos] = 0;
    }

    if (0 == value) {
        string[0] = 0x30;
    }
    else {
        str_pos = 0;
        next_digit = value;
        num_digits = 0;
        while (next_digit > 0) {
            next_digit /= 10;
            num_digits++;
        }
        while (num_digits > 0) {
            next_digit = value;
            mult = 1;
            while (next_digit > 9) {
                next_digit /= 10;
                mult *= 10;
            }
            string[str_pos] = 0x30 + next_digit;
            value -= next_digit * mult;
            str_pos++;
            num_digits--;
        }
    }

    uart_put_str(string);
}

