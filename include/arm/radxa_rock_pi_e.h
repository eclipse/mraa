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

#define MRAA_RADXA_ROCK_PI_E_GPIO_COUNT 28
#define MRAA_RADXA_ROCK_PI_E_I2C_COUNT  1
#define MRAA_RADXA_ROCK_PI_E_SPI_COUNT  1
#define MRAA_RADXA_ROCK_PI_E_UART_COUNT 2
#define MRAA_RADXA_ROCK_PI_E_PWM_COUNT  1
#define MRAA_RADXA_ROCK_PI_E_AIO_COUNT  1
#define MRAA_RADXA_ROCK_PI_E_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ROCK_PI_E   "Radxa ROCK Pi E"

mraa_board_t *
        mraa_radxa_rock_pi_e();

#ifdef __cplusplus
}
#endif
