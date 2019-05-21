/*
 * adc.h
 *
 *  Created on: May 10, 2019
 *      Author: Amberley
 */

#ifndef ADC_H_
#define ADC_H_

uint16_t adc_get_value(void);
void adc_init(void);
uint32_t adc_get_samples_per_second(void);
void adc_sample_start(void);
void adc_get_values_ac(uint32_t *p_frequency, uint32_t *p_dc_offset, uint32_t *p_vpp, uint32_t *p_rms_volts);
void adc_get_values_dc(uint32_t *p_dc_volts);

#endif /* ADC_H_ */
