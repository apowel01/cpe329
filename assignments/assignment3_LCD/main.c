#include "msp.h"

/* Port listing between MSP432 and LCD screen
 *      MSP432                  LCD
 *  - P.0                   - PIN 14
 *  - P.2                   - PIN 13
 *  - P.4                   - PIN 12
 *  - P.????                - PIN 11
 *
 *  ~ ADD OTHERS LATER WHEN FINALIZED ~ *
 *
 * Required Functions:
 *  Clear_LCD(); // clear the display
 *  Clear_LCD(); Home_LCD(); // move the cursor to the top left of the LCD
 *  Clear_LCD(); Write_char_LCD(); // write a character to the LCD
 *
 */

/**
 * main.c
 */
void main(void)
{

    // Write a short program to initialize the LCD and write a single character to the display.
    





    // !!! what does this do ??
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer
}
