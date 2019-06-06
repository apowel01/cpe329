/*
 * pmod_color.c
 *
 *  Created on: Jun 5, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "pmod_color.h"

#define PMOD_DEVICE_ADDRESS 0x29

void pmod_color_init(void)
{
    i2c_init(PMOD_DEVICE_ADDRESS);
}

void pmod_color_read(pmod_colors_t *p_colors)
{
    uint8_t byte_value;
    // command: point to address 0, hold address pointer
    i2c_write_byte(0xa0);
    // write to address 0, power ON (0x01)
    i2c_write_byte(0x01);
    // delay for at least 2.4mS efore starting data collection
    delay_ms(3);
    // write to address 0, ENABLE ADC channels
    i2c_write_byte(0x02);
    // Send a STOP, we are done setting it up
    i2c_stop();
    // need 2.4mS for ADCs to prepare themselves, plus 2.4mS for the integration time
    delay_ms(5);
    // now send a command, address of first color values + auto-increment
    i2c_write_byte(0xB4);
    // Because we used auto-increment, we can
    // read all 8 bytes and put them in the color array
    byte_value = i2c_read_byte();
    p_colors->clear = (uint16_t)byte_value;
    byte_value = i2c_read_byte();
    p_colors->clear |= (uint16_t)byte_value << 8;
    byte_value = i2c_read_byte();
    p_colors->red = (uint16_t)byte_value;
    byte_value = i2c_read_byte();
    p_colors->red |= (uint16_t)byte_value << 8;
    byte_value = i2c_read_byte();
    p_colors->green = (uint16_t)byte_value;
    byte_value = i2c_read_byte();
    p_colors->green |= (uint16_t)byte_value << 8;
    byte_value = i2c_read_byte();
    p_colors->blue = (uint16_t)byte_value;
    byte_value = i2c_read_byte();
    p_colors->blue |= (uint16_t)byte_value << 8;
    // send a STOP to indicate we are done reading registers
    i2c_stop();
 }

char * pmod_color_to_name(pmod_colors_t *p_colors)
{
    char * p_this_color = "unknown";

    // something crude initially
    if ((p_colors->blue > p_colors->green) &&
         (p_colors->blue > p_colors->red) &&
         (p_colors->blue > p_colors->clear)) {
        p_this_color = "blue";
     }
    else if ((p_colors->red > p_colors->green) &&
             (p_colors->red > p_colors->blue) &&
             (p_colors->red > p_colors->clear)) {
        p_this_color = "red";
     }
    else if ((p_colors->green > p_colors->blue) &&
         (p_colors->green > p_colors->red) &&
         (p_colors->green > p_colors->clear)) {
        p_this_color = "green";
     }
    else if ((p_colors->clear > p_colors->green) &&
         (p_colors->clear > p_colors->red) &&
         (p_colors->clear > p_colors->blue)) {
        p_this_color = "clear";
     }

    return p_this_color;
}

