/*
 * timer_a.h
 *
 *  Created on: Apr 23, 2019
 *      Author: trevo
 */

#ifndef TIMER_A_H_
#define TIMER_A_H_

void timer_a_init(void);
void TA0_0_IRQHandler(void);
void TA0_N_IRQHandler(void);
void timer_A_PWM_set(uint16_t degrees);

#endif /* TIMER_A_H_ */
