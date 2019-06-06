/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: Amberley
 */

#include "delay.h"
#include "msp.h"

#define PULSE_PERIOD 0x0001  // 20 ms : 0xEA60
#define PULSE_BASE 1475
#define PULSE_INCREMENT 305

uint16_t countflag = 0;
uint32_t p = 0;

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    //P5->OUT |= BIT6;
}

void TA0_N_IRQHandler(void)
{
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    //P5->OUT &= ~BIT6;
}

// Setting PWM Signal Pulse Width
void timer_A_PWM_set(uint16_t degrees)
{
    TIMER_A0->CCR[1] = PULSE_BASE + (PULSE_INCREMENT * degrees);    // Set Pulse Width

}


// initalize Timer A - correct rate is clock freq / # samples req'd in a second.
void timer_a_init(void)
{
    P5->DIR |= BIT6;  // GPIO bits, 0 == 25KHz clock, 1 = IRQ execution time

      TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                              // to use SMCLK in UP mode


      TIMER_A0->CCR[0] = PULSE_PERIOD;
      //set motor to 90 degrees
      //timer_A_PWM_set(9);
/*      TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
      TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
      NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
      NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR
*/


#if 0
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |=
            | TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0

#endif
}







