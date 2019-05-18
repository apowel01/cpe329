/*
 * uart.h
 *
 *  Created on: May 7, 2019
 *      Author: Amberley
 */

#ifndef UART_H_
#define UART_H_

void uart_init(void);
void uart_put_char(char chr);
void uart_put_str(char * p_str);
uint16_t uart_get_value(void);
void uart_main(void);
void uart_put_num(uint32_t value);

#endif /* UART_H_ */
