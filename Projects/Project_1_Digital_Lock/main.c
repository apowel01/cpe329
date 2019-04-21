#include "msp.h"
#include "keypad.h"
#include "LCD.h"
#include "delay.h"

/**
 * main.c
 */
void main(void)
{
    int count;
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

	keypadinit(); // initialize keypad
	lcd_init(); // initialize LCD screen
	Clear_LCD();
	Home_LCD();

	Write_string_LCD(0,0,"Andy rocks");
#if 1
	for (count = 0; count < 100; count++) {
	    delay_ms(50,3);
	}
	// lcd_set_position(0x40);
	Write_string_LCD(1,0,"& Sam rocks too");
	// lcd_set_position(0x05);
    Write_string_LCD(0,5,"Su");
	//Write_char_LCD('S');
#endif
}
