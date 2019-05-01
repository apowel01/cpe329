/*
Lecture 4.2 (Wednesday, Week 4 4/24/2019)
Lecture Notes:
    SPI = Serial Peripheral Interface
    - full duplex: sends and recieve data simutaneously
    - syncrhonous: syncrhonized to some sort of clock signal
    - independent or daisy chained
    - low power
    - no speed limit

Both have SPI mode
    EUSCI_A
    EUSCI_B
    - 4 of each
        EUSCI_A0->EUSCI_A3
        EUSCI_B0->EUSCI_B3
    For pin reference, see MSP432 QuickStart Guide

RANDOM EXAMPLE
    EUSCI_B0 (P1.5,1.6,1.7)
        -SPI
        -3-wire
        -SMLCK
        -divide by 1
        -interrupts on recieve

    See TRM 25.3.1 for recommendation on initializing or
        reconfiguring the eUSCI module
        1. Set UCSWRST
        2. Initialize all eUSCI registers with UCSWRST = 1 (including UCxCTL1)
        3. Configure ports
            -Change the SEL0 and SEL1 since they aren't the default GPIO anymore
        4. Clear UCSWRST through software
        5. Enable interrputs (optional) with UCRXIE or UCTXIE

* we have 1 big 16-bit register for config
*/

// MSP432
void EUSCIB0_IRQHandler(void) {
    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG) { // check receive interrupt flag
        RX_DATA = EUSCI_B0->RXBUF;
        while (!(EUSCI_B0->IFG & EUSCI_B_IFG_TXBUF));   // wait for TXBUF
        EUSCI_B0->TXUF = RX_DATA;
    }
}

void main(void) {
    // start with software reset
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // put serial Peripheral into
                                            // reset state
    EUSCI_B0->CTLW0 = (EUSCI_B_CTLW0_MST    // Master mode
                     | EUSCI_B_CTLW0_SYNC   // synchronous
                     | EUSCI_B_CTLW0_UCSSEL_2 // SMCLK
                     | EUSCI_B_CTLW0_SWRST; // keep reset
    EUSCI_B0->BWR = 0x01; // divide by 1 for full SMCLK freq.
    P1->SEL0 |= (BIT5 | BIT6 | BIT7); // select EUSCI_B0
    P1->SEL1 &= ~(BIT5 |BIT6 | BIT7); // for pins
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;    // clear reset to activate peripheral

    // interrupts on receive
    EUSCI_B0->IE |= EUSCI_B_RXIE; // enable receive interrupts
    NVIC->ISER[0] = (1 << (EUSCIB0_IRQn & 31)); // enable EUSCI_B0 ISR
    __enable_irq(); // enable global interrupts'
    while (!(EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG)) { // wait for TXBUF to empty

    }
    EUSCI_B0->TXBUF = data; // data is 8-bit variable
    while (1) {

    }
}
