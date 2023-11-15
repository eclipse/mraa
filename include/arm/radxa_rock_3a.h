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

#define MRAA_RADXA_ROCK_3A_GPIO_COUNT 28
#define MRAA_RADXA_ROCK_3A_I2C_COUNT  3
#define MRAA_RADXA_ROCK_3A_SPI_COUNT  1
#define MRAA_RADXA_ROCK_3A_UART_COUNT 7
#define MRAA_RADXA_ROCK_3A_PWM_COUNT  11
#define MRAA_RADXA_ROCK_3A_AIO_COUNT  1
#define MRAA_RADXA_ROCK_3A_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ROCK_3A   "Radxa ROCK3 Model A"

mraa_board_t *
    mraa_radxa_rock_3a();

#ifdef __cplusplus
}
#endif
