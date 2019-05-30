/*
 * keypad.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Amberley
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define KEYPAD_NOTPRESSED 0xFF
#define KEYPAD_STAR (-6)
#define KEYPAD_HASH (-13)

void keypadinit(void);
char keypad_getkey(void);
char keypad_get_ascii(void);
int keypad_get_number(uint8_t digits);
int keypad_get_digit(void);
int timer_A_PWM_set(uint16_t degrees);

#endif /* KEYPAD_H_ */
