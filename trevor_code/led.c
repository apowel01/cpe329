/*
 * delay.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "led.h"

void led_1_init(void)
{
    // configure P1 as GPIO by setting select to 00b
    P1->SEL0 &= ~BIT0;
    P1->SEL1 &= ~BIT0;

    // Set output direction for the LED
    P1->DIR |= BIT0;
}

void led_1_on(void)
{
    P1->OUT |= BIT0;
}

void led_1_off(void)
{
    P1->OUT &= ~BIT0;
}

void led_2_init(void)
{
    // configure P1 as GPIO by setting select to 00b
    P2->SEL0 &= ~(BIT0 | BIT1 | BIT2);
    P2->SEL1 &= ~(BIT0 | BIT1 | BIT2);

    // Set output direction for the LED
    P2->DIR |= (BIT0 | BIT1 | BIT2);
}

void led_2_red_on(void)
{
    P2->OUT |= BIT0;
}

void led_2_red_off(void)
{
    P2->OUT &= ~BIT0;
}
void led_2_green_on(void)
{
    P2->OUT |= BIT1;
}

void led_2_green_off(void)
{
    P2->OUT &= ~BIT1;
}
void led_2_blue_on(void)
{
    P2->OUT |= BIT2;
}

void led_2_blue_off(void)
{
    P2->OUT &= ~BIT2;
}
