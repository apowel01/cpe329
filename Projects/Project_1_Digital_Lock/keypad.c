/*
 * keypad.c
 *
 *  Created on: Apr 13, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "keypad.h"

#define ROW1 BIT4
#define ROW2 BIT5
#define ROW3 BIT6
#define ROW4 BIT7
#define COL1 BIT5
#define COL2 BIT6
#define COL3 BIT7

// initialize the MSP432 GPIO pins to appropriate rows and columns
void keypadinit (void)
{
    // already default set SEL0 AND SEL1 to GPIO.
    P3->DIR |= (COL1 | COL2 | COL3);  // set columns to outputs
    P6->DIR &= ~(ROW1 | ROW2 | ROW3 | ROW4); // set rows to inputs

    // enable pull-down resistor
    P6->REN |= (ROW1 | ROW2 | ROW3 | ROW4); // enable internal resistor
    P6->OUT &= ~(ROW1 | ROW2 | ROW3 | ROW4); // set resistors to pull down

    P3->OUT &= ~(COL1 | COL2 | COL3);
}

// read a button press
static uint8_t get_key (void)
{
    int rows;
    int col;
    uint8_t key;
    P3->OUT |= (COL1 | COL2 | COL3);
    _delay_cycles(25);
    rows = P6->IN & (ROW1 | ROW2 | ROW3 | ROW4);

    // check if its worth reading which button
    // by first checking IF pressed at all?
    if (rows == 0) {
        return KEYPAD_NOTPRESSED; // no button pressed
    }

    // iterate through COLumns to read through buttons
    for (col = 0; col < 3; col++) {
        P3->OUT &= ~(COL1 | COL2 | COL3); // clear columns
        P3->OUT |= (COL1 << col); //set 1 column high shifting each cycle
        _delay_cycles(25); // delay is fixed after compile
        rows = P6->IN & (ROW1 | ROW2 | ROW3 | ROW4);
        if (rows != 0) {
            break;  // if button was pressed
        }
    }

    if (col == 3) {  // no button press detected
        return KEYPAD_NOTPRESSED;
    }

    rows = rows >> 4; // move to low nibble
    if (rows == 4) {
        rows = 3; // binary decode
    }
    if (rows == 8) {
        rows = 4; // rows to values
    }
    key = rows * 3 - 2 + col;

    if (key == 11) {
        key = 0; // fix zero case
    }

    return key;
}

char keypad_getkey(void)
{
    char key_str;
    uint8_t key;
    key = get_key(); // was a key pressed?
    if (key <= 9) {
        key_str = 0x30 + key;
    }
    else if (key == 10) {
        key_str = '*';
    }
    else if (key == 12) {
        key_str = '#';
    }
    else {
        key_str = KEYPAD_NOTPRESSED;
    }
    return key_str;
}