/*
 * delay.h
 *
 *  Created on: Apr 8, 2019
 *      Author: Amberley
 */

#ifndef DELAY_H_
#define DELAY_H_

#define FREQ_1_5_MHz 15
#define FREQ_3_0_MHz 30
#define FREQ_6_0_MHz 60
#define FREQ_12_0_MHz 120
#define FREQ_24_0_MHz 240
#define FREQ_48_0_MHz 480

void delay_us(int us);
void delay_ms(int ms);
void delay_sec(int sec);
void delay_test(void);
void delay_set_dco(int freq);
// uint32_t delay_get_dco_freq(void);

#endif /* DELAY_H_ */
