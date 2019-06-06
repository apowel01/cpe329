/*
 * I2C.c
 *
 *  Created on: Jun 3, 2019
 *      Author: Amberley
 */

// referencing the digilent quick start guide for I2C
// psuedo code
#include "msp.h"

// ISR values

// initialize the i2c module for the given device address
void i2c_init_new(uint8_t device_address)
{
    P1->SEL0 |= BIT6 | BIT7;           // Set I2C pins of eUSCI_B0

    // Enable eUSCIB0 interrupt in NVIC module
    NVIC->ISER[0] = 1 << ((EUSCIB0_IRQn) & 31);

    // Configure USCI_B0 for I2C mode
    EUSCI_B0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // Software reset enabled
    EUSCI_B0->CTLW0 = EUSCI_A_CTLW0_SWRST | // Remain eUSCI in reset mode
            EUSCI_B_CTLW0_MODE_3 |          // I2C mode
            EUSCI_B_CTLW0_MST |             // Master mode
            EUSCI_B_CTLW0_SYNC |            // Sync mode
            EUSCI_B_CTLW0_SSEL__SMCLK;      // SMCLK

    EUSCI_B0->BRW = 30;                     // baudrate = SMCLK / 30 = 100kHz
    EUSCI_B0->I2CSA = device_address;        // Slave address
    EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Release eUSCI from reset

    EUSCI_B0->IE |= EUSCI_A_IE_RXIE |       // Enable receive interrupt
            EUSCI_A_IE_TXIE;
}
#if 0
#if 0
// I2C Interrupt Service Routine
void EUSCIB0_IRQHandler(void) {

    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0) {      // Check if transmit complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;    // Clear interrupt flag
        isr_ready = ISR_READY;                         // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0) {      // Check if receive complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;    // Clear interrupt flag
        isr_ready = ISR_READY;                         // Set global flag
    }
}
#else
void EUSCIB0_IRQHandler(void) {

    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0) {      // Check if transmit complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;    // Clear interrupt flag
        TransmitFlag = 1;                         // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0) {      // Check if receive complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;    // Clear interrupt flag
        TransmitFlag = 1;                         // Set global flag
    }

}
#endif
#endif


// write a byte to the i2c bus
void i2c_write_byte(uint8_t value)
{
    uint8_t HiAddress;
    uint8_t LoAddress;

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    i2c_wait_isr_ready();

    EUSCI_B0->TXBUF = value;      // Send the byte to store in EEPROM

    i2c_wait_isr_ready();

}

// set the stop condition
void i2c_stop(void)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

//read a byte from whatever the current address is
uint8_t i2c_read_byte(void)
{
    uint8_t value;

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR;   // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    i2c_wait_isr_ready();

    value = EUSCI_B0->RXBUF;    // Read byte from the buffer

    return value;
}


