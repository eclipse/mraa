/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
#include "math.h"

#define MAX_BUFFER_LENGTH 6
#define HMC5883L_I2C_ADDR 0x1E

// configuration registers
#define HMC5883L_CONF_REG_A 0x00
#define HMC5883L_CONF_REG_B 0x01

// mode register
#define HMC5883L_MODE_REG 0x02

// data register
#define HMC5883L_X_MSB_REG 0
#define HMC5883L_X_LSB_REG 1
#define HMC5883L_Z_MSB_REG 2
#define HMC5883L_Z_LSB_REG 3
#define HMC5883L_Y_MSB_REG 4
#define HMC5883L_Y_LSB_REG 5
#define DATA_REG_SIZE 6

// status register
#define HMC5883L_STATUS_REG 0x09

// ID registers
#define HMC5883L_ID_A_REG 0x0A
#define HMC5883L_ID_B_REG 0x0B
#define HMC5883L_ID_C_REG 0x0C

#define HMC5883L_CONT_MODE 0x00
#define HMC5883L_DATA_REG 0x03

// scales
#define GA_0_88_REG 0x00 << 5
#define GA_1_3_REG 0x01 << 5
#define GA_1_9_REG 0x02 << 5
#define GA_2_5_REG 0x03 << 5
#define GA_4_0_REG 0x04 << 5
#define GA_4_7_REG 0x05 << 5
#define GA_5_6_REG 0x06 << 5
#define GA_8_1_REG 0x07 << 5

// digital resolutions
#define SCALE_0_73_MG 0.73
#define SCALE_0_92_MG 0.92
#define SCALE_1_22_MG 1.22
#define SCALE_1_52_MG 1.52
#define SCALE_2_27_MG 2.27
#define SCALE_2_56_MG 2.56
#define SCALE_3_03_MG 3.03
#define SCALE_4_35_MG 4.35

int
main(int argc, char** argv)
{
    mraa_init();
    float direction = 0;
    int16_t x = 0, y = 0, z = 0;
    uint8_t rx_tx_buf[MAX_BUFFER_LENGTH];

    //! [Interesting]
    mraa_i2c_context i2c;
    i2c = mraa_i2c_init(0);

    mraa_i2c_address(i2c, HMC5883L_I2C_ADDR);
    rx_tx_buf[0] = HMC5883L_CONF_REG_B;
    rx_tx_buf[1] = GA_1_3_REG;
    mraa_i2c_write(i2c, rx_tx_buf, 2);
    //! [Interesting]

    mraa_i2c_address(i2c, HMC5883L_I2C_ADDR);
    rx_tx_buf[0] = HMC5883L_MODE_REG;
    rx_tx_buf[1] = HMC5883L_CONT_MODE;
    mraa_i2c_write(i2c, rx_tx_buf, 2);

    for (;;) {
#if 0
        int i = 0;
        //alternative, equivalent method which helps to understand exactly what
        //the below does
        mraa_i2c_address(i2c, HMC5883L_I2C_ADDR);
        for (i = 0; i < DATA_REG_SIZE; i++) {
            mraa_i2c_read_byte_data(i2c, HMC5883L_DATA_REG+i);
        }
#endif
        // first 'select' the register we want to read from
        mraa_i2c_address(i2c, HMC5883L_I2C_ADDR);
        mraa_i2c_write_byte(i2c, HMC5883L_DATA_REG);

        // then we read from that register incrementing with every read the
        // chosen register
        mraa_i2c_address(i2c, HMC5883L_I2C_ADDR);
        // this call behaves very similarly to the Wire receive() call
        mraa_i2c_read(i2c, rx_tx_buf, DATA_REG_SIZE);

        x = (rx_tx_buf[HMC5883L_X_MSB_REG] << 8) | rx_tx_buf[HMC5883L_X_LSB_REG];
        z = (rx_tx_buf[HMC5883L_Z_MSB_REG] << 8) | rx_tx_buf[HMC5883L_Z_LSB_REG];
        y = (rx_tx_buf[HMC5883L_Y_MSB_REG] << 8) | rx_tx_buf[HMC5883L_Y_LSB_REG];

        // scale and calculate direction
        direction = atan2(y * SCALE_0_92_MG, x * SCALE_0_92_MG);

        // check if the signs are reversed
        if (direction < 0)
            direction += 2 * M_PI;

        printf("Compass scaled data x : %f, y : %f, z : %f\n", x * SCALE_0_92_MG, y * SCALE_0_92_MG,
               z * SCALE_0_92_MG);
        printf("Heading : %f\n", direction * 180 / M_PI);
    }
}
