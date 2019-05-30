/*
 * main.c
 *
 *  Created on: May 29, 2019
 *      Author: Andy Hunt
 */

#include "timer_a.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"

void main(void) {

    // Initialize GPIO for Pulse as low set as output

    P_->DIR |= BIT_; // Enable GPIO out on pin ___

    timer_a_init(); //Initialize timer

    while(1) {
        if()
    }



}


