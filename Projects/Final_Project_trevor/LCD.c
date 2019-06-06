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
#define LCD_DATA_0 BIT0
#define LCD_DATA_1 BIT1
#define LCD_DATA_2 BIT2
#define LCD_DATA_3 BIT4
#define LCD_DATA_REG P4

#define LCD_DATA_PINS (LCD_DATA_0 | LCD_DATA_1 | LCD_DATA_2 | LCD_DATA_3)

static void lcd_data_nibble_put(unsigned char nibble)
{
    LCD_DATA_REG->OUT &= ~LCD_DATA_PINS;
    if (nibble & 0x01) {
        LCD_DATA_REG->OUT |= LCD_DATA_0;
    }
    if (nibble & 0x02) {
        LCD_DATA_REG->OUT |= LCD_DATA_1;
    }
    if (nibble & 0x04) {
        LCD_DATA_REG->OUT |= LCD_DATA_2;
    }
    if (nibble & 0x08) {
        LCD_DATA_REG->OUT |= LCD_DATA_3;
    }
}

static void lcd_command_nibble(unsigned char nibble)
{
    P2->OUT &= ~(LCD_RS | LCD_RW); // set RS/RW low
    P2->OUT &= ~LCD_EN; // set EN low

    lcd_data_nibble_put(nibble & 0x0F);
    P2->OUT |= LCD_EN; // pulse enable
    _delay_cycles(100);
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
    LCD_DATA_REG->DIR |= (LCD_DATA_PINS); // set data ports to GPIO output
//    P5->DIR |= (BIT4 | BIT5 | BIT6 | BIT7); // set data ports to GPIO output

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
 //   P2->OUT &= 0xF0; // forcing a clear of bottom 4 pins

    // upper 4 bits character
    lcd_data_nibble_put(data >> 4);

    P2->OUT |= LCD_EN;
    _delay_cycles(40 * 3);
    P2->OUT &= ~LCD_EN;

    // lower 4 bits character
    lcd_data_nibble_put(data & 0x0f);
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
    lcd_command(12); // cursor shift left
    delay_ms(10);
    Write_string_LCD(" ");
    delay_ms(10);
    lcd_command(0x16); // cursor shift right
    delay_ms(10);
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

void Write_number_LCD(uint32_t value)
{
    int num_digits;
    char string[20];
    int str_pos = 0;
    // int digits = 0;
    int next_digit;
    int mult = 0;

    for (str_pos = 0; str_pos < 20; str_pos++) {
        string[str_pos] = 0;
    }

    if (0 == value) {
        string[0] = 0x30;
    }
    else {
        str_pos = 0;
        next_digit = value;
        num_digits = 0;
        while (next_digit > 0) {
            next_digit /= 10;
            num_digits++;
        }
        while (num_digits > 0) {
            next_digit = value;
            mult = 1;
            while (next_digit > 9) {
                next_digit /= 10;
                mult *= 10;
            }
            string[str_pos] = 0x30 + next_digit;
            value -= next_digit * mult;
            str_pos++;
            num_digits--;
        }
    }

    Write_string_LCD(string);
}
