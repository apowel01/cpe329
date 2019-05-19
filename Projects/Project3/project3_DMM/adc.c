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
#define NUM_BUFFERS 2
#define BUFFER_0 0
#define BUFFER_1 1

typedef struct {
    uint16_t samples[SAMPLES_PER_SECOND];
    uint8_t buffer_ready_to_read;
    uint16_t max_volts;
    uint16_t min_volts;
    uint16_t frequency;
    uint16_t dc_offset;
    uint16_t peak_peak;
} sample_buffer_t;

// local file variables
static sample_buffer_t sample_buffers[NUM_BUFFERS]; // create sample array
static uint16_t current_sample = 0;
static uint8_t current_buffer = 0;

uint32_t adc_get_samples_per_second(void)
{
    return SAMPLES_PER_SECOND;
}

// get value from the ADC
uint16_t adc_get_value(void)
{
    return sample_buffers[0].samples[0];
}

void adc_sample_start(void)
{
    ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion from A to D
}

// interrupt handler for the ADC
void ADC14_IRQHandler(void)
{
    uint16_t value;

    // read conversion value
    value = ADC14->MEM[2]; // reading auto clears interrupt flag
    if (0 == sample_buffers[current_buffer].buffer_ready_to_read) {
        sample_buffers[current_buffer].samples[current_sample] = value;
        current_sample++;
        if (current_sample == SAMPLES_PER_SECOND) {
            current_sample = 0;
            sample_buffers[current_buffer].buffer_ready_to_read = 1;
            current_buffer++;
            if (NUM_BUFFERS == current_buffer) {
                current_buffer = 0;
            }
        }
    }
    else {
        // dump the value, the buffers are both full
    }
}

static void adc_analyze_buffer(sample_buffer_t *p_buffer)
{
    int i;
    int currently_rising = 0;

    // ac - peak to peak voltage, rms voltage, dc offset, frequency
    // find max and min volts
    p_buffer->max_volts = p_buffer->samples[0];
    p_buffer->min_volts = p_buffer->samples[0];
    for (i = 1; i < SAMPLES_PER_SECOND; i++) {
        if (p_buffer->max_volts < p_buffer->samples[i]) {
            p_buffer->max_volts = p_buffer->samples[i];
        }
        if (p_buffer->min_volts > p_buffer->samples[i]) {
            p_buffer->min_volts = p_buffer->samples[i];
        }
    }
    // find peak to peak voltage
    p_buffer->peak_peak = p_buffer->max_volts - p_buffer->min_volts;

    // find DC offset
    p_buffer->dc_offset = p_buffer->min_volts + (p_buffer->peak_peak / 2);

    // find frequency
    p_buffer->frequency = 0;
    for (i = 0; i < SAMPLES_PER_SECOND; i++) {
        if (currently_rising == 0) {
            if (p_buffer->samples[i] > p_buffer->dc_offset) {
                currently_rising = 1;
                p_buffer->frequency++;
            }
        }
        else {
            if (p_buffer->samples[i] < p_buffer->dc_offset) {
                currently_rising = 0;
            }
        }
    }
}

// determine input waveform frequency
void adc_get_values(uint32_t *p_frequency, uint32_t *p_dc_offset, uint32_t *p_vpp)
{
    uint32_t my_buffer = 0;
    sample_buffer_t *p_buffer = &sample_buffers[my_buffer];

    // look for a full buffer
    while (0 == p_buffer->buffer_ready_to_read) {
        my_buffer++;
        if (NUM_BUFFERS == my_buffer) {
            my_buffer = 0;
        }
        p_buffer = &sample_buffers[my_buffer];
    }
    adc_analyze_buffer(p_buffer);
    *p_frequency = p_buffer->frequency;
    *p_dc_offset = (p_buffer->dc_offset*100)/4990;
    *p_vpp = (p_buffer->peak_peak*100)/4990;
    p_buffer->buffer_ready_to_read = 0;
}

// initialize the adc
void adc_init(void)
{
    // configure ADC14
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; // disable ADC for configuration
    ADC14->CTL0 = ADC14_CTL0_SHP // sample pulse mode, use internal sample timer
                | ADC14_CTL0_SSEL_4 // select SMCLK
                | ADC14_CTL0_SHT0_2 // select 4 clocks for mem[2]
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
