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


void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    P6->OUT |= BIT0;
}

void TA0_N_IRQHandler(void)
{
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    P6->OUT &= ~BIT0;

}



// initalize Timer A - correct rate is clock freq / # samples req'd in a second.
void timer_a_init(void)
{
    P2->DIR |= BIT5;  // GPIO bits, 0 == 25KHz clock, 1 = IRQ execution time


      TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                              // to use SMCLK in UP mode

      // set CCR0 to 25KHz and use reset mode
      // 25KHz is a 40uS period, a 24MHz clock does 24 ticks per uS
      // so the period is 24*40 = 960 clock ticks
      TIMER_A0->CCR[0] = PULSE_PERIOD;
      // set CCR1 to give us a 25% duty cycle - 240 ticks
      TIMER_A0->CCR[1] = PULSE_ON;    // clear flags here, get code later //
      TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
      TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
      NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
      NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR



#if 0
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |=
            | TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0

#endif
}

