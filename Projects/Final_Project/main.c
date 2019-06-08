/*
 * main.c
 *
 */
#include "msp.h"
#include "timer_a.h"
#include "delay.h"
#include "LCD.h"
#include "pmod_color.h"
#include "speech.h"
#include "buttons.h"

void main(void) {
    pmod_colors_t colors;
    pmod_result_t results;
    uint8_t wait;
    char * color_name;

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz); // set DCO to 3MHz
//    timer_a_init(); // why are we doing this?
    // initialize the color sensor
    pmod_color_init();
    // initialize the speakjet text to speech module
    speech_init();
    // initialize the LCD
    lcd_init();
    // initialize buttons
    buttons_init();
    __enable_irq();


    P2->DIR |= BIT2 | BIT1 | BIT0;             //Configure Pins For output for LCD
    P2->OUT &= ~(BIT2 | BIT1 | BIT0);

    while(1) {
        wait = buttons_get_push(); // wait for a button to be pushed
                                   // either P1.1 or P1.4
        if (wait == 0)
        {
            // read colors from the pmod sensor
            pmod_color_read(&colors);
            // convert the colors to text
            color_name = pmod_color_to_name(&colors, &results);
            // send it to the speech module
            speech_say(&results);
            // delay so not too spammed
            delay_sec(2);
        }
        else
        {
            delay_ms(300);
            Clear_LCD();
        }
    }
}
