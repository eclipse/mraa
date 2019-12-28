/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include "mraa.h"

int
main(int argc, char** argv)
{
    mraa_init();
    mraa_add_subplatform(MRAA_GENERIC_FIRMATA, "/dev/ttyACM0");

    mraa_i2c_context i2c;
    i2c = mraa_i2c_init(0 + 512);
#if 0
    mraa_i2c_address(i2c, 0x62);

#if 1
    uint8_t rx_tx_buf[2];
    rx_tx_buf[0] = 0x0;
    rx_tx_buf[1] = 0x0;
    mraa_i2c_write(i2c, rx_tx_buf, 2);
#endif
    //mraa_i2c_write_byte_data(i2c, 0x0, 0x0);
    mraa_i2c_write_byte_data(i2c, 0x0, 0x1);

    mraa_i2c_write_byte_data(i2c, 0xFF, 0x08);
    mraa_i2c_write_byte_data(i2c, 0x00, 0x04);
    mraa_i2c_write_byte_data(i2c, 0xA0, 0x02);
#else
    mraa_i2c_address(i2c, 0x77);
    int res = mraa_i2c_read_byte_data(i2c, 0xd0);
    printf("res is 0x%x\n", res);

    uint8_t data[2];
    mraa_i2c_write_byte(i2c, 0x77);
    mraa_i2c_read(i2c, data, 1);

    res = mraa_i2c_read_word_data(i2c, 0xAA); // BMP085_CAL_AC1
    printf("res is %d\n", res);
#endif
    sleep(10);
}
