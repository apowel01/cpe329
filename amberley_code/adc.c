/*
 * adc.c
 *
 *  Created on: May 10, 2019
 *      Author: Amberley
 */

// interrupt handler for the ADC
void ADC14_IRQHandler(void) {
    volatile uint16_t analogValue;
    analogValue = ADC14->MEM[2]; // read conversion value; reading auto clears interrupt flag
}

void adc_init(void) {
    // configure ADC14
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
    // enable interrupts
    ADC14->IER0 |= ADC14_IER0_IE_2; // enable interrupts on MEM2
    NVIC->ISER[0] = (1 << ADC14_IRQr & 31); //enable ADC ISR in NVIC
    __enable_irq(); // enable global interrupts
    // select A14 input function
    P6->SEL0 |= BIT1;
    P6->SEL1 |= BIT1;
}
void main(void)
{
    adc_init();

    while(1) {
        ADC14->CTL0 |= ADC14_CTL0_SC; // start a conversion
        delay_us(1000); // wait 1ms between conversions
    }
}
