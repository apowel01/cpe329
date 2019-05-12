/*
Lecture 6.2
Author: Amberley Powell
Date: 5/8/2019
ADC
*/

// pick a channel (lloking at launchpad pinout)
// we wanna use.... A14

// ADC on A14(P6.1)
// use #SMCLK of 48 MHz
// save memory location 2
// sample 128 clocks
// init uses ~ pg 860 from TRM

// PROJECT 3 NOTE: RUN AT 48 MHz

void ADC14_IRQHandler(void) {
    volatile uint16_t analogValue;
    analogValue = ADC14->MEM[2]; // read conversion value; reading auto clear interrupt flag
}


void main(void)
{
    ADC14->CTL0 &= ~ADC14_CTL0_ENC; // disable ADC for configuration
    ADC14->CTL0 = ADC14_CTL0_SHP // sample pulse mode, use internal sample timer
                | ADC14_CTL0_SSEL_5 // select HSMCLK
                | ADC14_CTL0_SHT0_6 // select 128 clocks for mem[2]
                | ADC14_CTL0_ON; // turn on ADC14
    ADC14->CTL1 = (2 << ADC14_CTL0_CSTARTADD_OFS) // start conversions using mem[2]
                                                  // shift 2 into bits 20-16
                  | ADC14_CTL1_RES_3; // use 14-bit conversion
    ADC14->MCTL[2] = ADC14_MCTLN_INCH_14; // read from channel 14(A14)
    ADC14->CTL0 |= ADC14_CTL0_END; // enable ADC
    ADC14->IER0 |= ADC14_IER0_IE_2; // enable interrupts on MEM2
    NVIC->ISER[0] = (1 << ADC14_IRQn & 31); //enable ADC ISR in NVIC
    __enable_irq(); // enable global interrupts
    P6->SEL0 |= BIT1; // select A14 input function
    P6->SEL1 |= BIT1;

    while(1) {
        ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion
        delay_us(1000); // wait 1ms between conversions

    }



}
