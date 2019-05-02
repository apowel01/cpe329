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
    P1->SEL0 |= BIT5 | BIT6; // 1.5,1.6,1.7 are SPI bus, but 1.7 is MISO which we do not use
    P1->SEL1 &= ~(BIT5 | BIT6);
    P5->DIR |= BIT2;  // CS is on 5.2

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
            EUSCI_B_CTLW0_MST      |  // Set as SPI master
            EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
            EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
            EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
            EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->BRW = 0x01;              // divide by 1 gives full SMCLK

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Initialize USCI state machine

}

void dac_send(uint16_t data)
{
    uint8_t hi = (data & 0x0f00) >> 8;
    uint8_t lo = data & 0xff;

    hi |= 0x20;  // 0x20 = OUTPUT_GAIN_SELECT = 1x Vref,
    hi |= 0x10;  // ~SHDN = Ouput enabled ?? (i think - check data sheet)

    // Drive CS low to enable the DAC input
    P5->OUT &= ~BIT2;
    // Always wait for the TxBuf to be empty before writing a byte
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG) {
        // do nothing
    }
    // write out the hi byte
    EUSCI_B0->TXBUF = hi;
    // Always wait for the TxBuf to be empty before writing a byte
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG) {
        // do nothing
    }
    // write out the lo byte
    EUSCI_B0->TXBUF = lo;
    // wait for RxIFG which says Tx is finished, then push CS high
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG) {
        // do nothing
    }
    P5->OUT |= BIT2;

}

void dac_timer_a_init_square(uint16_t up_count)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enale
    TIMER_A0->CCR[0] = up_count;  // CCR0 counter
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC1; // UP count, SMCLK
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & 31);  // NVIC interrupt
    __enable_irq();  // global enable

}

static uint8_t timer_multiple = 0;
static uint8_t current_multiple = 0;
void dac_square_main(void)
{
    delay_set_dco(FREQ_12_0_MHz);
    dac_init();

#if 0
    // First test - output 1volt
    dac_send(1267);
#endif
    // second test - square wave 2v peak to peak with a 1v offset
    dac_timer_a_init_square(0xf800);
    while(1) {

    }
}

#define SQUARE_HI 3801  // 3*1267 = 3v
#define SQUARE_LO 1267  // 1 volt from the DC test
static uint8_t hi_lo_flag = 0;
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

    if (0 == hi_lo_flag) {
        dac_send(SQUARE_HI);
        if (current_multiple < timer_multiple) {
            current_multiple++;
        }
        else {
            hi_lo_flag = 1;
            current_multiple = 0;
        }
    }
    else {
        dac_send(SQUARE_LO);
        if (current_multiple < timer_multiple) {
            current_multiple++;
        }
        else {
            hi_lo_flag = 1;
            current_multiple = 0;
        }

    }
}
