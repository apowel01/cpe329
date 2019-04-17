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
void Clear_LCD(void);
void Home_LCD(void);

#endif /* LCD_H_ */
