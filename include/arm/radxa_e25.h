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

#define MRAA_RADXA_E25_GPIO_COUNT 17
#define MRAA_RADXA_E25_I2C_COUNT  1
#define MRAA_RADXA_E25_SPI_COUNT  2
#define MRAA_RADXA_E25_UART_COUNT 3
#define MRAA_RADXA_E25_PWM_COUNT  6
#define MRAA_RADXA_E25_AIO_COUNT  1
#define MRAA_RADXA_E25_PIN_COUNT  26
#define PLATFORM_NAME_RADXA_E25   "Radxa E25 Carrier Board"

mraa_board_t *
        mraa_radxa_e25();

#ifdef __cplusplus
}
#endif
