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

// special case, we need to ignore the HASH symbol
int lock_get_digit()
{
    int digit = KEYPAD_HASH;

    while (KEYPAD_HASH == digit) {
        digit = keypad_get_digit();
    }

    return digit;
}
uint8_t lock_get_pin(void)
{
//    uint8_t pin_valid;
    int number;
    char key_str;
    int pin_attempt = 0;

    // read & display first number
    number = lock_get_digit();
    key_str = 0x30 + number; // convert to ascii
    Write_char_LCD(key_str);
    if (KEYPAD_STAR == number) {
        lcd_backspace();
        return 0;
    }
    pin_attempt = 1000 * number;
    // if * return, else read and display keypress
    number = lock_get_digit();
    key_str = 0x30 + number; // convert to ascii
    Write_char_LCD(key_str);
    if (KEYPAD_STAR == number) {
        lcd_backspace();
        lcd_backspace();
        return 0;
    }
    pin_attempt += 100 * number;
    // if * return, else read and display keypress
    number = lock_get_digit();
    key_str = 0x30 + number; // convert to ascii
    Write_char_LCD(key_str);
    if (KEYPAD_STAR == number) {
        lcd_backspace();
        lcd_backspace();
        lcd_backspace();
        return 0;
    }
    pin_attempt |= 10 * number;
    // if * return, else read and display keypress
    number = lock_get_digit();
    key_str = 0x30 + number; // convert to ascii
    Write_char_LCD(key_str);
    if (KEYPAD_STAR == number) {
        lcd_backspace();
        lcd_backspace();
        lcd_backspace();
        lcd_backspace();
        return 0;
    }
    pin_attempt += number;

    return 1;
}
void lock_locked(void)
{
    Write_string_LCD("Hello World");
    lock_get_pin();
}

void lock_test(void)
{
    lock_locked();

    // read first number

    // if * return, else read and display keypress
    // if * return, else read and display keypress
    // if * return, else read and display keypress
}
