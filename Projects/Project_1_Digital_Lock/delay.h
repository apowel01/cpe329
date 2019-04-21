/*
 * delay.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */

#ifndef DELAY_H_
#define DELAY_H_
#define FREQ_1_5_MHz 1500000
#define FREQ_3_MHz 3000000
#define FREQ_6_MHz 6000000
#define FREQ_12_MHz 12000000
#define FREQ_24_MHz 24000000
#define FREQ_48_MHz 48000000

void set_DCO(int freq);
int get_DCO_MHz(void);
void delay_us(int us, int freq);
void delay_ms(int us, int freq);

#endif /* DELAY_H_ */
