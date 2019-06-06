/*
 * I2C.c
 *
 *  Created on: Jun 3, 2019
 *      Author: Amberley
 */

// referencing the digilent quick start guide for I2C
// psuedo code

#include "msp.h"
#include "I2C.h"


#define ISR_READY 1
#define ISR_NOT_READY 0
static uint8_t isr_ready = 0;

void i2c_init(uint8_t device_address)
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

static void i2c_wait_ready(void)
{
    while (ISR_NOT_READY == isr_ready);

    isr_ready = ISR_NOT_READY;
}

/*
        / I2C Interrupt Service Routine
 */
void EUSCIB0_IRQHandler(void)
{

    if (EUSCI_B0->IFG & EUSCI_B_IFG_TXIFG0) {      // Check if transmit complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_TXIFG0;    // Clear interrupt flag
        isr_ready = ISR_READY;                         // Set global flag
    }

    if (EUSCI_B0->IFG & EUSCI_B_IFG_RXIFG0) {      // Check if receive complete
        EUSCI_B0->IFG &= ~ EUSCI_B_IFG_RXIFG0;    // Clear interrupt flag
        isr_ready = ISR_READY;                         // Set global flag
    }

}


void i2c_start_tx(void)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;          // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT;       // I2C start condition

    i2c_wait_ready();
}
void i2c_start_rx(void)
{
    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR; // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)

    i2c_wait_ready();
}

// set the stop condition
void i2c_stop(void)
{
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;   // I2C stop condition
}

void i2c_write_bytes(uint8_t *p_values, uint8_t num_values)
{
    uint8_t i;

    i2c_start_tx();
    for (i=0; i < num_values; i++) {
        EUSCI_B0->TXBUF = p_values[i];      // Send the byte to store in EEPROM

        i2c_wait_ready();
    }

    i2c_stop();
}

void i2c_read_bytes(uint8_t *p_values, uint8_t num_values)
{
    uint8_t i;

    i2c_start_rx();
    for (i=0; i < num_values; i++) {
        if (i == num_values - 1) {
            i2c_stop();  // set stop for the last read
        }
        p_values[i] = EUSCI_B0->RXBUF;      // Send the byte to store in EEPROM

        i2c_wait_ready();
    }
}


#define COMMD_BIT 0x80

static uint8_t power_on[2] = {0x80, 0x01};
static uint8_t adc_on[2] = {0x80, 0x02};
static uint8_t read_base_address[2] = {0xB4, 0x00}; // we only use the first byte

uint8_t InitPmod(uint8_t Pmod_ADDRESS) {
    uint8_t ReceiveByte;
    uint8_t read_values[8];

    i2c_init(Pmod_ADDRESS);

    //Power on Color Sensor
    i2c_write_bytes(power_on, 2);
    // Wait for the device to power on - minimum 2.4mS
    delay_ms(3);

    //enalbe ADC to start the sample
    i2c_write_bytes(adc_on, 2);
    // Wait for at least 2.4mS for the ADCs to prepare themselves
    delay_ms(3);
    // wait for at least 2.4mS for the data to be collected
    delay_ms(3);
    // Set the register address to the first color byte with auto-increment
    i2c_write_bytes(read_base_address, 1);
    // read the 8 bytes of color values
    i2c_read_bytes(read_values, 8);
    return 1;
}

uint8_t InitPmod2(uint8_t Pmod_ADDRESS) {
    uint8_t ReceiveByte;
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

    //Power on Color Sensor
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;    // Set transmit mode (write)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

    i2c_wait_ready();                  // Wait for Pmod address to transmit

    EUSCI_B0 -> TXBUF = 0x29;          // Send Slave Address

    i2c_wait_ready();                  // Wait for Pmod address to transmit
    EUSCI_B0 -> TXBUF = (COMMD_BIT | 0x00);          // Send Enable Register Address

    i2c_wait_ready();                  // Wait for Pmod address to transmit
    EUSCI_B0 -> TXBUF = 0x0B;           //Send data byte to enable PON, RGBC ADC,
    // set stop bit
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    delay_us(3000); //wait for PON Start up
    //Check ID Register
     EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TR;    // Set transmit mode (write)
     EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition

     i2c_wait_ready();                  // Wait for Pmod address to transmit
     EUSCI_B0 -> TXBUF = 0x29;          // Send Slave Address

     i2c_wait_ready();                  // Wait for Pmod address to transmit
     EUSCI_B0 -> TXBUF = (COMMD_BIT | 0x13);          // Send ID Register Address

     i2c_wait_ready();                  // Wait for Pmod address to transmit

    EUSCI_B0->CTLW0 &= ~EUSCI_B_CTLW0_TR; // Set receive mode (read)
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTT; // I2C start condition (restart)
    EUSCI_B0 -> TXBUF &= ~COMMD_BIT;          // Send ID Register Address

    // Wait for start to be transmitted
    while ((EUSCI_B0->CTLW0 & EUSCI_B_CTLW0_TXSTT));

    // set stop bit to trigger after first byte
    EUSCI_B0->CTLW0 |= EUSCI_B_CTLW0_TXSTP;

    i2c_wait_ready();                  // Wait for Pmod address to transmit

    ReceiveByte = EUSCI_B0->RXBUF; // Read byte from the buffer

    return ReceiveByte;
}

