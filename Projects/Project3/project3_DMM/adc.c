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
// defines
#define SAMPLES_PER_SECOND 16000 // 16 samples / period at 1kHz

// local file variables
static uint16_t samples[SAMPLES_PER_SECOND]; // create sample array
static uint16_t current_sample = 0;

uint32_t adc_get_samples_per_second(void)
{
    return SAMPLES_PER_SECOND;
}

// get value from the ADC
uint16_t adc_get_value(void)
{
    return samples[0];
}

void adc_sample_start(void)
{
    ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion from A to D
}

// interrupt handler for the ADC
void ADC14_IRQHandler(void)
{
    // read conversion value
    samples[current_sample] = ADC14->MEM[2]; // reading auto clears interrupt flag
    current_sample++;
    if (current_sample == SAMPLES_PER_SECOND) {
        current_sample = 0;
    }
}

// determine input waveform frequency
uint32_t adc_get_frequency(void)
{
    int i;
    uint32_t num_rising_edges = 0;
    int currently_rising = 0;
    // count rising edges
    for (i = 1; i < SAMPLES_PER_SECOND; i++) {
        if (currently_rising == 0) {
            if (samples[i-1] < samples[i]) {
                currently_rising = 1;
                num_rising_edges++;
            }
        }
        else {
            if (samples[i-1] > samples[i]) {
                currently_rising = 0;
            }
        }
    }
    return num_rising_edges;
}

// initialize the adc
void adc_init(void)
{
    // configure ADC14
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; // disable ADC for configuration
    ADC14->CTL0 = ADC14_CTL0_SHP // sample pulse mode, use internal sample timer
                | ADC14_CTL0_SSEL_4 // select SMCLK
                | ADC14_CTL0_SHT0_0 // select 4 clocks for mem[2]
                | ADC14_CTL0_DIV__2 // divide SMCLK by 2 to get 24 MHz
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
