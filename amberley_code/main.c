#include "msp.h"
#include "delay.h"
#include "dac.h"
#include "uart.h"
#include "adc.h"
/**
 * main.c
 */

#define CALIBRATION_ZERO_TO_ONE_HALF_VOLT 4865 // 5140 **4900 works better *4850 works Perfectly from [0.04-0.37] (under 0.04 could use a boost) and effectively(=/- 1) out of acceptable range around .6
#define CALIBRATION_ONE_HALF_TO_ONE_VOLT 4950
#define CALIBRATION_ONE_TO_ONE_AND_HALF_VOLTS 4950
#define CALIBRATION_ONE_AND_HALF_TO_TWO_VOLTS 4970 // working 5095   4900 lower half doesn't like vals above 5k but upper half fails around 1.45
#define CALIBRATION_TWO_TO_THREE_VOLTS 4965 // working 5040
#define USE_ONE_HALF_TO_ONE_VOLT 2240
#define USE_ONE_TO_ONE_AND_HALF_VOLT 4990        //5184 *I think this original value was calibrated too high and that 4990 is a more accurate threshold
#define USE_ONE_AND_HALF_TO_TWO_VOLT 7690
#define USE_TWO_TO_THREE_VOLTS 10368     //10368

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
    else if (adc_value > USE_ONE_AND_HALF_TO_TWO_VOLT) {
        calibration = CALIBRATION_ONE_AND_HALF_TO_TWO_VOLTS;
    }
    else if (adc_value > USE_ONE_TO_ONE_AND_HALF_VOLT) {
        calibration = CALIBRATION_ONE_TO_ONE_AND_HALF_VOLTS;
    }
    else if (adc_value > USE_ONE_HALF_TO_ONE_VOLT) {
        calibration = CALIBRATION_ONE_HALF_TO_ONE_VOLT;
    }
    else {
        calibration = CALIBRATION_ZERO_TO_ONE_HALF_VOLT;
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
//        delay_sec(0);
        delay_ms(500);
    }
}
