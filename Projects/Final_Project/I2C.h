/*
 * I2C.h
 *
 *  Created on: Jun 3, 2019
 *      Author: Amberley
 */

#ifndef I2C_H_
#define I2C_H_

void i2c_init(uint8_t device_address);
void i2c_write_bytes(uint8_t *p_values, uint8_t num_values);
void i2c_read_bytes(uint8_t *p_values, uint8_t num_values);


#endif /* I2C_H_ */
