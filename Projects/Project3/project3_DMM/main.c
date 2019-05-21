#include "msp.h"
#include "delay.h"
#include "uart.h"
#include "adc.h"
#include "timer_a.h"
#include "vt100.h"
#include "buttons.h"

/**
 * main.c
 */

// DMM AC or DC mode flag
static uint8_t mode_is_ac = 1;

void main(void)
{
    // AC and DC mode values
    uint32_t frequency = 0;
    uint32_t dc_offset = 0;
    uint32_t vpp = 0;
    uint32_t dc_volts = 0;
    uint32_t rms_volts = 0;
    uint8_t last_mode_was_ac = ~mode_is_ac;
        // make sure that last mode doesn't
        // equal current mode, this way
        // the vt-100 terminal will initiate

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    // set clock frequency
    delay_set_dco(FREQ_48_0_MHz);

    // init ADC, TIMER A, buttons (GPIO), VT-100 terminal and UART
    adc_init();
    uart_init();
    timer_a_init();
    buttons_init();

    // enable global interrupts
    __enable_irq();

    // continuously read then print values from ADC to UART terminal
    while(1) {
        mode_is_ac = buttons_get_mode_ac(); // check if current mode is AC (1) or DC (0)
        if (last_mode_was_ac != mode_is_ac) { // if last mode was NOT the current mode
            if (1 == mode_is_ac) { // re-initialize as appropriate for set mode
                vt100_init_ac();
            }
            else {
                vt100_init_dc();
            }
            last_mode_was_ac = mode_is_ac; // update last mode to this new mode
        }
        if (1 == mode_is_ac) { // if mode is AC, update AC values and print to terminal
            adc_get_values_ac(&frequency, &dc_offset, &vpp, &rms_volts); // update freq, DC and Vpp from ADC
            vt100_put_frequency(frequency); // display updated frequency
            vt100_put_dc_offset(dc_offset); // display updated DC offset
            vt100_put_vpp(vpp); // display updated Vpp
            vt100_put_bar(rms_volts); // update the bar in the bar graph with true-rms
            vt100_put_rms_volts(rms_volts); // update rms
        }
        else { // if mode is DC, update DC values and print to terminal
            adc_get_values_dc(&dc_volts); // update dc voltage
            vt100_put_dc_volts(dc_volts); // display new DC voltage
            vt100_put_bar(dc_volts); // update bar graph with new DC voltage
        }
    }
}
