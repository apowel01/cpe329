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
 #define PMOD_RED_ADDR_LOW 0x00
 #define PMOD_RED_ADDR_HIGH 0x00

 #define PMOD_BLUE_ADDR_LOW 0x00
 #define PMOD_BLUE_ADDR_HIGH 0x00

 #define PMOD_GREEN_ADDR_LOW 0x00
 #define PMOD_GREEN_ADDR_HIGH 0x00

 #define PMOD_CLEAR_ADDR_LOW 0x00
 #define PMOD_CLEAR_ADDR_HIGH 0x00


 void InitPmod(uint8_t Pmod_ADDRESS);
 void WritePmod(uint16_t MemAddress, uint8_t MemByte);
 uint16_t ReadPmod(uint16_t MemAddress);
 uint16_t TransmitFlag = 0;

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


        P2->DIR |= BIT2 | BIT1 | BIT0;             //Configure LED2
        P2->OUT &= ~(BIT2 | BIT1 | BIT0);

        __enable_irq();                            //enable global interrupts

        InitPmod(PMOD_ADDRESS);

 //       WritePmod(0x1122, 0x21);

        for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

        redvalues = ReadPmod(PMOD_RED_LOW);                // Read lower byte red values from Pmod
        redvalues =


        bluevalues = ReadPmod(PMOD_BLUE_LOW);                // Read blue values from Pmod


        greenvalues = ReadPmod(PMOD_GREEN_LOW);               // Read green values from Pmod


        clearvalues = ReadPmod(PMOD_CLEAR_LOW);               // Read clear values from Pmod

        P2->OUT |= (redvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
        for (i=4000; i > 0; i--)             //wait before changing LED

        P2->OUT |= (bluevalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
        for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

        P2->OUT |= (greenvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
        for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

        P2->OUT |= (clearvalues & (BIT2 | BIT1 | BIT0));  // Set LED2 With 3 LSB of Value
        for (i=4000; i > 0; i--)             //Delay for EEPROM write cycle(5ms)

        //__sleep();          // go to lower Power mode


        while(1) {

        }
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
