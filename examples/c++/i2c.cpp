/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Outputs X,Y,Z co-ordinates and direction recursively using
 *                HMC5883L. Press Ctrl+C to exit.
 *
 */

/* standard headers */
#include <iostream>
#include <math.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa headers */
#include "mraa/common.hpp"
#include "mraa/i2c.hpp"

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

#define I2C_BUS 0

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        std::cout << "Exiting..." << std::endl;
        flag = 0;
    }
}

int
main(void)
{
    float direction;
    int16_t x, y, z;
    uint8_t rx_tx_buf[MAX_BUFFER_LENGTH];

    signal(SIGINT, sig_handler);

    //! [Interesting]
    mraa::I2c i2c(I2C_BUS);

    i2c.address(HMC5883L_I2C_ADDR);
    rx_tx_buf[0] = HMC5883L_CONF_REG_B;
    rx_tx_buf[1] = GA_1_3_REG;
    i2c.write(rx_tx_buf, 2);

    i2c.address(HMC5883L_I2C_ADDR);
    rx_tx_buf[0] = HMC5883L_MODE_REG;
    rx_tx_buf[1] = HMC5883L_CONT_MODE;

    while (flag) {
        i2c.address(HMC5883L_I2C_ADDR);
        i2c.writeByte(HMC5883L_DATA_REG);

        i2c.address(HMC5883L_I2C_ADDR);
        i2c.read(rx_tx_buf, DATA_REG_SIZE);

        x = (rx_tx_buf[HMC5883L_X_MSB_REG] << 8) | rx_tx_buf[HMC5883L_X_LSB_REG];
        z = (rx_tx_buf[HMC5883L_Z_MSB_REG] << 8) | rx_tx_buf[HMC5883L_Z_LSB_REG];
        y = (rx_tx_buf[HMC5883L_Y_MSB_REG] << 8) | rx_tx_buf[HMC5883L_Y_LSB_REG];

        // scale and calculate direction
        direction = atan2(y * SCALE_0_92_MG, x * SCALE_0_92_MG);

        // check if the signs are reversed
        if (direction < 0)
            direction += 2 * M_PI;

        std::cout << "Compass scaled data x : %f, y : %f, z : %f" << x * SCALE_0_92_MG
                  << y * SCALE_0_92_MG << z * SCALE_0_92_MG << std::endl;
        std::cout << "Heading : %f" << direction * 180 / M_PI << std::endl;

        sleep(1);
    }
    //! [Interesting]

    return EXIT_SUCCESS;
}
