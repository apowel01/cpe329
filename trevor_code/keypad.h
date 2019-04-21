/*
 * keypad.h
 *
 *  Created on: Apr 13, 2019
 *      Author: Amberley
 */

#ifndef KEYPAD_H_
#define KEYPAD_H_

#define KEYPAD_NOTPRESSED 0xFF

void keypadinit (void);
char keypad_getkey(void);
char keypad_get_ascii(void);
int keypad_get_number(uint8_t digits);

#endif /* KEYPAD_H_ */
