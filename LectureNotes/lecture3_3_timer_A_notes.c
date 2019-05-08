/* CPE 329-15
    Lecture 3.3 (Week 3, Friday)
    4/19/2019
    Example code for Timer_A
*/

static void CS_init(void)
{
    // setup SMCLK - 3 MHZ
    // SETUP ACLK - 32.678 KHz THIS NUMBER IS INCORRECT
}

static void GPIO_init(void)
{

}

void TA0_0_IRQHandler(void)
{
    P1->OUT ^= BIT0; // toggle LED0
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; //clear interupt flag
    TIMER_A0->CCTL[0] += 16339; // increment 0.5s on timer
}

void TA0_N_IRQHandler(void)
{
    if (TIMER_A0->CCTL[1] & TIMER_A_CCTLN_CCIFG) {
        TIMER_A0->CCTL[1] &= ~ TIMER_A_CCTLN_CCIFG; // clear interrupt flag
        P2->OUT ^= BIT2; // toggle blue LED1
    }
}


void main(void)
{
    // continuous mode is best for set intervals
    CCR[1] += 0xABCD; // this will overflow and give us the 15 bit shift

    CS_init(); //setup clocks
    GPIO_init(); // setup LED0 & LED1

    TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_1 | TIMER_A_CTL_MC_2; // setup timerA
                                            // to use ACLK in continuous mode
    TIMER_A0->CCR[0] = 16339; // 32.678 kHz from ACLK = 32678/2 Hz
    TIMER_A0->CCR[1] = 24508; // 0.75 * SMCLK
    // clear flags here, get code later //
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
    NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
    NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR
    __enable_irq(); // enable global interrupts
    while(){

    };
}
