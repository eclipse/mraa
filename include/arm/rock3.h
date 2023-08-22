/*
 * Author: Jayson Reis <santosdosreis@gmail.com>
 * Copyright (c) 2023 Jayson Reis.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// https://wiki.radxa.com/Rock3/hardware/rock3a-gpio

#define MRAA_ROCK3_I2C_COUNT  3
#define MRAA_ROCK3_SPI_COUNT  1
#define MRAA_ROCK3_UART_COUNT 6
#define MRAA_ROCK3_PWM_COUNT  7
#define MRAA_ROCK3_AIO_COUNT  1
#define MRAA_ROCK3_PIN_COUNT  40

mraa_board_t *
        mraa_rock3();

#ifdef __cplusplus
}
#endif
