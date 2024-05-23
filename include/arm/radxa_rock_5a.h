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

#define MRAA_RADXA_ROCK_5A_GPIO_COUNT 27
#define MRAA_RADXA_ROCK_5A_I2C_COUNT  5
#define MRAA_RADXA_ROCK_5A_SPI_COUNT  2
#define MRAA_RADXA_ROCK_5A_UART_COUNT 5
#define MRAA_RADXA_ROCK_5A_PWM_COUNT  9
#define MRAA_RADXA_ROCK_5A_AIO_COUNT  1
#define MRAA_RADXA_ROCK_5A_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ROCK_5A   "Radxa ROCK 5A"

mraa_board_t *
        mraa_radxa_rock_5a();

#ifdef __cplusplus
}
#endif
