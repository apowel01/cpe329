#include "msp.h"
#include "LCD.h"
#include "keypad.h"
#include "delay.h"
#include "lock.h"
#include "timer_a.h"
#include "exec_timing.h"
#include "dac.h"
#include "func_gen.h"
#include "uart.h"
/**
 * main.c
 */

void main(void)
{
//    char key_str;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

//    timer_a_init();
//    lock_main();
//    exec_timing_main();
//    dac_main();
//    func_gen_main();
    uart_main();
}
