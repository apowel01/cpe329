/* CPE 329-15
    Lecture  (Week 5, Friday)
    5/2/2019
    ADC on A14 (P6.1)
    using HSMCLK of 48 MHz
    saving in mem location 2
    Sample 128 CLKs
    Samples should be chosen based on input signal

*/



main(void){
  ADC14 -> CTL0 &= ~ADC14_CTL0_ENC; //disable ADC for configuration
  ADC14 -> CTL0 = ADC14_CTL0_ADC14_SHP // Sample pulse mode. use internal sample timer
                | ADC14_CTL0_SSEL_5  // Select HSMCLK
                | ADC14_CTL0_SHT0_6  // Select 128 clocks for mem[2]
                | ADC14_CTL0_ON     // turn on ADC14 (for power)

  ADC14 -> CTL1 = (2 << ADC14_CTL1_CSTARTADD_OFS) //start conversions using mem[2]
              // shifts 2 into bits 20-16 **he was unsure of this #define**
                | ADC14_CTL1_RES_3;  //use 14-bit conversion
  ADC14 -> MCTL[2] = ADC14_MTCLN_INCH_14; // read from channel 14 (A14)
  ADC14 -> CTL0 |= ADC14_CTL0_ENC; //enable ADC
  ADC14 -> IER0 |= ADC14_IER0_IE_2; // enable interrupts on MEM2
  NVIC -> iser[0] = (1<< ADC14_IRQr & 3); // ENABLE ADC ISR in NVIC
  __enable_irq(); //enable global interrupts
  P6 -> SEL0 |= BIT1; //select A14 input function
  P6 -> SEL1 |= BIT1;
  while(1){
    ADC14 -> CTL0 |= ADC14_CTL0_SC; //start a conversions
    delay)us(1000); //wait 1ms between conversions
  }
}

void ADC14_IRQHandler (void) {
  volative uint16_t analogValue;
  analogValue = ADC14 -> MEM[2]; // read conversion values
                                 // reading auto clears
                                 // inturrept flag)
}
