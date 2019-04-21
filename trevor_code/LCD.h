/*
 * LCD.h
 *
 *  Created on: Apr 12, 2019
 *      Author: Amberley
 */

#ifndef LCD_H_
#define LCD_H_

void Write_string_LCD(char *p_str);
void lcd_init(void);
void Write_char_LCD(unsigned char data);
void Write_number_LCD(int value);
void Clear_LCD(void);
void Home_LCD(void);
void lcd_backspace(void);
void lcd_position_set(uint8_t position);

#endif /* LCD_H_ */
