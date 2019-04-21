/*
 * LCD.c
 *
 *  Created on: Apr 12, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "LCD.h"
#include "delay.h"

#define LCD_RS BIT5
#define LCD_RW BIT6
#define LCD_EN BIT7

static void lcd_command_nibble(unsigned char nibble)
{
    P2->OUT &= ~(LCD_RS | LCD_RW); // set RS/RW low
    P2->OUT &= ~LCD_EN; // set EN low

    P4->OUT &= 0xF0; // forcing bits clear
    P4->OUT |= (nibble & 0x0F); // lower 4 bits of command
    P2->OUT |= LCD_EN; // pulse enable
    _delay_cycles(1 * 3);
    P2->OUT &= ~LCD_EN;
}

static void lcd_command(unsigned char cmd)
{
    // upper 4 bits cmd
    lcd_command_nibble(cmd >> 4);

    // lower 4 bits cmd
    lcd_command_nibble(cmd);

    _delay_cycles(1000 * 3); // delay 1ms

    if (cmd < 4) {
        _delay_cycles(3000 * 3); // delay 3ms for commands 1 and 2
    }
}

void lcd_init(void)
{
    P4->DIR |= (BIT0 | BIT1 | BIT2 | BIT3); // set data ports to GPIO output
    P2->DIR |= (LCD_RS | LCD_RW | LCD_EN); // set pins to output E, R/W, and SEL to output

    _delay_cycles(40000 * 3); // wait 40ms
    lcd_command_nibble(0x3);
    _delay_cycles(10000 * 3); // wait 10ms
    lcd_command_nibble(0x3);
    _delay_cycles(1000 * 3); // wait 1ms
    lcd_command_nibble(0x3);
    _delay_cycles(1000 * 3); // wait 1ms
    lcd_command_nibble(0x2);
    _delay_cycles(1000 * 3); // wait 1ms

    lcd_command(0x28); // set data to 4-bit, 2 line, 5x7 font
    lcd_command(0x0F); // display on, cursor on, blink on
    lcd_command(0x16); // cursor shift right

    Clear_LCD();
    Home_LCD();
}

void Write_char_LCD(unsigned char data)
{
    P2->OUT |= LCD_RS;
    P2->OUT &= ~LCD_RW;
    P2->OUT &= ~LCD_EN;
    P2->OUT &= 0xF0; // forcing a clear of bottom 4 pins

    // upper 4 bits character
    P4->OUT &= 0xF0; // forcing bits clear
    P4->OUT |= ((data >> 4) & 0x0F);
    P2->OUT |= LCD_EN;
    _delay_cycles(40 * 3);
    P2->OUT &= ~LCD_EN;

    // lower 4 bits character
    P4->OUT &= 0xF0; // forcing bits clear
    P4->OUT |= (data & 0x0F);
    P2->OUT |= LCD_EN;
    _delay_cycles(40 * 3);
    P2->OUT &= ~LCD_EN;
}

void Write_string_LCD(char *p_str)
{
    while (*p_str) {
        Write_char_LCD(*p_str);
        delay_ms(50);
        p_str++;
    }
}

void lcd_position_set(uint8_t position)
{
    lcd_command(0x80 | position);
}

void lcd_backspace(void)
{
    lcd_command(14); // cursor shift left
    Write_string_LCD(" ");
    lcd_command(0x16); // cursor shift right
}

// clear the display
void Clear_LCD(void)
{
    lcd_command(0x01);
}

// move the cursor to the top left of the LCD
void Home_LCD(void)
{
    lcd_command(0x02);
}

void Write_number_LCD(int value)
{
    char string[20];
    int str_pos = 0;
    int digits = 0;
    int next_digit;

    for (str_pos = 0; str_pos < 20; str_pos++) {
        string[str_pos] = 0;
    }

    if (0 == value) {
        string[0] = 0x30;
    }
    else {
        str_pos = 0;
        while (1) {
            next_digit = value;
            digits = 0;
            while (next_digit > 10) {
                next_digit /= 10;
                digits++;
            }
            string[str_pos] = 0x30 + next_digit;
            value -= next_digit * (digits * 10);
            str_pos++;
            if (0 == digits) {
                break;
            }
        }
    }

    Write_string_LCD(string);
}
