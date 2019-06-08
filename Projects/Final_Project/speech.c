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
#include "LCD.h"

// The allophone library for the speakjet uses "\" in front of an allophone to
// tell the speakjet how to process it (an escape character).  The compile also sees them as
// an escape character for special processing, so in the constant we use "\\" to escape the escape
// character, this means the compiler strips the first "\" and leaves the second one, so we get
// the correct string for transmission
#define BLUE_PHONEME "\\BO \\FAST \\LO \\IHWW"
//#define BLUE_PHONEME "\BO \FAST \LO \IHWW"
#define RED_PHONEME "\\RR \\SLOW \\EH \\ED"
//#define RED_PHONEME "\RR \SLOW \EH \ED"
#define GREEN_PHONEME "\\Slow \\GO \\FAST \\RR \\SLOW \\IY \\NE"
//#define GREEN_PHONEME "\\Slow \GO \FAST \RR \SLOW \IY \NE"
#define BLACK_PHONEME "\\DO \\ AWRR \\P4 \\EK" //        \\SPEED, 50 \\STRESS
//#define BLACK_PHONEME "\DO \AWRR \P4 \EK"

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
    P6->DIR |= BIT0;
    P6->OUT |= BIT0;

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
//            bit_buffer[i+1] = ((*p_string & (1 << i)) > 0) ? 1 : 0;
            bit_buffer[9-i] = ((*p_string & (1 << i)) > 0) ? 1 : 0;
        }
        p_string++;
        character_sending = 1;
    }
}

void speech_say(pmod_result_t * p_results)
{
    uint8_t color_spoken = 0;
    Clear_LCD();

    if (p_results->blue) {
        rcx_send_string(BLUE_PHONEME);
        Write_string_LCD("Blue ");
        color_spoken = 1;
    }
    if (p_results->green) {
        rcx_send_string(GREEN_PHONEME);
        Write_string_LCD("Green ");
        color_spoken = 1;
    }
    if (p_results->red) {
        rcx_send_string(RED_PHONEME);
        Write_string_LCD("Red ");
        color_spoken = 1;
    }

    if (0 == color_spoken) {
        rcx_send_string(BLACK_PHONEME);
        Write_string_LCD("Dark ");
    }
}

void speech_init(void)
{
    rcx_init();
}
