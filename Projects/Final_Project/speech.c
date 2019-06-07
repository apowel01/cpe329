/*
 * speech.c
 *
 *  Created on: Jun 6, 2019
 *      Author: Amberley
 *
 *  It turns out that the speakjet chip UART is not traditional, it is logic HIGH instead of LOW and uses a
 *  lower voltage than normal UART.  With this in mind, we use a GPIO Pin to "bit bang" the characters using
 *  8-none-1 because that is the speakjet default.  We will use timer_a to time the pulses.
 */
#include "msp.h"
#include "pmod_color.h"

#define BLUE_PHONEME "\BO \FAST \LO \IHWW"
#define RED_PHONEME "\RR \SLOW \EH \ED"
#define GREEN_PHONEME "\Slow \GO \FAST \RR \SLOW \IY \NE"
#define CLEAR_PHONEME "\SLOW \KO \FAST \LE \IYRR"
#define BLACK_PHONEME "\DO \AWRR \P4 \EK"

#define SPEECH_BLUE_THRESHOLD 8192
#define SPEECH_RED_THRESHOLD 8192
#define SPEECH_GREEN_THRESHOLD 8192
#define SPEECH_CLEAR_THRESHOLD 8192

static uint8_t bit_buffer[10] = {1,0,0,0,0,0,0,0,0,0}; // defines start and stop conditions - bits 0 and 9
static uint8_t character_sending = 0;
static uint8_t current_bit = 0;

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // if we are not sending a character, keep the line low
    if (character_sending == 0) {
        P6->OUT &= ~BIT0;
        return;
    }
    if (bit_buffer[current_bit] == 0) {
        P6->OUT &= ~BIT0;
    }
    else {
        P6->OUT |= BIT0;
    }
    // check for completion
    current_bit++;
    if (current_bit > 9) {
        current_bit = 0;  // ready for next character
        character_sending = 0; // allow next character to be loaded into bit_buffer
    }
}

// this is where we set timer a for a pulse that gives us 9600 baud
static void rcx_init(void)
{
    P2->DIR |= BIT0;
    P2->OUT |= BIT0;

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

    // we have a 3Mhz clock, so 9600 baud is 1 transition every
    // 3000000/9600 = 312 clocks
    TIMER_A0->CCR[0] = 312;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
}

static void rcx_send_string(char * p_string)
{
    uint8_t i;

    // check for termination
    while (0 != *p_string) {
        while (character_sending) {
            // wait for the timer to send the character
        }
        // set the bits in the array
        for (i=0; i<8; i++) {
            bit_buffer[i+1] = ((*p_string & (1 << i)) > 0) ? 1 : 0;
        }
        p_string++;
        character_sending = 1;
    }
}

static uint8_t speech_needs_blue(pmod_colors_t * p_colors)
{
    uint8_t blue = 0;

    if (p_colors->blue > SPEECH_BLUE_THRESHOLD) {
        blue = 1;
    }

    return blue;
}

static uint8_t speech_needs_red(pmod_colors_t * p_colors)
{
    uint8_t red = 0;

    if (p_colors->red > SPEECH_RED_THRESHOLD) {
        red = 1;
    }

    return red;
}

static uint8_t speech_needs_green(pmod_colors_t * p_colors)
{
    uint8_t green = 0;

    if (p_colors->green > SPEECH_GREEN_THRESHOLD) {
        green = 1;
    }

    return green;
}

static uint8_t speech_needs_clear(pmod_colors_t * p_colors)
{
    uint8_t clear = 0;

    if (p_colors->clear > SPEECH_CLEAR_THRESHOLD) {
        clear = 1;
    }

    return clear;
}

void speech_say(pmod_colors_t * p_colors)
{
    uint8_t color_spoken = 0;

    if (speech_needs_clear(p_colors)) {
        rcx_send_string(CLEAR_PHONEME);
        color_spoken = 1;
    }
    if (speech_needs_blue(p_colors)) {
        rcx_send_string(BLUE_PHONEME);
        color_spoken = 1;
    }
    if (speech_needs_green(p_colors)) {
        rcx_send_string(GREEN_PHONEME);
        color_spoken = 1;
    }
    if (speech_needs_red(p_colors)) {
        rcx_send_string(RED_PHONEME);
        color_spoken = 1;
    }

    if (0 == color_spoken) {
        rcx_send_string(BLACK_PHONEME);
    }
}

void speech_init(void)
{
    rcx_init();
}
