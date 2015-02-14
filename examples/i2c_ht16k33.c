/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Michael Ring    <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */

#include "mraa.h"

#define HT16K33_I2C_ADDR 0x70
#define HT16K33_SYS_SETUP_ON 0x21
#define HT16K33_DISP_SETUP_ON 0x81
#define HT16K33_DISP_BRIGHTNESS_1_16 0xE0

#define HT16K33_MEM_ROW0  0x00
#define HT16K33_MEM_ROW1  0x02
#define HT16K33_MEM_ROW2  0x04
#define HT16K33_MEM_ROW3  0x06
#define HT16K33_MEM_ROW4  0x08
#define HT16K33_MEM_ROW5  0x0A
#define HT16K33_MEM_ROW6  0x0C
#define HT16K33_MEM_ROW7  0x0E

int
main(int argc, char **argv)
{
//! [Interesting]

    mraa_i2c_context i2c;

    i2c = mraa_i2c_init(0);
    if (i2c == NULL) {
      printf("Could not initialize i2c Interface, check syslog for more details, exit...\n");
      exit(1);
    }

    int result = mraa_i2c_address(i2c,HT16K33_I2C_ADDR);
    if ( result != MRAA_SUCCESS) {
      printf("Could not set i2c device address, exit...\n");
      exit(1);
    }

    // Initialize Chip
    result = mraa_i2c_write_byte(i2c,HT16K33_SYS_SETUP_ON);
    if ( result != MRAA_SUCCESS) {
      printf("Could not turn chip on, exit...\n");
      exit(1);
    }

    result = mraa_i2c_write_byte(i2c,HT16K33_DISP_BRIGHTNESS_1_16);
    if ( result != MRAA_SUCCESS) {
      printf("Could not enable, exit...\n");
      exit(1);
    }

    result = mraa_i2c_write_byte(i2c,HT16K33_DISP_SETUP_ON);
    if ( result != MRAA_SUCCESS) {
      printf("Could not enable, exit...\n");
      exit(1);
    }

    // Send Data
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW0);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW1);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW2);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW3);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW4);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW5);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW6);
    mraa_i2c_write_byte_data(i2c, 0xff, HT16K33_MEM_ROW7);
      sleep(2);
    int i = 0;
    for (i=0; i<8; i++) {
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW0);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW1);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW2);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW3);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW4);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW5);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW6);
      mraa_i2c_write_byte_data(i2c, 1<<i, HT16K33_MEM_ROW7);
      sleep(1);
    }
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW0);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW1);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW2);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW3);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW4);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW5);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW6);
    mraa_i2c_write_byte_data(i2c, 0x00, HT16K33_MEM_ROW7);

    mraa_i2c_stop(i2c);
//! [Interesting]

}
