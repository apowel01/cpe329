/*
 * vt100.h
 *
 *  Created on: May 18, 2019
 *      Author: Amberley
 */

#ifndef VT100_H_
#define VT100_H_

void vt100_clear_screen(void);
void vt100_set_cursor_position(uint8_t line, uint8_t column);
void vt100_init(void);
void vt100_put_frequency(uint32_t value);
void vt100_put_dc_offset(uint32_t value);
void vt100_put_vpp(uint32_t value);



#endif /* VT100_H_ */
