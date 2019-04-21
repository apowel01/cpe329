/*
 * delay.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"
#include "LCD.h"
#include "led.h"
#include "keypad.h"

static int dco_frequency = FREQ_3_0_MHz; // the board default value

/* Transition to VCORE Level 1: AM0_LDO --> AM1_LDO */
void set_Vcore_level_1(void)
{
    while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
        PCM->CTL0 = PCM_CTL0_KEY_VAL | PCM_CTL0_AMR_1;
    while ((PCM->CTL1 & PCM_CTL1_PMR_BUSY));
}

void set_flash_cntrl_wait_48_MHz(void)
{
    /* Configure Flash wait-state to 1 for both banks 0 & 1 */
    FLCTL->BANK0_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK0_RDCTL_WAIT_MASK)) | FLCTL_BANK0_RDCTL_WAIT_1;
    FLCTL->BANK1_RDCTL = (FLCTL->BANK0_RDCTL & ~(FLCTL_BANK1_RDCTL_WAIT_MASK)) | FLCTL_BANK1_RDCTL_WAIT_1;
}


// Set DCO frequency to desired value
void delay_set_dco(int freq)
{
    // change DC0 from default of 3MHz to 12MHz.
    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register

    dco_frequency = freq;
    switch(dco_frequency) {
    case FREQ_1_5_MHz:
        CS->CTL0 = CS_CTL0_DCORSEL_0; // set DCO = 1.5 MHz
        break;
    case FREQ_3_0_MHz:
        CS->CTL0 = CS_CTL0_DCORSEL_1; // set DCO = 3 MHz
        break;
    case FREQ_6_0_MHz:
        CS->CTL0 = CS_CTL0_DCORSEL_2; // set DCO = 6 MHz
        break;
    case FREQ_12_0_MHz:
        CS->CTL0 = CS_CTL0_DCORSEL_3; // set DCO = 12 MHz
        break;
    case FREQ_24_0_MHz:
        CS->CTL0 = CS_CTL0_DCORSEL_4; // set DCO = 24 MHz
        break;
    case FREQ_48_0_MHz:
        set_Vcore_level_1();
        set_flash_cntrl_wait_48_MHz();
        CS->CTL0 = CS_CTL0_DCORSEL_5; // set DCO = 48 MHz
        break;
    }

    // select clock sources
    CS->CTL1 = CS_CTL1_SELA_2 | CS_CTL1_SELS_3 | CS_CTL1_SELM_3;
    CS->KEY = 0; // lock the CS registers
}

void delay_us(int us)
{
 //   int count;

#if 0
    int iterations;
    if (dco_frequency == FREQ_1_5_MHz) {
        iterations = (us * 3) / 2;
        for (count = 0; count < iterations; count++) {
        }
    }
    else if (dco_frequency == FREQ_3_0_MHz) {
        iterations = (us * 3) - 60;
        for (count = 0; count < iterations; count++) {
        }
    }
    else {
        for (count = 0; count < us; count++)
        {
            switch (dco_frequency) {
            case FREQ_6_0_MHz:
 //               _delay_cycles(1);
                break;
            case FREQ_12_0_MHz:
                _delay_cycles(1);
                break;
            case FREQ_24_0_MHz:
                _delay_cycles(6);
                break;
            case FREQ_48_0_MHz:
                _delay_cycles(48);
                break;
            }
        }
    }
#else
    int j;
    if(dco_frequency == FREQ_1_5_MHz)
    {
      for(j = 0; j < us-57; j++);
    }
    else if(dco_frequency == FREQ_3_0_MHz)
    {
      for(j = 0; j < ((us *10)/79) -2; j++);
    }
    else if(dco_frequency == FREQ_6_0_MHz)
    {
      for(j = 0; j < us/10+6; j++);
    }
    else if(dco_frequency == FREQ_12_0_MHz)
    {
      for(j = 0; j < us/10 + 19; j++);
    }
    else if(dco_frequency == FREQ_24_0_MHz)
    {
      for(j = 0; j < us/10 + 45; j++);
    }
    else if(dco_frequency == FREQ_48_0_MHz)
    {
     for(j = 0; j < us/10 + 90; j++);
    }

//    count = ((us * dco_frequency) / 80);
//    while (count-- > 0) {
//    }
#endif
}

void delay_ms(int ms)
{
    int count;

    for (count = 0; count < ms; count++)
    {
        switch (dco_frequency) {
        case FREQ_1_5_MHz:
            _delay_cycles(1450);
            break;
        case FREQ_3_0_MHz:
            _delay_cycles(3000);
            break;
        case FREQ_6_0_MHz:
            _delay_cycles(6000);
            break;
        case FREQ_12_0_MHz:
            _delay_cycles(12000);
            break;
        case FREQ_24_0_MHz:
            _delay_cycles(24000);
            break;
        case FREQ_48_0_MHz:
            _delay_cycles(48000);
            break;
        }

    }
}

void delay_sec(int sec)
{
    int count;

    for (count = 0; count < sec; count++)
    {
        switch (dco_frequency) {
        case FREQ_1_5_MHz:
            _delay_cycles(1500000);
            break;
        case FREQ_3_0_MHz:
            _delay_cycles(3000000);
            break;
        case FREQ_6_0_MHz:
            _delay_cycles(6000000);
            break;
        case FREQ_12_0_MHz:
            _delay_cycles(12000000);
            break;
        case FREQ_24_0_MHz:
            _delay_cycles(24000000);
            break;
        case FREQ_48_0_MHz:
            _delay_cycles(48000000);
            break;
        }

    }
}


void delay_dco_freq_set(void)
{
    char key_str;
    Clear_LCD();
    Home_LCD();
    Write_string_LCD("set freq: ");
    key_str = keypad_get_number(1);
    switch (key_str) {
    case 1:
        delay_set_dco(FREQ_1_5_MHz);
        Write_string_LCD("1.5MHz");
        return;
    case 2:
        delay_set_dco(FREQ_3_0_MHz);
        Write_string_LCD("3MHz");
        return;
    case 3:
        delay_set_dco(FREQ_6_0_MHz);
        Write_string_LCD("6MHz");
        return;
    case 4:
        delay_set_dco(FREQ_12_0_MHz);
        Write_string_LCD("12MHz");
        return;
    case 5:
        delay_set_dco(FREQ_24_0_MHz);
        Write_string_LCD("24MHz");
        return;
    case 6:
        delay_set_dco(FREQ_48_0_MHz);
        Write_string_LCD("48MHz");
        return;
    }

}

uint8_t delay_interval_type_get()
{
    uint8_t int_type = 0;

    lcd_position_set(0x40); // put in second line
    Write_string_LCD("type=");
    int_type = keypad_get_number(1);
    delay_ms(10);
    switch (int_type) {
    case 1:
        Write_string_LCD("us");
        break;
    case 2:
        Write_string_LCD("ms");
        break;
    case 3:
        Write_string_LCD("sec");
        break;
    }

    return int_type;
}
uint8_t delay_interval_value_get()
{
    uint8_t int_value = 0;

    Write_string_LCD(", val=");
    int_value = keypad_get_number(2);
    delay_ms(10);
    Write_number_LCD(int_value);
    delay_sec(5);
    return int_value;
}
void delay_test(void)
{
    uint8_t int_type;
    uint8_t int_value;
    unsigned char str;
    while (1) {
        led_1_init();
        delay_dco_freq_set(); // handles getting/setting the speed
        int_type = delay_interval_type_get();
        int_value = delay_interval_value_get();

        Clear_LCD();
        Write_string_LCD("Test Running");
        lcd_position_set(0x40); // put in second line
        Write_string_LCD("*=next, #=quit");
        while (1){
            switch (int_type) {
            case 1:
                led_1_on();
                delay_us(int_value);
                led_1_off();
                delay_us(int_value);
                break;
             case 2:
                 led_1_on();
                 delay_ms(int_value);
                 led_1_off();
                 delay_ms(int_value);
                 break;
             case 3:
                 led_1_on();
                 delay_sec(int_value);
                 led_1_off();
                 delay_sec(int_value);
                 break;
             }
            str = keypad_get_ascii();
            if (str == '*') {
                break;
            }
            if (str == '#') {
                return;
            }
        }

    }
}
