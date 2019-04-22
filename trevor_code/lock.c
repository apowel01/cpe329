/*
 * lock.c
 *
 *  Created on: Apr 21, 2019
 *      Author: trevo
 */

#include "msp.h"
#include "keypad.h"
#include "LCD.h"
#include "delay.h"
#include "led.h"

#define PIN_FAIL_INCREMENT 5
#define UNLOCK_TIME  10
#define RELOCK_WARNING_TIME 10

static int stored_pin = 1234;
static int pin_fail_penalty = 0;

// special case, we need to ignore the HASH symbol
int lock_get_and_print_digit()
{
    char key_str;
    int digit = KEYPAD_HASH;

    while (KEYPAD_HASH == digit) {
        digit = keypad_get_digit();
    }
    key_str = 0x30 + digit; // convert to ascii
    Write_char_LCD(key_str);

    return digit;
}
int lock_get_pin(void)
{
//    uint8_t pin_valid;
    int digit = 0;
    int pin_attempt = 0;

    // first digit is the thousands
    digit = lock_get_and_print_digit();
    if (KEYPAD_STAR == digit) {
        return 0;
    }
    pin_attempt = 1000 * digit;
    // second digit is hundreds
    digit = lock_get_and_print_digit();
    if (KEYPAD_STAR == digit) {
        return 0;
    }
    pin_attempt += 100 * digit;
    // third digit is tens
    digit = lock_get_and_print_digit();
    if (KEYPAD_STAR == digit) {
        return 0;
    }
    pin_attempt += 10 * digit;
    // fourth/last digit
    digit = lock_get_and_print_digit();
    if (KEYPAD_STAR == digit) {
        return 0;
    }
    pin_attempt += digit;

    return pin_attempt;
}
void lock_locked(void)
{
    Clear_LCD();
    Home_LCD();

    // set LED 2 red
    led_2_off();
    led_2_red_on();
    // put message on display
    Write_string_LCD("LOCKED");
    lcd_position_set(0x40);
    Write_string_LCD("Enter Pin: ");
}

void lock_init()
{
    lcd_init(); // initialize LCD display
    Clear_LCD();
    Home_LCD();
    keypadinit(); // initialize keypad
    led_2_init();
}

void lock_unlocked(void)
{
    Clear_LCD();
    Home_LCD();
    Write_string_LCD("UNLOCKED");
    // set LED 2 green
    led_2_off();
    led_2_green_on();
    // reset the failure penalty
    pin_fail_penalty = 0;
    delay_sec(UNLOCK_TIME);
}

void lock_frozen(void)
{
    Clear_LCD();
    Home_LCD();
    Write_string_LCD("ATTEMPT FAILED");
    lcd_position_set(0x40);
    Write_string_LCD("RESET IN ");
    // set LED 2 blue
    led_2_off();
    led_2_blue_on();

    // the penalty for failure gets worse....
    pin_fail_penalty += PIN_FAIL_INCREMENT;
    Write_number_LCD(pin_fail_penalty);
    Write_string_LCD("s");
    delay_sec(pin_fail_penalty);
}

void lock_relock(void)
{
    Clear_LCD();
    Home_LCD();
    Write_string_LCD("RELOCK IN ");
    Write_number_LCD(RELOCK_WARNING_TIME);
    Write_string_LCD("s");
    delay_sec(RELOCK_WARNING_TIME);
}

void lock_main(void)
{
    int pin_attempt = 0;

    lock_init();

    // infinite loop
    while (1) {
        pin_attempt = 0;

        // start in the locked state
        while (0 == pin_attempt) {
            lock_locked();
            pin_attempt = lock_get_pin();
        }

        // we have a pin, see if it is valid
        if (pin_attempt != stored_pin) {
            lock_frozen();
            continue;
        }

        // pin was valid - unlock
        lock_unlocked();

        // lock_relock (after some time)
        lock_relock();

        // and go to the beginning of the loop to
        // unlock again
    } // infinite loop
}
