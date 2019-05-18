#include "msp.h"
#include "delay.h"
#include "uart.h"
#include "adc.h"
#include "timer_a.h"
#include "vt100.h"

/**
 * main.c
 */

// ADC define values for calibration
#define CALIBRATION_ZERO_TO_ONE_HALF_VOLT 4865
#define CALIBRATION_ONE_HALF_TO_ONE_VOLT 4950
#define CALIBRATION_ONE_TO_ONE_AND_HALF_VOLTS 4950
#define CALIBRATION_ONE_AND_HALF_TO_TWO_VOLTS 4970
#define CALIBRATION_TWO_TO_THREE_VOLTS 4965
// ADC define values for thresholds
#define USE_ONE_HALF_TO_ONE_VOLT 2240
#define USE_ONE_TO_ONE_AND_HALF_VOLT 4990
#define USE_ONE_AND_HALF_TO_TWO_VOLT 7690
#define USE_TWO_TO_THREE_VOLTS 10368

// read analog voltage as digital
static void put_voltage(uint16_t adc_value)
{
    static uint16_t calibration = 5045; // default calibration value
    uint32_t volts; // volts place
    uint32_t tenths; // tenths place
    uint32_t hundredths; // hundredths place
    uint32_t voltage_remainder = 0; // default remainder
    // determine which calibration to use based on voltage range
    if (adc_value > USE_TWO_TO_THREE_VOLTS) { // if value is greater than 2V
        calibration = CALIBRATION_TWO_TO_THREE_VOLTS;
    }
    else if (adc_value > USE_ONE_AND_HALF_TO_TWO_VOLT) { // greater than 1.5V
        calibration = CALIBRATION_ONE_AND_HALF_TO_TWO_VOLTS;
    }
    else if (adc_value > USE_ONE_TO_ONE_AND_HALF_VOLT) { // greater than 1V
        calibration = CALIBRATION_ONE_TO_ONE_AND_HALF_VOLTS;
    }
    else if (adc_value > USE_ONE_HALF_TO_ONE_VOLT) { // greater than 0.5V
        calibration = CALIBRATION_ONE_HALF_TO_ONE_VOLT;
    }
    else { // less than 0.5V
        calibration = CALIBRATION_ZERO_TO_ONE_HALF_VOLT;
    }
    voltage_remainder = adc_value * 100; // get remainder without
    voltage_remainder /= calibration;    // ...using any floats
    volts = voltage_remainder / 100; // save remainder as uint32_t
    voltage_remainder -= volts * 100; // remove volts information
    tenths = voltage_remainder / 10; // save tenths place
    hundredths = voltage_remainder - (tenths * 10); // save hundredths place
    // print voltage readout to UART terminal
    uart_put_num(volts);
    uart_put_char('.');
    uart_put_num(tenths);
    uart_put_num(hundredths);
    uart_put_str("V\r\n"); // print 'V' for units of volts and return new line
}

void main(void)
{
    uint16_t new_value = 0;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // set clock frequency
    delay_set_dco(FREQ_48_0_MHz);

    // init ADC and UART
    adc_init();
    uart_init();
    timer_a_init();
    vt100_init();

    // enable interrupts
    __enable_irq();

    // continuously read then print values from ADC to UART terminal
    while(1) {
        vt100_clear_screen();
        new_value = adc_get_frequency(); // get the new analog value
        vt100_set_cursor_position(10,10);
        put_voltage(new_value); // display value to terminal
        delay_ms(500); // delay for UART send time
    }
}
