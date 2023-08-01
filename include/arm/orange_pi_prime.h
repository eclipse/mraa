/*
 * Author: Gunjan <viraniac@gmail.com>
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_ORANGE_PI_PRIME_GPIO_COUNT 28
#define MRAA_ORANGE_PI_PRIME_I2C_COUNT  2
#define MRAA_ORANGE_PI_PRIME_SPI_COUNT  1
#define MRAA_ORANGE_PI_PRIME_UART_COUNT 1
#define MRAA_ORANGE_PI_PRIME_PIN_COUNT  40

mraa_board_t *
        mraa_orange_pi_prime();

#ifdef __cplusplus
}
#endif
