/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: Amberley
 */
#define PART_NONE

#include "msp.h"
#include "delay.h"

#ifdef PART_A
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // make sure the GPIOs are triggered at the same point in the 2 ISRs,
    // that will minimize any skew caused by instruction processing time
    P6->OUT |= BIT0;
}

void TA0_N_IRQHandler(void)
{
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    P6->OUT &= ~BIT0;
}
#endif

#if (defined PART_B) || (defined PART_C)
static uint8_t IRQ_toggle = 0;
void TA0_0_IRQHandler(void)
{
    P6->OUT |= BIT1;  // measure IRQ execution time
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

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



    P6->OUT &= ~BIT1;  // end measure IRQ execution time
}
#endif

#ifdef PART_D
static uint8_t IRQ_toggle = 0;
static volatile uint16_t timer_extender_limit = 1000;
static uint16_t timer_extender_count = 0;
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // count up to the extender limit befrooe toggling the GPIO
    if (timer_extender_limit > timer_extender_count) {
        timer_extender_count++;
    }
    else {
        if (0 == IRQ_toggle) {
            led_1_on();
            IRQ_toggle = 1;
        }
        else {
            led_1_off();
            IRQ_toggle = 0;
        }
        timer_extender_count = 0;
    }

}
#endif

#ifdef PART_E
static uint8_t IRQ_0_toggle = 0;
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // make sure the GPIOs are triggered at the same point in the 2 ISRs,
    // that will minimize any skew caused by instruction processing time
    if (0 == IRQ_0_toggle) {
        P6->OUT |= BIT0;
        IRQ_0_toggle = 1;
    }
    else {
        P6->OUT &= ~BIT0;
        IRQ_0_toggle = 0;
    }


}
static uint8_t IRQ_N_toggle = 0;
static volatile uint16_t timer_extender_limit = 1;
static uint16_t timer_extender_count = 0;
void TA0_N_IRQHandler(void)
{
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag

    // count up to the extender limit befrooe toggling the GPIO
    if (timer_extender_limit > timer_extender_count) {
        timer_extender_count++;
    }
    else {
        if (0 == IRQ_N_toggle) {
            P6->OUT |= BIT1;
            IRQ_N_toggle = 1;
        }
        else {
            P6->OUT &= ~BIT1;
            IRQ_N_toggle = 0;
        }
        timer_extender_count = 0;
    }


}
#endif

#ifdef PART_F
// we want a 1mS counter that we can read from the main program
// declare a volatile and increment it from CCR0
// a 32bit mS counter will roll over in 49 days, so we do not care about handling that
static uint32_t game_ms_counter = 0;
static uint32_t start_value= 0;
static uint8_t IRQ_0_toggle = 0;
void TA0_0_IRQHandler(void)
{
    _disable_interrupts();
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    game_ms_counter += 1;
    // DEBUG - we create a GPIO square wave so we can see this is working
    if (0 == IRQ_0_toggle) {
        P6->OUT |= BIT0;
        IRQ_0_toggle = 1;
    }
    else {
        P6->OUT &= ~BIT0;
        IRQ_0_toggle = 0;
    }
    _enable_interrupts();
}
void PORT1_IRQHandler(void)
{
    _disable_interrupts();
    uint32_t delta;
    if (P1->IFG & BIT1) {
        // Button ONE press sets the start_count to the current
        // game counter
        start_value = game_ms_counter;
        // clear the display ready to print the value
        Clear_LCD();
        Home_LCD();
        P1->IFG &= ~BIT1; // clears button 1
    }
    if (P1->IFG & BIT4) {
        // we want the difference between current counter and the button 1 "start_value"
        delta = game_ms_counter - start_value;
        (void)delta;
        Write_number_LCD(delta);
        Write_string_LCD("mS");
        P1->IFG &= ~BIT4; // clear flag
    }

    _enable_interrupts();

}
#endif

void timer_a_init(void)
{
    P6->DIR |= BIT0 | BIT1;  // GPIO bits, 0 == 25KHz clock, 1 = IRQ execution time

#ifdef PART_A
    // set up the clocks
    delay_set_dco(FREQ_24_0_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

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
    // set up the clocks
    delay_set_dco(FREQ_24_0_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

    // SET MCLK to P4.3
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;
    P4->DIR |= BIT3;

    // set CCR0 to 25KHz and use reset mode
    // 25KHz is a 40uS period, a 24MHz clock does 24 ticks per uS
    // so the period is 24*40 = 960 clock ticks, 50% duty cycle = 480
    TIMER_A0->CCR[0] = 480; // 25KHz 50% duty cycle
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    __enable_irq(); // enable global interrupts

#endif
#ifdef PART_C    // set up the clocks
    delay_set_dco(FREQ_24_0_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

    // SET MCLK to P4.3
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;
    P4->DIR |= BIT3;

    // set CCR0 to be the smallest possible value that works.
    // Seems to be constrained at about 10uS, or about 240 ticks
    // Since the ISR takes about
    TIMER_A0->CCR[0] = 150;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    __enable_irq(); // enable global interrupts

#endif
#ifdef PART_D
    // set up the clocks
    delay_set_dco(FREQ_1_5_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

    // init led
    led_1_init();

    // We want a 10 Sec period, at 1.5MHz this is 15 million clock ticks.
    // for simplicity set CCR0 = 50000 and then the ISR will trigger 300 times
    // for 10 Sec
//    timer_extender_limit = 1000;
    TIMER_A0->CCR[0] = 15000;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    __enable_irq(); // enable global interrupts

#endif
#ifdef PART_E
    // set up the clocks
    delay_set_dco(FREQ_1_5_MHz);

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode

    // set CCR0 to 500uS and toggle it every IRQ
    TIMER_A0->CCR[0] = 750;
    // set CCR1 to 500uS and toggle it every 2nd IRQ
    TIMER_A0->CCR[1] = 750;    // clear flags here, get code later //
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR
    __enable_irq(); // enable global interrupts
#endif
#ifdef PART_F
    // set up a clock with a 1mS interval to be the counter
    delay_set_dco(FREQ_1_5_MHz);
    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                            // to use SMCLK in UP mode
    // init the LCD
    lcd_init();
    // set CCR0 to 1500
    TIMER_A0->CCR[0] = 1500;
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR

    // set up right and left buttons as interrupts
    P1->DIR &= ~(BIT1 | BIT4); // set as inputs
    P1->REN |= (BIT1 | BIT4); // enable resistors
    P1->OUT |= (BIT1 | BIT4); // set resistors to pull-up
    P1->IES |= (BIT1 | BIT4); // set interrupt on high -> low
    P1->IFG &= ~(BIT1 | BIT4); // clear interrupt flags
    P1->IE |= (BIT1 | BIT4); // enable interrupts GPIO
    /* pretty much ISER[1] only for GPIO,
    everything else will be ISER[0] - Dr. Hummel */
    NVIC->ISER[1] = 1 << (PORT1_IRQn & 31); // enable GPIO interrupts in NVIC
    Clear_LCD();
    Home_LCD();

//    __enable_irq(); //enable global interrupts (ARM auto clears the flags)

#endif

    while (1) {

    }
}

