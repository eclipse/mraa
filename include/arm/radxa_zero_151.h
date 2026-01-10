/*
 * Author: Sergey Ko <u306060@gmail.com>
 * Copyright (c) Radxa Limited.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_RADXA_ZERO_151_GPIO_COUNT 23
#define MRAA_RADXA_ZERO_151_I2C_COUNT  4
#define MRAA_RADXA_ZERO_151_SPI_COUNT  2
#define MRAA_RADXA_ZERO_151_UART_COUNT 3
#define MRAA_RADXA_ZERO_151_PWM_COUNT  2
#define MRAA_RADXA_ZERO_151_AIO_COUNT  0
#define MRAA_RADXA_ZERO_151_PIN_COUNT  40
#define PLATFORM_NAME_RADXA_ZERO   "Radxa Zero"

mraa_board_t *
    mraa_radxa_zero();

#ifdef __cplusplus
}
#endif
