/*
 * vt100.c
 *
 *  Created on: May 18, 2019
 *      Author: Amberley
 */

#include "msp.h"
#include "uart.h"
#include "vt100.h"

// frequency
#define FREQ_LINE 7
#define FREQ_TEXT_COL_START 5
#define FREQ_VALUE_COL_START (FREQ_TEXT_COL_START + 11)
#define FREQ_HZ_COL_START (FREQ_VALUE_COL_START + 4)
// mode header
#define MODE_LINE 3
#define MODE_TEXT_COL_START 5
// DC offset
#define DC_OFFSET_LINE 4
#define DC_OFFSET_TEXT_COL_START 5
#define DC_OFFSET_VALUE_COL_START (DC_OFFSET_TEXT_COL_START + 11)
#define DC_OFFSET_V_COL_START (DC_OFFSET_VALUE_COL_START+5)
// RMS voltage
#define RMS_LINE 5
#define RMS_TEXT_COL_START 5
#define RMS_VALUE_COL_START (RMS_TEXT_COL_START + 13)
#define RMS_V_COL_START (RMS_VALUE_COL_START+5)
// Vpp voltage
#define P2P_LINE 6
#define P2P_TEXT_COL_START 5
#define P2P_VALUE_COL_START (P2P_TEXT_COL_START + 5)
#define P2P_V_COL_START (P2P_VALUE_COL_START+5)
// outline box
#define BOX_TOP 1
#define BOX_BOTTOM 13
#define BOX_HOME 1
#define BOX_END 80
// bar graph axis and markers
#define AXIS_LINE 10
#define GRAPH_START 5
#define GRAPH_END (GRAPH_START+33)
#define SCALE_LINE (AXIS_LINE+1)
#define BAR_LINE (AXIS_LINE-1)

// VT-100 escape sequence strings
static char clear_entire_screen[] = {0x1B,'[','2','J',0};
static char set_80_columns[] = {0x1B,'[','?','3','l',0}; // set screen width to 80 columns

// output a number with two decimals accuracy to terminal
static void vt100_put_num_2_decimals(uint16_t value)
{
    uint32_t volts; // volts place
    uint32_t tenths; // tenths place
    uint32_t hundredths; // hundredths place

    volts = value / 100; // save remainder as uint32_t
    value -= volts * 100; // remove volts information
    tenths = value / 10; // save tenths place
    hundredths = value - (tenths * 10); // save hundredths place
    // print voltage readout to UART terminal
    uart_put_num(volts);
    uart_put_char('.');
    uart_put_num(tenths);
    uart_put_num(hundredths);
}

// update frequency to terminal
void vt100_put_frequency(uint32_t value)
{
    vt100_set_cursor_position(FREQ_LINE,FREQ_VALUE_COL_START);
    uart_put_str("    "); // clear value space
    vt100_set_cursor_position(FREQ_LINE,FREQ_VALUE_COL_START);
    uart_put_num(value);
}

// update DC offset to terminal
void vt100_put_dc_offset(uint32_t value)
{
    vt100_set_cursor_position(DC_OFFSET_LINE,DC_OFFSET_VALUE_COL_START);
    uart_put_str("    "); // clear value space
    vt100_set_cursor_position(DC_OFFSET_LINE,DC_OFFSET_VALUE_COL_START);
    vt100_put_num_2_decimals(value);
}

// update Vpp to terminal
void vt100_put_vpp(uint32_t value)
{
    vt100_set_cursor_position(P2P_LINE,P2P_VALUE_COL_START);
    uart_put_str("    "); // clear value space
    vt100_set_cursor_position(P2P_LINE,P2P_VALUE_COL_START);
    vt100_put_num_2_decimals(value);
}

// clear the entire terminal screen
void vt100_clear_screen(void)
{
    uart_put_str(clear_entire_screen);
}

// set terminal cursor to position line, column
void vt100_set_cursor_position(uint8_t line, uint8_t column)
{
    uart_put_char(0x1B);
    uart_put_char('[');
    uart_put_num(line);
    uart_put_char(';');
    uart_put_num(column);
    uart_put_char('H');
}

// draw outer box for terminal
void vt100_draw_box(void)
{
    int i;

    vt100_set_cursor_position(BOX_TOP,BOX_HOME+1);

    for(i=0; i < BOX_END-2; i++) {
        uart_put_char('_');
    }

    vt100_set_cursor_position(BOX_BOTTOM,BOX_HOME+1);

    for(i=0; i < BOX_END-2; i++) {
        uart_put_char('_');
    }

    for(i = BOX_TOP+1; i <= BOX_BOTTOM; i++) {
        vt100_set_cursor_position(i,BOX_HOME);
        uart_put_char('|');
        vt100_set_cursor_position(i,BOX_END);
        uart_put_char('|');
    }
}

// fill in bar graph with value
void vt100_put_bar(uint32_t value)
{
    int i;

    vt100_set_cursor_position(BAR_LINE,GRAPH_START);
    for(i=GRAPH_START; i < GRAPH_END; i++) {
        uart_put_char(' ');
    }

    vt100_set_cursor_position(BAR_LINE,GRAPH_START);
    for(i=GRAPH_START; i < GRAPH_START+(value/10); i++) {
        uart_put_char('#');
    }
}

// create bar graph scale
void vt100_bar_graph_scale(void)
{
    int i;

    vt100_set_cursor_position(AXIS_LINE,GRAPH_START);

    for(i=GRAPH_START; i < GRAPH_END; i++) {
        uart_put_char('_');
    }
    vt100_set_cursor_position(SCALE_LINE,GRAPH_START);
    uart_put_char('0');
    vt100_set_cursor_position(SCALE_LINE,GRAPH_START+10);
    uart_put_char('1');
    vt100_set_cursor_position(SCALE_LINE,GRAPH_START+20);
    uart_put_char('2');
    vt100_set_cursor_position(SCALE_LINE,GRAPH_START+30);
    uart_put_char('3');
}

// set up the terminal window for DMM using VT-100
void vt100_init(void)
{
    vt100_clear_screen(); // clears the screen
    uart_put_str(set_80_columns); // 80 column wide mode
    vt100_draw_box();
    // DMM_mode
    vt100_set_cursor_position(MODE_LINE,MODE_TEXT_COL_START);
    uart_put_str("DMM Mode: AC");

    // DC Offset
    vt100_set_cursor_position(DC_OFFSET_LINE,DC_OFFSET_TEXT_COL_START);
    uart_put_str("DC Offset:");
    vt100_set_cursor_position(DC_OFFSET_LINE,DC_OFFSET_V_COL_START);
    uart_put_str("V");

    //AC voltage True RMS
    vt100_set_cursor_position(RMS_LINE,RMS_TEXT_COL_START);
    uart_put_str("RMS Voltage:");
    vt100_set_cursor_position(RMS_LINE,RMS_V_COL_START);
    uart_put_str("VRMS");

    //AC voltage peak to peak value
    vt100_set_cursor_position(P2P_LINE,P2P_TEXT_COL_START);
    uart_put_str("VPP:");
    vt100_set_cursor_position(P2P_LINE,P2P_V_COL_START);
    uart_put_str("V");

    //frequency
    vt100_set_cursor_position(FREQ_LINE,FREQ_TEXT_COL_START);
    uart_put_str("Frequency:");
    vt100_set_cursor_position(FREQ_LINE,FREQ_HZ_COL_START);
    uart_put_str("Hz");

    vt100_bar_graph_scale();
}


