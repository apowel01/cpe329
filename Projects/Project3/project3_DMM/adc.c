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
#include "math.h"

// defines
#define SAMPLES_PER_SECOND 16000 // 16 samples / period at 1kHz
#define NUM_BUFFERS 2 // use two buffers: fill one while analyzing other
#define CALIBRATION_CONSTANT 4990 // ADC calibration factor

// buffer type definition
typedef struct {
    uint16_t samples[SAMPLES_PER_SECOND];
    uint8_t buffer_ready_to_read;
    // ac values
    uint16_t max_volts;
    uint16_t min_volts;
    uint16_t frequency;
    uint16_t dc_offset;
    uint16_t peak_peak;
    uint32_t rms_volts;
    // dc values
    uint32_t dc_volts;
} sample_buffer_t;

// local file variables
static sample_buffer_t sample_buffers[NUM_BUFFERS]; // create sample array
static uint16_t current_sample = 0;
static uint8_t current_buffer = 0;

// returns amount of samples per second
uint32_t adc_get_samples_per_second(void)
{
    return SAMPLES_PER_SECOND;
}

// get value from the ADC
uint16_t adc_get_value(void)
{
    return sample_buffers[0].samples[0];
}

// start new sample with ADC
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
    // only fill sample buffer if we are not analyzing it
    if (0 == sample_buffers[current_buffer].buffer_ready_to_read) {
        sample_buffers[current_buffer].samples[current_sample] = value;
        current_sample++;
        if (current_sample == SAMPLES_PER_SECOND) { // buffer full
            current_sample = 0;
            sample_buffers[current_buffer].buffer_ready_to_read = 1;
            current_buffer++; // go to next buffer
            if (NUM_BUFFERS == current_buffer) { // reset to buffer 0
                current_buffer = 0;
            }
        }
    }
    else {
        // dump the value, the buffers are both full
    }
}

// analyze buffer for AC values
static void adc_analyze_buffer_ac(sample_buffer_t *p_buffer)
{
    int i; // for running through sample buffer index
    int currently_rising = 0; // flag to track if the waveform is rising
    uint16_t v0; // variables for frequency evaluation
    uint16_t v1;
    uint16_t v2;

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

    // find true rms voltage
    // rms = sqrt((Vpp/2)^2 + Vdc_offset^2)
    p_buffer->rms_volts = sqrt((p_buffer->peak_peak / 2) * (p_buffer->peak_peak / 2) + (p_buffer->dc_offset * p_buffer->dc_offset));

    // find frequency
    p_buffer->frequency = 0; // initialize to 0
    // we get some erroneous reading at low frequencies, so check
    // rising or falling for TWO consecutive samples before accepting
    // direction change
    v0 = p_buffer->dc_offset; // reference point for slope sign change, mid-way up the wave
    for (i = 1; i < SAMPLES_PER_SECOND; i++) {
        v1 = p_buffer->samples[i];
        v2 = p_buffer->samples[i-1];
        if (currently_rising == 0) {
            if ((v1 > v0) && (v2 > v0)) {
                currently_rising = 1;
                p_buffer->frequency++;
            }
        }
        else {
            if ((v1 < v0) && (v2 < v0)) {
                currently_rising = 0;
            }
        }
    }
    // adjust for calibration of frequency values
    if (p_buffer->frequency > 890) {
        p_buffer->frequency += 5;
    }
    else if (p_buffer->frequency > 690) {
        p_buffer->frequency += 4;
    }
    else if (p_buffer->frequency > 590) {
        p_buffer->frequency += 3;
    }
    else if (p_buffer->frequency > 390) {
        p_buffer->frequency += 2;
    }
    else if (p_buffer->frequency > 290) {
        p_buffer->frequency += 1;
    }
    else {
        // no adjustment needed
    }
}

// return AC values for printing
void adc_get_values_ac(uint32_t *p_frequency, uint32_t *p_dc_offset, uint32_t *p_vpp, uint32_t *p_rms_volts)
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
    adc_analyze_buffer_ac(p_buffer);
    *p_frequency = p_buffer->frequency;
    *p_dc_offset = (p_buffer->dc_offset*100)/CALIBRATION_CONSTANT; // *100 to prevent floating point
    *p_vpp = (p_buffer->peak_peak*100)/CALIBRATION_CONSTANT;
    *p_rms_volts = (p_buffer->rms_volts*100)/CALIBRATION_CONSTANT;
    p_buffer->buffer_ready_to_read = 0; // done with values, give buffer to IRQ
}

// analyze DC buffer values
static void adc_analyze_buffer_dc(sample_buffer_t *p_buffer)
{
    int i;
    // average over a period of less than 0.1s
    p_buffer->dc_volts = 0;
    for (i = 0; i < ((SAMPLES_PER_SECOND / 10) - 1); i++) {
        p_buffer->dc_volts += p_buffer->samples[i];
    }
    p_buffer->dc_volts /= ((SAMPLES_PER_SECOND / 10) - 1);
}

// return DC values for printing
void adc_get_values_dc(uint32_t *p_dc_volts)
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
    adc_analyze_buffer_dc(p_buffer);
    *p_dc_volts = (p_buffer->dc_volts*100)/CALIBRATION_CONSTANT;
    p_buffer->buffer_ready_to_read = 0; // give buffer back to IRQ
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
