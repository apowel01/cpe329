/* CPE 329-15
    Lecture  (Week 5, Friday)
    5/2/2019
    Example code for UART
    Built for example hardware
    many different options

*/



EUSCI_A0 -> |= EUSCI_A_CTLW0_SWRST; // put serial device into reset state
EUSCI_A0 -> CTLW0 = EUSCI_A_CTLW0_PEN //Enable parity - odd
                  | EUSCI_A_CTLW0_UCSSEL_2 // SMCLK
                  | EUSCI_A_CTLW0_MODE_0 // UART mode
                  | EUSCI_A_CTLW0_SWRST; // keep in reset
EUSCI_A0 -> BRW = 0x01; // from baud rate calculation
EUSCI_A0 -> MCTLW = (10 << EUSCI_A_MCTL_BRF_) //causing it to shift 4 bits as
                    |USSCI_A_MCTLW_OS16;      //offest from mctlw register

P1 -> SEL0 |= (BIT2|BIT3);   // Configure EUSCI_A0
P1 -> SEL0 &= ~(BIT2|BIT3); //TX and Rx pins
EUSCI_A0 -> CLTW0 &= ~EUSCI_A_CTLW0_SWRST; // Activate serial device



// when sending data like characters need to send each char at a time
// reuse funcs from LCD display 
