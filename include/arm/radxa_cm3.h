/*
 * Author: Nascs <nascs@radxa.com>
 * Copyright (c) 2023 Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_RADXA_CM3_GPIO_COUNT 28
#define MRAA_RADXA_CM3_I2C_COUNT  3
#define MRAA_RADXA_CM3_SPI_COUNT  2
#define MRAA_RADXA_CM3_UART_COUNT 1
#define MRAA_RADXA_CM3_PWM_COUNT  9
#define MRAA_RADXA_CM3_AIO_COUNT  0
#define MRAA_RADXA_CM3_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_CM3_IO   "Radxa Compute Module 3(CM3) IO Board"
#define PLATFORM_NAME_RADXA_CM3_IO_2   "Radxa CM3 IO Board"
#define PLATFORM_NAME_RADXA_CM3_RPI_CM4_IO   "Radxa CM3 RPI CM4 IO"  // The core board of the Radxa CM3 is compatible with the RPI CM4 IO backplane.

mraa_board_t *
        mraa_radxa_cm3();

#ifdef __cplusplus
}
#endif
