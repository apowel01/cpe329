#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include "lock.h"
#include "timer_a.h"
#include "exec_timing.h"
#include "dac.h"
#include "uart.h"
/**
 * main.c
 */

void main(void)
{
//    char key_str;
    delay_set_dco(FREQ_3_0_MHz);

	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer

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
//    timer_a_init();
//    lock_main();
//    exec_timing_main();
//    dac_main();

    uart_init();

    while(1) {
        uart_put_char('#');
    }
#endif
}
