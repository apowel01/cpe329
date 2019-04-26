// Running DAC Using SPI notes
// Author: Andrew Hunt


/*EUSCI_B0
 * SCLK - 1.5    CLK PIN
 * SIMO - 1.6     PIN
 * USE SMCLK Speed Can do up to 20MHz from data sheet, might work higher
 * hes running it at 12 MHz
 * msb first
 * MST MIDDLE
 * phase 0
 * polarity
 *
*/
#define =

main(void){
  uint16_t data;
  uint8_t loByte;
  uint8_t hiByte;


  set_DCO(12MHz); // set SMCLK to 12 MHz

  EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // put serial per into reset stack
  EUSCI_B0 -> CTLW0 = EUSCI_B_CTLW0_CKPL; // set clock inactive high for middle 1,1
  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_MSB; // SB first
  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_MST; // Master mode
  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SYNC; // SPI is synchronous
  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_UCSSEL_2; // Use SMCLK
  EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_SWRST; // Keep SPI reset state

  EUSCI_B0 -> BRW = 0x01; // run at full SMCLK Speed
  P1 -> SEL0 |= (BIT5|BIT6); // configure SCLK and SIMO
  P1 -> SEL1 &= ~(BIT5|BIT6); // for EUSCI_B0
  P1 -> DIR |= BIT0; // set P1.0 for CS
  EUSCI_B0 -> CTLW0 &= ~EUSCI_B_CTLW0_SWRST; // activate SPI

  P1 -> |= BIT0; // Initialize CS high (Prof reccommend make this a pound define)

  while (1){
    for (data = 0; data < 4096; data++){
      loByte = data & 0xFF; // mask lower 8 bits data
      hiByte = (data >> 8) & 0xFF; // mask upper 4 bits of data
      biByte |= (BIT4|BIT5); // set gain and shutdown bits to 1

      P1 -> OUT &= ~BIT0; // set CS low
      while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXBUF)); // wait for Tx to be empty ()*might have an issue with this line cause I dont know if I copied it correctly
      EUSCI_B0 -> TXBUF = hiByte;
      while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_TXBUF)); // wait for Tx to be empty ()*might have an issue with this line cause I dont know if I copied it correctly
      EUSCI_B0 -> TXBUF = loByte;

      while(!(EUSCI_B0 -> IFG & EUSCI_B_IFG_RXIFG)); // wait for transmit to finish
      P1 -> OUT |= BIT0; // set CS high
      delay_us(200); // wait before changing voltage
    }

  }

}
