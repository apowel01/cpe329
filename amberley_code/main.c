#include "msp.h"
#include "delay.h"
#include "dac.h"
#include "uart.h"
/**
 * main.c
 */

void main(void)
{
    uint16_t inValue;
    delay_set_dco(FREQ_3_0_MHz);

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

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
}
