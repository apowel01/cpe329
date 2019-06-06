/*
 * I2C.h
 *
 *  Created on: Jun 3, 2019
 *      Author: Amberley
 */

#ifndef I2C_H_
#define I2C_H_


void i2c_init(uint8_t device_address);
void i2c_write_byte(uint16_t address, uint8_t value);
uint8_t i2c_read_byte(uint16_t address);



#endif /* I2C_H_ */
