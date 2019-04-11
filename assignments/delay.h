/*
 * delay.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */

#ifndef DELAY_H_
#define DELAY_H_

#define FREQ_1_5_MHz 100
#define FREQ_3_0_MHz 3
#define FREQ_6_0_MHz 6
#define FREQ_12_0_MHz 12
#define FREQ_24_0_MHz 24
#define FREQ_48_0_MHz 48

void set_DCO(int freq);
int get_DCO_MHz(void);
void delay_us(int us);

#endif /* DELAY_H_ */
