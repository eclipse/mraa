/*
 * Author: Nascs <nascs@radxa.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_RADXA_ROCK_3C_GPIO_COUNT 27
#define MRAA_RADXA_ROCK_3C_I2C_COUNT  2
#define MRAA_RADXA_ROCK_3C_SPI_COUNT  1
#define MRAA_RADXA_ROCK_3C_UART_COUNT 5
#define MRAA_RADXA_ROCK_3C_PWM_COUNT  7
#define MRAA_RADXA_ROCK_3C_AIO_COUNT  0
#define MRAA_RADXA_ROCK_3C_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ROCK_3C   "Radxa ROCK3 Model C"

mraa_board_t *
        mraa_radxa_rock_3c();

#ifdef __cplusplus
}
#endif
