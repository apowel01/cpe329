/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"
#include "adc.h"

// Timer A interrupt handler, just starts ADC sample
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    adc_sample_start(); // start taking a sample on the ADC
}

// initalize Timer A - correct rate is clock freq / # samples req'd in a second.
void timer_a_init(void)
{
    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                                // to use SMCLK (48M) in UP mode
    TIMER_A0->CCR[0] = delay_get_dco_freq() / adc_get_samples_per_second();
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
}

