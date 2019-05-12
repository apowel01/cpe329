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
    static uint16_t calibration_one_volt = 5184;
    uint16_t new_value = 0;
    uint32_t voltage_int = 0;
    uint32_t voltage_remainder = 0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz);


    adc_init();
    uart_init();

    while(1) {
        ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion
        new_value = adc_get_value();
        voltage_remainder = new_value * 100;
        voltage_remainder /= calibration_one_volt;
        voltage_int = voltage_remainder / 100;
        voltage_remainder -= voltage_int * 100;
        uart_put_num(voltage_int);
        uart_put_char('.');
        uart_put_num(voltage_remainder);
        uart_put_str("V\r\n");
        delay_sec(2);
    }
}
