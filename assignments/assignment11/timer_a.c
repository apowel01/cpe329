/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: Amberley
 */

#include "delay.h"
#include "msp.h"

#define PULSE_ON 0x1194
#define PULSE_PERIOD 0xEA60


// Timer A interrupt handler, just starts ADC sample
void TA0_N_IRQHandler(void)
{

    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

}

// initalize Timer A - correct rate is clock freq / # samples req'd in a second.
void timer_a_init(void)
{
    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 // up mode
            | TIMER_A_CTL_MC_1; // toggle/reset mode
    TIMER_A0->CCR[0] = PULSE_PERIOD;                      // 20ms pulse wait
    TIMER_A0->CCR[1] = PULSE_ON;                     // 1.5ms pulse on
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE
            | TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR0 ISR
}

