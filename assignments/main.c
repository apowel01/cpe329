#include "msp.h"
#include "delay.h"

static void set_P1_GPIO_output(void)
{
    // configure P1 as GPIO by setting select to 00b
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;

    // Set output direction
    P1->DIR |= BIT0;
}

static void set_P2_GPIO_output(void)
{
    // configure P2 as GPIO by setting select to 00b
    P2->SEL0 &= ~BIT0;
    P2->SEL1 &= ~BIT0;

    // Set output direction
    P2->DIR |= BIT0;
}

static void set_P4_MCLK_output(void)
{
    // configure P4 as GPIO by setting select to 00b
    P4->SEL0 |= BIT3;
    P4->SEL1 &= ~BIT3;

    // Set output direction
    P4->DIR |= BIT3;
}

static void set_LED1_on(void)
{
    P1->OUT |= BIT0;
}

static void set_LED1_off(void)
{
    P1->OUT &= ~BIT0;
}

static void set_LED2_on(void)
{
    P2->OUT |= BIT0;
}

static void set_LED2_off(void)
{
    P2->OUT &= ~BIT0;
}

/*static void set_LED2_color(int red; int green; int blue;)
{
    if (red == 1){
        P2->OUT |= BIT1;
    }
    else{
        P2->OUT &= ~BIT1;
    }

    if (green == 1){
        P2->OUT |= BIT2;
    }
    else{
        P2->OUT &= ~BIT2;
    }

    if (blue == 1){
        P2->OUT |= BIT3;
    }
    else{
        P2->OUT &= ~BIT3;
    }

}
*/

/**
 * main.c
 */
int main(void)
{

    set_DCO(FREQ_12_0_MHz);              // set the DCO frequency to 6 MHz
    set_P1_GPIO_output();
    set_LED1_off();
    delay_us(50000);
    set_LED1_on();

    set_P2_GPIO_output();
    set_LED2_off();
    set_LED2_on();
    //set_LED2_color(red, green, blue);

    set_LED1_off();                     // turn LED1 off
    set_LED2_off();                     // turn LED2 off

    set_P4_MCLK_output();
    while(1){
        P4->OUT ^= BIT3;
    }

/*    while (1) // continuous loop
    {
        P1->OUT ^= BIT0;  // blink P1.0 LED
        P2->OUT &= ~(BIT0 | BIT1 | BIT2);
        P2->OUT |= (color & 0X07);
        for (i = 63535; i> 0; i--); // Delay
        color++;
              P2
    }
*/

    // change color of LED2
	// WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
}
