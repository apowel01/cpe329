/*
 * LCD.h
 *
 *  Created on: Apr 12, 2019
 *      Author: Amberley
 */

#ifndef LCD_H_
#define LCD_H_

void Write_string_LCD(uint8_t line, uint8_t pos, char *p_str);
void lcd_init(void);
void Write_char_LCD(unsigned char data);
void Clear_LCD(void);
void Home_LCD(void);
void lcd_cursor_home(void);
void lcd_set_position(uint8_t pos);

#endif /* LCD_H_ */
