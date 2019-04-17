#include "msp.h"
#include "LCD.h"
#include "keypad.h"

/**
 * main.c
 */

void main(void)
{
    char key_str;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    lcd_init(); // initialize LCD display
    Clear_LCD();
    Home_LCD();

	keypadinit(); // initialize keypad

    while(1) {
        key_str = keypad_getkey();
        if (key_str != KEYPAD_NOTPRESSED) {
            Clear_LCD();
            Write_char_LCD(key_str);
        }
        _delay_cycles(25000);
    }
}
