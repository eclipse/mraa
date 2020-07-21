/*
 * Author: Srinivas Kandagatla <srinivas.kandagatla@linaro.org>
 * Copyright (c) 2015 Linaro Limited.
 * Copyright (c) 2014 Intel Corporation.
 *
 * Copied from include/arm/banana.h
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_96BOARDS_LS_GPIO_COUNT 12
#define MRAA_96BOARDS_LS_I2C_COUNT  2
#define MRAA_96BOARDS_LS_SPI_COUNT  1
#define MRAA_96BOARDS_LS_UART_COUNT 2
#define MRAA_96BOARDS_LS_PIN_COUNT  40
#define MRAA_96BOARDS_LED_COUNT 6

mraa_board_t* mraa_96boards();

#ifdef __cplusplus
}
#endif
