#include "msp.h"
#include "delay.h"
#include "dac.h"
#include "uart.h"
#include "adc.h"
/**
 * main.c
 */

#define CALIBRATION_ZERO_TO_ONE_VOLT 5140
#define CALIBRATION_ONE_TO_TWO_VOLTS 5095 // working
#define CALIBRATION_TWO_TO_THREE_VOLTS 5040 // working
#define USE_ONE_TO_TWO_VOLTS 5184
#define USE_TWO_TO_THREE_VOLTS 10368

static void put_voltage(uint16_t adc_value)
{
    static uint16_t calibration = 5045;
    uint32_t volts;
    uint32_t tenths;
    uint32_t hundredths;
    uint32_t voltage_remainder = 0;
    if (adc_value > USE_TWO_TO_THREE_VOLTS) {
        calibration = CALIBRATION_TWO_TO_THREE_VOLTS;
    }
    else if (adc_value > USE_ONE_TO_TWO_VOLTS) {
        calibration = CALIBRATION_ONE_TO_TWO_VOLTS;
    }
    else {
        calibration = CALIBRATION_ZERO_TO_ONE_VOLT;
    }
    voltage_remainder = adc_value * 100;
    voltage_remainder /= calibration;
    volts = voltage_remainder / 100;
    voltage_remainder -= volts * 100;
    tenths = voltage_remainder / 10;
    hundredths = voltage_remainder - (tenths * 10);
    uart_put_num(volts);
    uart_put_char('.');
    uart_put_num(tenths);
    uart_put_num(hundredths);
    uart_put_str("V\r\n");
}

void main(void)
    {

    uint16_t new_value = 0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz);

    adc_init();
    uart_init();

    while(1) {
        ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion
        new_value = adc_get_value();
        put_voltage(new_value);
        delay_sec(2);
    }
}
