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

#define MRAA_RADXA_ROCK_3B_GPIO_COUNT 28
#define MRAA_RADXA_ROCK_3B_I2C_COUNT  3
#define MRAA_RADXA_ROCK_3B_SPI_COUNT  1
#define MRAA_RADXA_ROCK_3B_UART_COUNT 6
#define MRAA_RADXA_ROCK_3B_PWM_COUNT  11
#define MRAA_RADXA_ROCK_3B_AIO_COUNT  1
#define MRAA_RADXA_ROCK_3B_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ROCK_3B   "Radxa ROCK 3 Model B"

mraa_board_t *
        mraa_radxa_rock_3b();

#ifdef __cplusplus
}
#endif
