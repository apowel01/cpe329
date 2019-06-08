/*
 * pmod_color.c
 *
 *  Created on: Jun 5, 2019
 *      Author: Amberley
 */
#include "msp.h"
#include "delay.h"
#include "pmod_color.h"
#include "I2C.h"

#define PMOD_DEVICE_ADDRESS 0x29
#define NUM_RED 2  // number of red shades
#define NUM_GREEN 2  // number of green shades
#define NUM_BLUE 2  // number of blue shades

char * color_names[NUM_RED][NUM_GREEN][NUM_BLUE];


void pmod_color_init(void)
{
    // initialize the array of names, we can improve on this later
    color_names[0][0][0] = "black";
    color_names[0][0][1] = "blue";
    color_names[0][1][0] = "green";
    color_names[0][1][1] = "blue green";
    color_names[1][0][0] = "red";
    color_names[1][0][1] = "red blue";
    color_names[1][1][0] = "red green";
    color_names[1][1][1] = "red green blue";
    }

static uint8_t power_on[2] = {0xA0, 0x01};
static uint8_t adc_on[2] = {0xA0, 0x03};
static uint8_t read_base_address[2] = {0xB4, 0x00}; // we only use the first byte

void pmod_color_read(pmod_colors_t *p_colors)
{
    uint8_t read_values[8];
    uint8_t read_registers[32];

    // We do this every time we sample for 2 reasons:
    // 1 - it sets the device address, so we can share the us with others
    // 2 - in debug I found that the us could get into an error state, needing
    //     a power reset to recover.  Doing a soft reset might help with that.
    i2c_init(PMOD_DEVICE_ADDRESS);

    //Power on Color Sensor
    i2c_write_bytes(power_on, 2);
   // delay for at least 2.4mS before starting data collection
    delay_ms(3);

    // write to address 0, ENABLE ADC channels
    i2c_write_bytes(adc_on, 2);
    // need 2.4mS for ADCs to prepare themselves
    delay_ms(3);
    // wait for at least 2.4mS for the data to be collected
    delay_ms(3);

    // for debug, read all the registers in the chip
//    i2c_write_bytes(0, 1); // use this for debug, it pulls the command registers
//    i2c_read_bytes(read_registers, 28);

    // Set the register address to the first color byte with auto-increment
    i2c_write_bytes(read_base_address, 1);
    // read the 8 bytes of color values
    i2c_read_bytes(read_values, 8);

    // convert read values to the color array
    p_colors->clear = (uint16_t)read_values[0];
    p_colors->clear |= (uint16_t)read_values[1] << 8;
    p_colors->red = (uint16_t)read_values[2];
    p_colors->red |= (uint16_t)read_values[3] << 8;
    p_colors->green = (uint16_t)read_values[4];
    p_colors->green |= (uint16_t)read_values[5] << 8;
    p_colors->blue = (uint16_t)read_values[6];
    p_colors->blue |= (uint16_t)read_values[7] << 8;
}


char * pmod_color_to_name(pmod_colors_t *p_colors, pmod_result_t *p_result)
{
    uint32_t threshold;
    uint32_t blue = (uint32_t)p_colors->blue;
    uint32_t green = (uint32_t)p_colors->green;
    uint32_t red = (uint32_t)p_colors->red;
    uint32_t clear = (uint32_t)p_colors->clear;

    threshold = (clear*30)/100;
    // Pick an aritrary level, below which we
    if (threshold < 200) {
        threshold = 200;
    }
    else if (clear > 1000){
        threshold = 400;
    }

    p_result->blue = (blue > threshold) ? 1 : 0;
    p_result->red = (red > threshold) ? 1 : 0;
    p_result->green = (green > threshold) ? 1 : 0;

   if ((p_result->blue + p_result->red + p_result->green) > 1){
       p_result->blue = 0;
       p_result->red = 0;
       p_result->green = 0;
   }
    return color_names[p_result->red][p_result->green][p_result->blue];
}

