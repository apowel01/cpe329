/*
 * delay.c
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"

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

void set_DCO(int freq)
{
    // change DC0 from default of 3MHz to 12MHz.
    CS->KEY = CS_KEY_VAL; // unlock CS registers
    CS->CTL0 = 0; // clear register

    switch(freq) {
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

int get_DCO_MHz(void)
{
    int MHz = 0;
    if (CS->CTL0 & CS_CTL0_DCORSEL_0){
        MHz = FREQ_1_5_MHz;
    }
    else if (CS->CTL0 & CS_CTL0_DCORSEL_1){
        MHz = FREQ_3_0_MHz;
    }
    else if (CS->CTL0 & CS_CTL0_DCORSEL_2){
        MHz = FREQ_6_0_MHz;
    }
    else if (CS->CTL0 & CS_CTL0_DCORSEL_3){
        MHz = FREQ_12_0_MHz;
    }
    else if (CS->CTL0 & CS_CTL0_DCORSEL_4){
        MHz = FREQ_24_0_MHz;
    }
    else if (CS->CTL0 & CS_CTL0_DCORSEL_5){
        MHz = FREQ_48_0_MHz;
    }

    return MHz;
}

// us will be provided an integer number of microseconds for delay
void delay_us(int us)
{
    int iterations;
    int frequency = get_DCO_MHz();
    int i;

    if (us > 50000){
        us = 50000;
    }

    if (frequency == FREQ_1_5_MHz){
        iterations = us * 3;
        iterations /= 2;
    }
    else{
        iterations = frequency * us;
    }

    for(i = 0; i < iterations; i++){

    }
}
