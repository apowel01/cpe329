#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include "lock.h"

/**
 * main.c
 */

void main(void)
{
//    char key_str;

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

    lcd_init(); // initialize LCD display
    Clear_LCD();
    Home_LCD();
	keypadinit(); // initialize keypad

#if 0
	Write_string_LCD("Hello World");
    while(1) {
        key_str = keypad_get_ascii();
        if (key_str != KEYPAD_NOTPRESSED) {
            Clear_LCD();
            Write_char_LCD(key_str);
        }
        _delay_cycles(25000);
    }
#else
    lock_main();
#endif
}
