/*
 * vt100.c
 *
 *  Created on: May 18, 2019
 *      Author: Amberley
 */

#include "msp.h"
#include "uart.h"

#define FREQ_LINE 2
#define FREQ_TEXT_COL_START 5
#define FREQ_VALUE_COL_START (FREQ_TEXT_COL_START + 10)
#define FREQ_HZ_COL_START (FREQ_VALUE_COL_START + 4)

static char clear_entire_screen[] = {0x1B,'[','2','J',0};
static char set_80_columns[] = {0x1B,'[','?','3','l',0};

void vt100_put_frequency(uint32_t value)
{
    vt100_set_cursor_position(FREQ_LINE,FREQ_VALUE_COL_START);
    uart_put_num(value);
}

void vt100_clear_screen(void)
{
    uart_put_str(clear_entire_screen);
}

void vt100_set_cursor_position(uint8_t line, uint8_t column)
{
    uart_put_char(0x1B);
    uart_put_char('[');
    uart_put_num(line);
    uart_put_char(';');
    uart_put_num(column);
    uart_put_char('H');
}

void vt100_init(void)
{
    uart_put_str(set_80_columns);
    vt100_set_cursor_position(FREQ_LINE,FREQ_TEXT_COL_START);
    uart_put_str("frequency");
    vt100_set_cursor_position(FREQ_LINE,FREQ_HZ_COL_START);
    uart_put_str("Hz");
}


