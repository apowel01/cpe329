/*
 * buttons.c
 *
 *  Created on: May 19, 2019
 *      Author: Amberley
 */

// Right button and left button on launchpad
// left button P1.1 and right button P1.4

// detect each button press independently, left for AC right for DC
// buttons are shorts to ground when pressed
// since both buttons are connected to port 1, both will run the same ISR
#include "msp.h"
// handle AC/DC mode
static uint8_t mode_is_ac = 1;

// return if AC is the current mode
uint8_t buttons_get_mode_ac(void)
{
    return mode_is_ac;
}

// Port1 IRQ Handler for buttons 1 & 2 (left and right)
void PORT1_IRQHandler(void)
{
    if (P1->IFG & BIT1) {
        // button 1
        mode_is_ac = 1;
        P1->IFG &= ~BIT1; // clears button 1
    }
    if (P1->IFG & BIT4) {
        // button 4
        mode_is_ac = 0;
        P1->IFG &= ~BIT4; // clear flag
    }
}

// initialize GPIO and interrupts for buttons 1 & 2
void buttons_init(void)
{
    // set up right and left buttons as interrupts
    P1->DIR &= ~(BIT1 | BIT4); // set as inputs
    P1->REN |= (BIT1 | BIT4); // enable resistors
    P1->OUT |= (BIT1 | BIT4); // set resistors to pull-up
    P1->IES |= (BIT1 | BIT4); // set interrupt on high -> low
    P1->IFG &= ~(BIT1 | BIT4); // clear interrupt flags
    P1->IE |= (BIT1 | BIT4); // enable interrupts GPIO
    NVIC->ISER[1] = 1 << (PORT1_IRQn & 31); // enable GPIO interrupts in NVIC
}


