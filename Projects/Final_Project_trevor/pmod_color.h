/*
 * pmod_color.h
 *
 *  Created on: Jun 5, 2019
 *      Author: Amberley
 */

#ifndef PMOD_COLOR_H_
#define PMOD_COLOR_H_

typedef struct {
    uint16_t red;
    uint16_t green;
    uint16_t blue;
    uint16_t clear;
} pmod_colors_t;

void pmod_color_init(void);
void pmod_color_read(pmod_colors_t *p_colors);
char * pmod_color_to_name(pmod_colors_t *p_colors);

#endif /* PMOD_COLOR_H_ */
