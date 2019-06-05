/*
 * timer_a.c
 *
 *  Created on: Apr 23, 2019
 *      Author: Amberley
 */

#include "delay.h"
#include "msp.h"

#define PULSE_PERIOD 0xFFFF  // 20 ms : 0xEA60
#define PULSE_BASE 1475
#define PULSE_INCREMENT 305

uint16_t countflag = 0;
uint32_t p = 0;

void TA0_0_IRQHandler(void)
{
    TIMER_A0->CCTL[0] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    //P5->OUT |= BIT6;
    if (p <= 200) {
        ++p;
    }
    else if(p > 200){
        countflag = 1;
        p = 0;
        //P2->OUT ^= BIT1;
    }
    else {
        P2->OUT &= ~(BIT0|BIT1|BIT2);
    }

    if (countflag == 1) {
        countflag = 0;
        P2->OUT ^= (BIT2);
    }

}

void TA0_N_IRQHandler(void)
{
    TIMER_A0->CCTL[1] &= ~TIMER_A_CCTLN_CCIFG; // clear interrupt flag
    //P5->OUT &= ~BIT6;
}

// Setting PWM Signal Pulse Width
void timer_A_PWM_set(uint16_t degrees)
{
    TIMER_A0->CCR[1] = PULSE_BASE + (PULSE_INCREMENT * degrees);    // Set Pulse Width

}


// initalize Timer A - correct rate is clock freq / # samples req'd in a second.
void timer_a_init(void)
{
    P5->DIR |= BIT6;  // GPIO bits, 0 == 25KHz clock, 1 = IRQ execution time

      TIMER_A0->CTL |= TIMER_A_CTL_TASSEL_2 | TIMER_A_CTL_MC_1; // setup timerA
                                              // to use SMCLK in UP mode


      TIMER_A0->CCR[0] = PULSE_PERIOD;
      //set motor to 90 degrees
      //timer_A_PWM_set(9);
      TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_CCIE; // enable interrupts on timer A0
      TIMER_A0->CCTL[1] |= TIMER_A_CCTLN_CCIE;
      NVIC->ISER[0] = 1 << (TA0_0_IRQn & 31); // enable CCR0 ISR
      NVIC->ISER[0] = 1 << (TA0_N_IRQn & 31); // enable CCR1 ISR



#if 0
    TIMER_A0->CCTL[0] |= TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0
    TIMER_A0->CCTL[1] |=
            | TIMER_A_CCTLN_OUT
            | TIMER_A_CCTLN_OUTMOD_6; // enable interrupts on timer A0

#endif
}







//******************************************************************************
// CPE 329 - Assignment 9
// The MSP432 acts as the master and the EEPROM is a slave.
// The EEPROM uses 3 external connections A2 A1 A0 to set the lower 3 bits of
// its bus address. This creates a bus address of "1 0 1 0 A2 A1 A0". The code
// below assumes those three connections are all connected to VSS (Ground) and
// are logic 0. This gives the EEPROM a bus address of 0x50.
//
//
//                               /|\   /|\
//              MSP432P401      10k    10k     24LC256 EEPROM
//                master         |     |           Slave
//            -----------------  |     |     -----------------
//           |    P1.6/UCB0SDA|<-|----| ->  |SDA (5)         |
//           |                   |    |     |                |
//           |                   |    |     |                |
//           |    P1.7/UCB0SCL|<-|------>  |SCL (6)          |
//           |                   |         |                 |
//
// Paul Hummel
// Cal Poly
// May 2017 (created)
// Built with CCSv8.1
//******************************************************************************

 /*
 * main.c
 *
 *  Created on: May 31, 2019
 *      Author: Andy Hunt
 */
#include "msp.h"
#include "timer_a.h"
#include "delay.h"
#include "LCD.h"

#define PMOD_ADDRESS 0x29
#define PMOD_REDREG 0x00
#define PMOD_BLUEREG 0x00
#define PMOD_GREENREG 0x00
#define PMOD_CLEARREG 0x00

void InitPmod(uint8_t Pmod_ADDRESS);
void WritePmod(uint16_t MemAddress, uint8_t MemByte);
uint16_t ReadPmod(uint16_t MemAddress);
uint16_t TransmitFlag = 0;
//uint16_t countflag = 0;

void InitPmod(uint8_t Pmod_ADDRESS) {

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
 EUSCI_B0->I2CSA = Pmod_ADDRESS;        // Slave address
 EUSCI_B0->CTLW0 &= ~EUSCI_A_CTLW0_SWRST; // Release eUSCI from reset

 EUSCI_B0->IE |= EUSCI_A_IE_RXIE |       // Enable receive interrupt
                   EUSCI_A_IE_TXIE;

 }



void main(void) {

    WDT_A->CTL = WDT_A_CTL_PW | WDT_A_CTL_HOLD;     // stop watchdog timer

    delay_set_dco(FREQ_3_0_MHz); // set DCO to 3MHz
    timer_a_init();

    __enable_irq();

    //////

    uint32_t i;
    uint16_t redvalues;
    uint16_t bluevalues;
    uint16_t greenvalues;
    uint16_t clearvalues;
//        uint32_t p;


    P2->DIR |= BIT2 | BIT1 | BIT0;             //Configure LED2
    P2->OUT &= ~(BIT2 | BIT1 | BIT0);

    __enable_irq();                            //enable global interrupts

    InitPmod(PMOD_ADDRESS);

//       WritePmod(0x1122, 0x21);

//       for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

/*       redvalues = ReadPmod(PMOD_REDREG);                // Read red values from Pmod
    bluevalues = ReadPmod(PMOD_BLUEREG);                // Read blue values from Pmod
    greenvalues = ReadPmod(PMOD_GREENREG);               // Read green values from Pmod
    clearvalues = ReadPmod(PMOD_CLEARREG);               // Read clear values from Pmod

    P2->OUT |= (redvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
    for (i=4000; i > 0; i--)             //wait before changing LED

    P2->OUT |= (bluevalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
    for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

    P2->OUT |= (greenvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
    for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

    P2->OUT |= (clearvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
    for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

    //__sleep();          // go to lower Power mode
*/

    while(1) {
/*           if ((countflag == 1) & (p <= 1000)) {
            countflag = 0;
            ++p;
        }
        else if((countflag == 1) & (p > 1000)){
            countflag = 0;
            p = 0;
            P2->OUT ^= BIT2;
        }
        else {
            P2->OUT |= (BIT0|BIT1|BIT2);
        }
*/  }
}


    void WritePmod(uint16_t MemAddress, uint8_t MemByte) {


        uint8_t HiAddress;
        uint8_t LoAddress;

        HiAddress = MemAddress >> 8;
        LoAddress = MemAddress & 0xFF;

        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;    // Set transmit mode (write)
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

        while (!TransmitFlag);                  // Wait for Pmod address to transmit
            TransmitFlag = 0;
            EUSCI_B0 -> TXBUF = HiAddress;          // Send the high byte of the memory address

        while (!TransmitFlag);                   // Wait for the transmit to complete
            TransmitFlag = 0;
            EUSCI_B0 -> TXBUF = LoAddress;           // Send the low byte of the memory address

        while (!TransmitFlag);                   // Wait for the transmit to complete
            TransmitFlag = 0;
            EUSCI_B0 -> TXBUF = MemByte;             // Send the byte to store in Pmod

        while (!TransmitFlag);                   // Wait for the transmit to complete
            TransmitFlag = 0;
            EUSCI_B0 -> CTLW0 |= EUSCI_B_CTLW0_TXSTP; // I2C stop condition
        }


    uint16_t ReadPmod(uint16_t RGBAddress) {
        uint8_t ReceiveByte;
        uint8_t HiAddress;
        uint8_t LoAddress;

        HiAddress = RGBAddress >> 8;
        LoAddress = RGBAddress & 0xFF;

        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR; // Set transmit mode (write)
        EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

        while (!TransmitFlag); // Wait for Pmod address to transmit
            TransmitFlag = 0;

            EUSCI_B0 -> TXBUF = HiAddress; // Send the high byte of the memory address

        while (!TransmitFlag); // Wait for the transmit to complete
            TransmitFlag = 0;

            EUSCI_B0 -> TXBUF = LoAddress; // Send the low byte of the memory address

        while (!TransmitFlag); // Wait for the transmit to complete
            TransmitFlag = 0;

            EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR; // Set receive mode (read)
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

        // Wait for start to be transmitted
        while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

        // set stop bit to trigger after first byte
            EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

        while (!TransmitFlag); // Wait to receive a byte
            TransmitFlag = 0;

            ReceiveByte = EUSCI_B0->RXBUF; // Read byte from the buffer

        return ReceiveByte;
        }


    /*
    / I2C Interrupt Service Routine
    */
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
