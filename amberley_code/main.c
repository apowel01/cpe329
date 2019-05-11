#include "msp.h"
#include "delay.h"
#include "dac.h"
#include "uart.h"
/**
 * main.c
 */

void main(void)
{
    delay_set_dco(FREQ_3_0_MHz);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

}
