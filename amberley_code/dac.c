/*
 * dac.c
 *
 *  Created on: Apr 30, 2019
 *      Author: Amberley
 */

#include "msp.h"
#include "delay.h"

// #define TRIANGLE // use to select TRIANGLE or SQUARE functions for the IRQ handler

#define SQUARE_PERIOD 0xEB73 // set the period of the square wave to 20ms
#define TRIANGLE_PERIOD 617 // set the period of the triangle wave to 20ms

#define SQUARE_HI 3801  // 3*1267 = 3v
#define SQUARE_LO 1267  // 1 volt from the DC test
// triangle versions of the above defines are with the triangle IRQ handler

#define FULL_SMCLK 1 // *AMP CHECK THIS
#define DAC_CS BIT2 // DAC CS is P5.2
#define NVIC_int_const 31

void dac_init(void)
{
    // init SPI us
    P1->SEL0 |= BIT5 | BIT6; // 1.5,1.6,1.7 are SPI bus, but 1.7 is MISO which we do not use
    P1->SEL1 &= ~(BIT5 | BIT6);
    P5->DIR |= DAC_CS;  // CS is on 5.2

    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_SWRST; // Put eUSCI state machine in reset

    EUSCI_B0->CTLW0 = EUSCI_B_CTLW0_SWRST    |  // keep eUSCI in reset
            EUSCI_B_CTLW0_MST      |  // Set as SPI master
            EUSCI_B_CTLW0_SYNC     |  // Set as synchronous mode
            EUSCI_B_CTLW0_CKPL     |  // Set clock polarity high
            EUSCI_B_CTLW0_UCSSEL_2 |  // SMCLK
            EUSCI_B_CTLW0_MSB;        // MSB first

    EUSCI_B0->BRW = FULL_SMCLK;              // divide by 1 gives full SMCLK

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_SWRST;  // Initialize USCI state machine

}

#define send_hi 0x0F00
#define send_lo 0xFF
#define output_gain_sel 0x20 // select 1x Vref
#define out_en 0x10 // enable output


void dac_send(uint16_t data)
{
    uint8_t hi = (data & send_hi) >> 8;
    uint8_t lo = data & send_lo;

    hi |= output_gain_sel;  // 0x20 = OUTPUT_GAIN_SELECT = 1x Vref,
    hi |= out_en;  // ~SHDN = Ouput enabled ?? (i think - check data sheet)

    // Drive CS low to enable the DAC input
    P5->OUT &= ~DAC_CS;
    // Always wait for the TxBuf to be empty before writing a byte
//#if 0
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG) {
        // do nothing
    }
//#endif
    // write out the hi byte
    EUSCI_B0->TXBUF = hi;
    // Always wait for the TxBuf to be empty before writing a byte
//#if 0
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG) {
        // do nothing
    }
//#endif
    // write out the lo byte
    EUSCI_B0->TXBUF = lo;
    // wait for RxIFG which says Tx is finished, then push CS high
//#if 0
    while (!EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG) {
        // do nothing
    }
//#endif
    P5->OUT |= DAC_CS; // disable DAC

}

// can output square or traingle depending on IRQ configuration
void dac_timer_a_init_square(uint16_t up_count)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt
    TIMER_A0->CCTL[0] = TIMER_A_CCTLN_CCIE; // TACCR0 interrupt enale
    TIMER_A0->CCR[0] = up_count;  // CCR0 counter
    TIMER_A0->CTL = TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // UP count, SMCLK
    NVIC->ISER[0] = 1 << ((TA0_0_IRQn) & NVIC_int_const);  // NVIC interrupt
    __enable_irq();  // global enable
}

void dac_square_main(void)
{
    delay_set_dco(FREQ_12_0_MHz);
    dac_init();
    // dac_timer_a_init_square(SQUARE_PERIOD); // for square wave usage
    dac_timer_a_init_square(TRIANGLE_PERIOD);
    while(1) {

    }
}

#ifdef SQUARE
static uint8_t hi_lo_flag = 0;
static uint8_t timer_multiple = 0;
static uint8_t current_multiple = 0;
void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt

    if (0 == hi_lo_flag) { // if flag is low
        dac_send(SQUARE_HI); // send square output high
        if (current_multiple < timer_multiple) { // if counter is less than desired multiple of timer
            current_multiple++; // increment counter and repeat
        }
        else { // if counter is not less than timer multiple
            hi_lo_flag = 1; // set flag high
            current_multiple = 0; // reset counter to 0
        }
    }
    else { // if flag is high
        dac_send(SQUARE_LO); // send square wave low
        if (current_multiple < timer_multiple) { // repeat above logic for timer multiple
            current_multiple++;
        }
        else {
            hi_lo_flag = 1;
            current_multiple = 0;
        }
    }
}
#endif


#ifdef TRIANGLE // for the triangle waveform
#define TRIANGLE_HI 3779  // 3*1267 = 3v
#define TRIANGLE_LO 1262  // 1 volt from the DC test
#define TRIANGLE_INCREMENT 13 // increment value

static uint16_t triangle_current = 0; // current desired triangle value default
static uint8_t hi_lo_flag = 0;   // 0 is increment, 1 is decrement

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG;  // Clear the CCR0 interrupt
    if (0 == hi_lo_flag) { // 0 == format used to help prevent syntax errors when
        if (triangle_current < TRIANGLE_HI) { // compiling (i.e. if we forget an = it will throw an error)
            triangle_current += TRIANGLE_INCREMENT; // take current triangle value and increment
        }
        else {
            hi_lo_flag = 1; // change he flag to one to decrement
        }
    }
    else {
        if (triangle_current > TRIANGLE_LO) { // if value is still higher than desired low value
            triangle_current -= TRIANGLE_INCREMENT; // take current triangle value and decrement
        }
        else {
            hi_lo_flag = 0; // set the flag to zero to increment
        }
    }
    dac_send(triangle_current); // send the dac the value for current triangle output
}
#endif
