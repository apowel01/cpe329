// Interrupt lecture notes and code
// Author: Amberley Powell

// Right button and left button on launchpad
// left button P1.1 and right button P1.4

// detect each button press independently and do different things for each
// buttons are shorts to ground when pressed
// since both buttons are connected to port 1, both will run the same ISR

static void PORT1_IRQHandler(void)
{
    if (P1->IFG & BIT1) {
        // button 1 stuff
        P1->IFG &= ~BIT1; // clears button 1
    }
    if (P1->IFG & BIT4) {
        // button 4 stuff
        P1->IFG &= ~BIT4; // clear flag
    }
}

void main(void)
{
    // set up right and left buttons as interrupts
    P1->DIR &= ~(BIT1 | BIT4); // set as inputs
    P1->REN |= (BIT1 | BIT4); // enable resistors
    P1->OUT |= (BIT1 | BIT4); // set resistors to pull-up
    P1->IES |= (BIT1 | BIT4); // set interrupt on high -> low
    P1->IFG &= ~(BIT1 | BIT4); // clear interrupt flags
    P1->IE |= (BIT1 | BIT4); // enable interrupts GPIO
    /* pretty much ISER[1] only for GPIO,
    everything else will be ISER[0] - Dr. Hummel */
    NVIC->ISER[1] = 1 << (POR1_IRQn & 31); // enable GPIO interrupts in NVIC
    __enable_irq(); //enable global interrupts (ARM auto clears the flags)

    // do nothing loop
    while(1) {
    }
}
