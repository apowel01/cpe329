/*
 * uart.c
 *
 *  Created on: May 7, 2019
 *      Author: Amberley
 */
#include "msp.h"
#if 0
typedef struct {
  __IO uint16_t CTLW0;                                                           /*!< eUSCI_Ax Control Word Register 0 */
  __IO uint16_t CTLW1;                                                           /*!< eUSCI_Ax Control Word Register 1 */
       uint16_t RESERVED0;
  __IO uint16_t BRW;                                                             /*!< eUSCI_Ax Baud Rate Control Word Register */
  __IO uint16_t MCTLW;                                                           /*!< eUSCI_Ax Modulation Control Word Register */
  __IO uint16_t STATW;                                                           /*!< eUSCI_Ax Status Register */
  __I  uint16_t RXBUF;                                                           /*!< eUSCI_Ax Receive Buffer Register */
  __IO uint16_t TXBUF;                                                           /*!< eUSCI_Ax Transmit Buffer Register */
  __IO uint16_t ABCTL;                                                           /*!< eUSCI_Ax Auto Baud Rate Control Register */
  __IO uint16_t IRCTL;                                                           /*!< eUSCI_Ax IrDA Control Word Register */
       uint16_t RESERVED1[3];
  __IO uint16_t IE;                                                              /*!< eUSCI_Ax Interrupt Enable Register */
  __IO uint16_t IFG;                                                             /*!< eUSCI_Ax Interrupt Flag Register */
  __I  uint16_t IV;                                                              /*!< eUSCI_Ax Interrupt Vector Register */
} EUSCI_A_Type;
#endif
// Initialize the UART
void uart_init(void)
{
    EUSCI_A0->CTLW0 |= EUSCI_A_CTLW0_SWRST; // put serial device into reset state
    // Set 8-bit no parity
    EUSCI_A0->CTLW0 = EUSCI_A_CTLW0_UCSSEL_2 // SMCLK
            | EUSCI_A_CTLW0_MODE_0 // UART mode
            | EUSCI_A_CTLW0_SWRST; // keep in reset
    EUSCI_A0->BRW = 0x01; // from baud rate calculation
    EUSCI_A0->MCTLW = (10 << EUSCI_A_MCTLW_BRF_OFS) //causing it to shift 4 bits as
                        |EUSCI_A_MCTLW_OS16;      //offest from mctlw register
    P1->SEL0 |= (BIT2|BIT3);   // Configure EUSCI_A0
    P1->SEL1 &= ~(BIT2|BIT3); //TX and Rx pins
    EUSCI_A0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Activate serial device
}

// output  single character - note, we do not do this in interrupt,
// we just keep sending the bytes.
void uart_put_char(char chr)
{
    // wait for the Tx buffer to be ready
    while(!(EUSCI_A0->IFG & EUSCI_A_IFG_TXIFG)) {
    }

    // buffer is free, write out data
    EUSCI_A0->TXBUF = chr;

}

// output a string
void uart_put_str(char * p_str)
{
    // early out for bad pointer
    if (0 == p_str) {
        return;
    }

    while (0 != *p_str) {
        uart_put_char(*p_str);

        p_str++;
    }
}

