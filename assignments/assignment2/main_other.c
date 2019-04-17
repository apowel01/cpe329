#include "msp.h"
#include "delay.h"

// use for reading MCLK frequency using pin 4.3
static void set_P4_MCLK_output(void)
{
    // configure P4.3 to output MCLK by setting select to 10b
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;

    // Set output direction
    P4->DIR |= BIT3;
}

static void set_P1_GPIO_output(void)
{
    // configure P1 as GPIO by setting select to 00b
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;

    // Set output direction
    P1->DIR |= BIT0;
}

// turn LED1 off
static void set_LED1_off(void)
{
    P1->OUT &= ~BIT0;
}

// turn LED1 on
static void set_LED1_on(void)
{
    P1->OUT |= BIT0;
}

// turns LED2 on
static void set_LED2_on(void)
{
    P2->OUT |= BIT0;
}

// turn LED2 off
static void set_LED2_off(void)
{
    P2->OUT &= ~BIT0;
}

static void set_P2_GPIO_output(void)
{
    // configure P2 as GPIO by setting select to 00b
    P2->SEL0 &= ~BIT0;
    P2->SEL1 &= ~BIT0;

    // Set output direction
    P2->DIR |= BIT0;
}

/**
 * main.c
 */
int main(void)
{
    int freq = FREQ_48_MHz;
    set_DCO(freq); // set DCO to specified frequency: 1.5, 3, 6, 12, 24 or 48 MHz
    set_P4_MCLK_output(); // set pin 4.3 to output MCLK (for scope measurement)

    /*// configure LEDs 1 and 2



    set_P2_GPIO_output();
    set_LED2_off();
    set_LED2_on();

    set_LED1_off(); // turn LED1 off
    set_LED2_off(); // turn LED2 off*/

    set_P1_GPIO_output(); // select port one bit 0 to use GPIO output
    set_LED1_on(); // demonstrate that LED1 turns on
    delay_us(40,freq);
    //delay_ms(40,get_DCO_MHz());
    set_LED1_off(); // make sure LED1 is off

    while(1) {
        P4->OUT ^= BIT3;
    }
}
