/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: trevo
 */
#include "msp.h"
#include "led.h"
#include "delay.h"


void TA0_0_IRQHandler(void)
{
    // make sure the GPIOs are triggered at the same point in the 2 ISRs,
    // that will minimize any skew caused by instruction processing time
    led_1_on();
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
}

void TA0_N_IRQHandler(void)
{
    led_1_off();
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
}

void timer_a_init(void)
{

    // set up the clocks
    delay_set_dco(FREQ_24_0_MHz);

    // initialize on LED1
    led_1_init();

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode
    // set CCR0 to 25KHz and use reset mode
    // 25KHz is a 40uS period, a 24MHz clock does 24 ticks per uS
    // so the period is 24*40 = 960 clock ticks
    TIMER_A0->CCR[0] = 960;
    // set CCR1 to give us a 25% duty cycle - 240 ticks
    TIMER_A0->CCR[1] = 240;

//    TIMER_A0->CCR[0] = 16339; // 32.678 kHz from ACLK = 32678/2 Hz
//    TIMER_A0->CCR[1] = 24508; // 0.75 * SMCLK
    // clear flags here, get code later //
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR
    __enable_irq(); // enable global interrupts

    while (1) {

    }
}

