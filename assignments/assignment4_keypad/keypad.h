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


#endif /* KEYPAD_H_ */
