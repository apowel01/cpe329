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

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

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
    uint16_t inValue;
    dac_init();
    uart_init();

    while(1) {
        //while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG));
        //EUSCI_A0->TXBUF = 'A';
        inValue = uart_get_value();
        if (inValue < 4096) {
            dac_send(inValue);
        }
        else {
            uart_put_str("ERROR: number out of range");
        }
    }

#endif
}
