/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: trevo
 */
#define PART_B

#include "msp.h"
#include "led.h"
#include "delay.h"

#ifdef PART_A
void TA0_0_IRQHandler(void)
{
    // make sure the GPIOs are triggered at the same point in the 2 ISRs,
    // that will minimize any skew caused by instruction processing time
    P6->OUT |= BIT0;
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
}

void TA0_N_IRQHandler(void)
{
    P6->OUT &= ~BIT0;
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
}
#endif

#ifdef PART_B
static uint8_t IRQ_toggle = 0;
void TA0_0_IRQHandler(void)
{
    P6->OUT |= BIT1;  // measure IRQ execution time

    // make sure the GPIOs are triggered at the same point in the 2 ISRs,
    // that will minimize any skew caused by instruction processing time
    if (0 == IRQ_toggle) {
        P6->OUT |= BIT0;
        IRQ_toggle = 1;
    }
    else {
        P6->OUT &= ~BIT0;
        IRQ_toggle = 0;
    }


    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    P6->OUT &= ~BIT1;  // end measure IRQ execution time
}

#endif

void timer_a_init(void)
{
    P6->DIR |= BIT0 | BIT1;  // GPIO bits, 0 == 25KHz clock, 1 = IRQ execution time

    // set up the clocks
    delay_set_dco(FREQ_24_0_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode
#ifdef PART_A
    // set CCR0 to 25KHz and use reset mode
    // 25KHz is a 40uS period, a 24MHz clock does 24 ticks per uS
    // so the period is 24*40 = 960 clock ticks
    TIMER_A0->CCR[0] = 960;
    // set CCR1 to give us a 25% duty cycle - 240 ticks
    TIMER_A0->CCR[1] = 240;    // clear flags here, get code later //
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR
    __enable_irq(); // enable global interrupts

#endif
#ifdef PART_B
    // SET MCLK to P4.3
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;
    P4->DIR |= BIT3;

    // set CCR0 to 25KHz and use reset mode
    // 25KHz is a 40uS period, a 24MHz clock does 24 ticks per uS
    // so the period is 24*40 = 960 clock ticks, 50% duty cycle = 480
//    TIMER_A0->CCR[0] = 480;  25KHz 50% duty cycle
    TIMER_A0->CCR[0] = 60;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
#endif
#ifdef PART_c
    // SET MCLK to P4.3
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;
    P4->DIR |= BIT3;

    // set CCR0 to be the smallest possible value that works.  In
    // theory this is when we become bound by the ISR execution time
    // of about 2.4uS
    TIMER_A0->CCR[0] = 24;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
#endif

    while (1) {

    }
}

