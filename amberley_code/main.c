#include "msp.h"
#include "delay.h"
#include "dac.h"
#include "uart.h"
#include "adc.h"
/**
 * main.c
 */

void main(void)
{
    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_48_0_MHz);

    // call adc.c main function
    adc_main();

}
