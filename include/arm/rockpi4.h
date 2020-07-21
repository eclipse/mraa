/*
 * Author: Brian <brian@vamrs.com>
 * Copyright (c) 2019 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_ROCKPI4_GPIO_COUNT 27
#define MRAA_ROCKPI4_I2C_COUNT  3
#define MRAA_ROCKPI4_SPI_COUNT  2
#define MRAA_ROCKPI4_UART_COUNT 2
#define MRAA_ROCKPI4_PWM_COUNT  2
#define MRAA_ROCKPI4_AIO_COUNT  1
#define MRAA_ROCKPI4_PIN_COUNT  40

mraa_board_t *
        mraa_rockpi4();

#ifdef __cplusplus
}
#endif
