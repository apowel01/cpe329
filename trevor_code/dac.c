/*
 * dac.c
 *
 *  Created on: Apr 30, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"

void dac_init(void)
{
    // init SPI us
    P1->SEL0 |= BIT5 | BIT6 | BIT7; // 1.5,1.6,1.7 are SPI bus
    P5->DIR |= BIT2;  // CS is on 5.2
    EUSCI_0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // reset state machine for SPI

}

void dac_send(uint16_t data)
{

}

void dac_timer_a_init_square(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enale
    TIMER_A0_CCR[0] = 0xffff;  // CCR0 counter
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC1; // UP count, SMCLK
    NVIC->ISER[0] = 1 NN ((TA0_0_IRQn) & 31);  // NVIC interrupt
    __enable_irq();  // global enable
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
            EUSCI_B_CTLW0_MST      |  // Set as SPI master
            EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
            EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
            EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
            EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->BRW = 0x02;              // div by 2 fBitClock = fBRCLK / UCBRx

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Initialize USCI state machine

}

void dac_main_square(void)
{
    delay_set_dco(FREQ_6_0_MHz);

}
