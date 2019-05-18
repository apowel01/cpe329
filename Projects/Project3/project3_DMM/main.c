#include "msp.h"


/**
 * main.c
 */
void main(void)
{
	WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;		// stop watchdog timer


CS -> KEY = CS_KEY_VAL;    // Unlock CS register
CS -> CTL1 |=  CS_CTL1_DIVHS_0    // set HSMCLK clock divider to 1
		| CS_CTL1_DIVS_1        // set SMCLK clock divider to 2
		| CS_CTL1_SELS_HFXTCLK;  // select HFXTCLK as clock source
CS -> CTL2 |= CS_CTL2_HFXTFREQ_6 ;  // set HFXTCLK clock source to 40 - 48 MHz
CS -> KEY = 0;   //  Locks CS Register

}
