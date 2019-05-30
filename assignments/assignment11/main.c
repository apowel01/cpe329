/*
 * main.c
 *
 *  Created on: May 29, 2019
 *      Author: Andy Hunt
 */
#include "msp.h"
#include "timer_a.h"
#include "delay.h"
#include "LCD.h"
#include "keypad.h"



// creates array of values for square wave
// variable duty cycle from 10% to 90% controlled by keypad

void main(void) {

    int digit = 0;
    uint16_t degrees = 90;
    uint16_t new_degrees = degrees;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz); // set DCO to 3MHz
    timer_a_init();
    keypadinit();


    __enable_irq();

    // create a pulse signal with a 1.5ms pulse and 20 ms period using a TimerA output
    // 7.5% duty cycle

    timer_A_PWM_set(degrees); // Set PWM to initial value

    while(1) {
        digit = keypad_get_digit();
        if (digit == KEYPAD_STAR) {
            if (degrees >= 10) {
                new_degrees = degrees - 10;
            }
        }
        else if (digit == KEYPAD_HASH) {
            if (degrees <= 170) {
                new_degrees = degrees + 10;
            }
        }
        else {
            new_degrees = keypad_get_number(1);
            new_degrees += digit*10;

        }
        if (new_degrees <= 18) {
            degrees = new_degrees;
            timer_A_PWM_set(degrees);
        }
    }



}


