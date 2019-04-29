/*
CPE 329 - 15
DR. HUMMEL
lECTURE 5.1
AMBERLEY POWELL
*/

// why this way over ISR?
// this is parallel hardware, so it will happen at the same time

main (void) {
    set_ACLK(32.kHz);
    P2->SEL0 |= (BIT5 | BIT6); // select
    P2->SEL1 &= ~(BIT5 | BIT6); // timer A0
    P2->DIR |= (BIT5 | BIT6); // CCR2.3

    TIMER_A0->CCR[0] = 6553; // set 20ns period
    TIMER_A0->CCR[2] = 1638; // 25%
    TIMER_A0->CCR[3] = 4914; // 75%

    TIMER_A0->CCTL[2] = TIMER_A_CCTLN_OUTMOD_7; // set output
    TIMER_A0->CCTL[3] = TIMER_A_CCTLN_OUTMOD_7; // modes to set / reset

    TIMER_A0->CTL = TIMER_A_CTL_SSEL_1  | TIMER_AA_CTL_MC_1; // select ACLK
                                                             // count in up mode

    while (1) {

    }
}
