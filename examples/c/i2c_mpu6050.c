/*
 * Author: Manivannan Sadhasivam <manivannan.sadhasivam@linaro.org>
 * Copyright (c) 2018, Linaro Ltd.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Outputs Accelerometer and Gyroscope value from MPU6050 recursively.
 *                Press Ctrl+C to exit.
 *
 */

/* standard headers */
#include <endian.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/i2c.h"

#define I2C_BUS 0

/* register definitions */
#define MPU6050_ADDR 0x68
#define MPU6050_REG_PWR_MGMT_1 0x6b
#define MPU6050_REG_RAW_ACCEL_X 0x3b
#define MPU6050_REG_RAW_ACCEL_Y 0x3d
#define MPU6050_REG_RAW_ACCEL_Z 0x3f
#define MPU6050_REG_RAW_GYRO_X 0x43
#define MPU6050_REG_RAW_GYRO_Y 0x45
#define MPU6050_REG_RAW_GYRO_Z 0x47

/* bit definitions */
#define MPU6050_RESET 0x80
#define MPU6050_SLEEP (1 << 6)
#define MPU6050_PLL_GYRO_X (1 << 1)

/* accelerometer scale factor for (+/-)2g */
#define MPU6050_ACCEL_SCALE 16384.0

/* gyroscope scale factor for (+/-)250/s */
#define MPU6050_GYRO_SCALE 131.0

volatile sig_atomic_t flag = 1;

void
sig_handler(int signum)
{
    if (signum == SIGINT) {
        fprintf(stdout, "Exiting...\n");
        flag = 0;
    }
}

int16_t
i2c_read_word(mraa_i2c_context dev, uint8_t command)
{
    return be16toh(mraa_i2c_read_word_data(dev, command));
}

int
main(void)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_i2c_context i2c;
    uint8_t data;
    int16_t accel_data[3];
    int16_t gyro_data[3];
    int ret;

    /* install signal handler */
    signal(SIGINT, sig_handler);

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize I2C bus */
    i2c = mraa_i2c_init(I2C_BUS);
    if (i2c == NULL) {
        fprintf(stderr, "Failed to initialize I2C\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set slave address */
    status = mraa_i2c_address(i2c, MPU6050_ADDR);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* reset the sensor */
    status = mraa_i2c_write_byte_data(i2c, MPU6050_RESET, MPU6050_REG_PWR_MGMT_1);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    sleep(5);

    /* configure power management register */
    ret = mraa_i2c_read_byte_data(i2c, MPU6050_REG_PWR_MGMT_1);
    if (ret == -1) {
        return EXIT_FAILURE;
    }

    data = ret;
    data |= MPU6050_PLL_GYRO_X;
    data &= ~(MPU6050_SLEEP);

    status = mraa_i2c_write_byte_data(i2c, data, MPU6050_REG_PWR_MGMT_1);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    sleep(5);

    while (flag) {
        /* read raw accel data */
        accel_data[0] = i2c_read_word(i2c, MPU6050_REG_RAW_ACCEL_X) / MPU6050_ACCEL_SCALE;
        accel_data[1] = i2c_read_word(i2c, MPU6050_REG_RAW_ACCEL_Y) / MPU6050_ACCEL_SCALE;
        accel_data[2] = i2c_read_word(i2c, MPU6050_REG_RAW_ACCEL_Z) / MPU6050_ACCEL_SCALE;

        /* read raw gyro data */
        gyro_data[0] = i2c_read_word(i2c, MPU6050_REG_RAW_GYRO_X) / MPU6050_GYRO_SCALE;
        gyro_data[1] = i2c_read_word(i2c, MPU6050_REG_RAW_GYRO_Y) / MPU6050_GYRO_SCALE;
        gyro_data[2] = i2c_read_word(i2c, MPU6050_REG_RAW_GYRO_Z) / MPU6050_GYRO_SCALE;

        fprintf(stdout, "accel: x:%d y:%d z:%d\n", accel_data[0], accel_data[1], accel_data[2]);
        fprintf(stdout, "gyro: x:%d y:%d z:%d\n\n", gyro_data[0], gyro_data[1], gyro_data[2]);

        sleep(2);
    }

    /* stop i2c */
    mraa_i2c_stop(i2c);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* stop i2c */
    mraa_i2c_stop(i2c);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
