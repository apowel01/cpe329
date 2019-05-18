/*
 * vt100.c
 *
 *  Created on: May 18, 2019
 *      Author: Amberley
 */

#include "msp.h"
#include "uart.h"

static char clear_entire_screen[] = {0x1B,'[','2','J',0};
static char set_cursor_position[] = {0x1B,'[',0,';',0,'H',0};
static char set_80_columns[] = {0x1B,'[','?','3','l',0};

void vt100_clear_screen(void)
{
    uart_put_str(clear_entire_screen);
}

void vt100_set_cursor_position(uint8_t line, uint8_t column)
{
    set_cursor_position[2] = line;
    set_cursor_position[4] = column;
    uart_put_str(set_cursor_position);
}

void vt100_init(void)
{
    uart_put_str(set_80_columns);
}


