/*
 * adc.c
 *
 *  Created on: May 10, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"
#include "uart.h"
#include "adc.h"

// flag values
#define READ_FLAG_READY 1
#define READ_FLAG_NOT_READY 0


static uint16_t analogValue = 0;
static uint8_t ready_to_read = READ_FLAG_NOT_READY;
uint16_t adc_get_value(void)
{
    uint16_t tvalue;
    // wait for the RX buffer to be ready
    while(ready_to_read != READ_FLAG_READY) {
    }
    tvalue = analogValue;
    ready_to_read = READ_FLAG_NOT_READY;
    return tvalue; // cast receive buffer as char
}

// interrupt handler for the ADC
void ADC14_IRQHandler(void) {
    analogValue = ADC14->MEM[2]; // read conversion value; reading auto clears interrupt flag
    ready_to_read = READ_FLAG_READY;
}

void adc_init(void) {
    // configure ADC14
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; // disable ADC for configuration
    ADC14->CTL0 = ADC14_CTL0_SHP // sample pulse mode, use internal sample timer
                | ADC14_CTL0_SSEL_4 // select SMCLK
                | ADC14_CTL0_SHT0_0 // select 4 clocks for mem[2]
                | ADC14_CTL0_ON; // turn on ADC14
    ADC14->CTL1 = (2 << ADC14_CTL1_CSTARTADD_OFS) // start conversions using mem[2]
                                                  // shift 2 into bits 20-16
                  | ADC14_CTL1_RES_3; // use 14-bit conversion
    ADC14->MCTL[2] = ADC14_MCTLN_INCH_14; // read from channel 14(A14)
    ADC14->CTL0 |= ADC14_CTL0_ENC; // enable ADC
    // enable interrupts
    ADC14->IER0 |= ADC14_IER0_IE2; // enable interrupts on MEM2
    NVIC->ISER[0] = 1 << (ADC14_IRQn & 31); //enable ADC14 ISR in NVIC
    __enable_irq(); // enable global interrupts
    // select A14 input function on P6.1
    P6->SEL0 |= BIT1;
    P6->SEL1 |= BIT1;
}

void adc_main(void)
{

}
