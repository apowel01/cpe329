/*
 * func_gen.c
 *
 *  Created on: May 3, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "keypad.h"
#include "LCD.h"
#include "delay.h"
#include "math.h"
#include "dac.h"

// defines
// LCD management defines
#define WAVEFORM_POSITION 0
#define WAVEFORM_SQUARE     "Square  "
#define WAVEFORM_SINE       "Sine    "
#define WAVEFORM_SAWTOOTH   "Sawtooth"
#define FREQ_POSITION 40
#define FREQ_UNIT_POSITION 43
#define FREQ_UNIT "Hz"
#define DUTY_CYCLE_POSITION 48
#define DUTY_CYCLE_UNIT_POSITION 50
#define DUTY_CYCLE_UNIT "%"
#define DUTY_CYCLE_UNUSED "--"
// array sample create defines
#define VPP_MULTIPLIER 2
#define MAX_SAMPLES (96 * 5)
#define VDD 4095
#define VDD_HALF 2047
#define SIGNAL_MAX_OFFSET MAX_SAMPLES
#define NVIC_INT_CONST 31
#define PI 3.14159265
// func. gen. defines
#define CCR0_VALUE 250 // must be within 75% of maximum speed

// typedefs
typedef enum {
    waveform_type_min_e = 6,
    waveform_type_square_e = 7,
    waveform_type_sine_e = 8,
    waveform_type_sawtooth_e = 9,
    waveform_type_max_e = 10
}waveform_type_e;

typedef enum {
    sample_100Hz = 1,
    sample_200Hz = 2,
    sample_300Hz = 3,
    sample_400Hz = 4,
    sample_500Hz = 5
}sample_increment_e;

// local variables
static waveform_type_e waveform_type = waveform_type_square_e;
static uint16_t sine_samples[MAX_SAMPLES];
static uint16_t sawtooth_samples[MAX_SAMPLES];
static uint16_t square_samples[MAX_SAMPLES];
static uint16_t current_sample = 0;
static sample_increment_e sample_increment = 1;
static uint16_t duty_cycle = 5;

// creates array of values for square wave
// variable duty cycle from 10% to 90% controlled by keypad
static void create_square_samples(void)
{
    int i;
    for (i = 0; i < (MAX_SAMPLES * duty_cycle)/10; i++) {
        square_samples[i] = VDD_HALF + (SIGNAL_MAX_OFFSET * VPP_MULTIPLIER);
    }
    for (; i < MAX_SAMPLES; i++) { // use existing value of i to preserve duty cycle changes
        square_samples[i] = VDD_HALF - (SIGNAL_MAX_OFFSET * VPP_MULTIPLIER);
    }
}

// creates array of values for sine wave
static void create_sine_samples(void)
{
    int i;
    double param;
    double result;
    for (i = 0; i < MAX_SAMPLES; i++) {
        param = i; // converts i from an int to a double
        result = sin((param * PI) / (MAX_SAMPLES/2)); // get sin value using radians
        sine_samples[i] = (uint16_t)((result * SIGNAL_MAX_OFFSET * VPP_MULTIPLIER) + VDD_HALF); // generate sample and convert to uint16_t
    }
}

// creates array of sawtooth samples
static void create_sawtooth_samples(void)
{
    int i;
    for (i = 0; i < MAX_SAMPLES; i++) {
        sawtooth_samples[i] = (VDD_HALF - (SIGNAL_MAX_OFFSET * VPP_MULTIPLIER)) + (i * 2 * VPP_MULTIPLIER);
    }
}

// interrupt handler which drives the DAC outputs
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

    // select waveform
    switch (waveform_type) {
    case waveform_type_square_e:
        dac_send(square_samples[current_sample]);
        break;
    case waveform_type_sine_e:
        dac_send(sine_samples[current_sample]);
        break;
    case waveform_type_sawtooth_e:
        dac_send(sawtooth_samples[current_sample]);
        break;
    }
    current_sample += sample_increment;
    if (current_sample >= MAX_SAMPLES) {
        current_sample = 0; // reset after reaching end of sample index
    }
}

// initialize timer a
void func_gen_timer_a_init(uint16_t up_count)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enable
    TIMER_A0->CCR[0] = up_count;  // CCR0 counter
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // UP count, SMCLK
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & NVIC_INT_CONST);  // NVIC interrupt
}

// call this to run the whole program - main loop
void func_gen_main(void)
{
    int digit;
    char *p_waveform = WAVEFORM_SQUARE; // pointer to waveform type

    // init board clock speed
    delay_set_dco(FREQ_12_0_MHz); // must be less than 20 MHz to meet DAC requirements

    // init DAC
    dac_init();

    // init LCD
    lcd_init();

    // init keypad
    keypadinit();

    // create waveform arrays
    create_square_samples();
    create_sine_samples();
    create_sawtooth_samples();

    // set default waveform (square wave 50% duty cycle)
    waveform_type = waveform_type_square_e;
    duty_cycle = 5; // 5/10 = 50% duty cycle
    sample_increment = sample_100Hz;

    // update display
    Clear_LCD();
    Home_LCD();
    lcd_position_set(FREQ_UNIT_POSITION);
    Write_string_LCD(FREQ_UNIT);
    lcd_position_set(DUTY_CYCLE_UNIT_POSITION);
    Write_string_LCD(DUTY_CYCLE_UNIT);

    // init timerA
    func_gen_timer_a_init(CCR0_VALUE); // TimerA CCR0 value is 250, within 75% of maximum rate found to be 215.

    while (1) {
        // update display
        lcd_position_set(WAVEFORM_POSITION);
        Write_string_LCD(p_waveform);
        lcd_position_set(FREQ_POSITION);
        Write_number_LCD(sample_increment * 100); // convert sample_increment (frequency) to Hz
        lcd_position_set(DUTY_CYCLE_POSITION);
        if (waveform_type == waveform_type_square_e) { // only if waveform type is a square wave
            Write_number_LCD(duty_cycle * 10); // convert duty cycle to % (e.g. 5 * 10 = 50%)
        }
        else { // if it is sine or sawtooth display '--'% for duty cycle
            Write_string_LCD(DUTY_CYCLE_UNUSED);
        }


        // enable interrupts
        __enable_irq();  // global enable

        // read the keypad
        digit = keypad_get_digit();

        // disable interrupts
        __disable_irq(); // global disable

        // change waveform based on input
        switch (digit) { // case numbers/chars match keypad labels
        case 1: // change waveform frequency to 100 Hz
            sample_increment = sample_100Hz;
            break;
        case 2: // change waveform frequency to 200 Hz
            sample_increment = sample_200Hz;
            break;
        case 3: // change waveform frequency to 300 Hz
            sample_increment = sample_300Hz;
            break;
        case 4: // change waveform frequency to 400 Hz
            sample_increment = sample_400Hz;
            break;
        case 5: // change waveform frequency to 500 Hz
            sample_increment = sample_500Hz;
            break;
        case 6: // does nothing
            continue;
            // break;
        case 7: // change waveform type to square wave
            waveform_type = waveform_type_square_e;
            p_waveform = WAVEFORM_SQUARE;
            break;
        case 8: // change waveform type to sine wave
            waveform_type = waveform_type_sine_e;
            p_waveform = WAVEFORM_SINE;
            break;
        case 9: // change waveform type to sawtooth wave
            waveform_type = waveform_type_sawtooth_e;
            p_waveform = WAVEFORM_SAWTOOTH;
            break;
        case KEYPAD_STAR: // change duty cycle by -10%
            if (duty_cycle > 1) { // min 10% duty cycle
                duty_cycle--;
            }
            create_square_samples();
            break;
        case 0: // change duty cycle back to 50%
            duty_cycle = 5;
            create_square_samples();
            break;
        case KEYPAD_HASH: // change duty cycle by +10%
            if (duty_cycle < 9) { // max 90% duty cycle
                duty_cycle++;
            }
            create_square_samples(); // regen. square array with
                                     // adjusted duty cycle
            break;
        default: // we should never get here
            continue;
            // break;
        }
    }
}
