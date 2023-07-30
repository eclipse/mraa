/*
 * Author: Brian <brian@vamrs.com>
 * Author: Paul <paul.chang@mrshim.de>
 * Copyright (c) 2019 Vamrs Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

/*
 * Currently 2 product versions are supported: Rock Pi 4 and Rock Pi S
 */
#define ROCKPI_NUM_SUPPORTED_HW 2

/*
 * Defines for Rock Pi 4 boards.
 */
#define MRAA_ROCKPI4_INDEX      0
#define MRAA_ROCKPI4_I2C_COUNT  3
#define MRAA_ROCKPI4_SPI_COUNT  2
#define MRAA_ROCKPI4_UART_COUNT 2
#define MRAA_ROCKPI4_PWM_COUNT  2
#define MRAA_ROCKPI4_AIO_COUNT  1
#define MRAA_ROCKPI4_PIN_COUNT  40

/*
 * Defines for Rock Pi S boards (V11, V12, 13)
 * Since there hardware board versions cannot be distinguished
 * programatically,the lowest counts are be used.
 */
#define MRAA_ROCKPIS_INDEX      1
#define MRAA_ROCKPIS_I2C_COUNT  3
#define MRAA_ROCKPIS_SPI_COUNT  1
#define MRAA_ROCKPIS_UART_COUNT 2
#define MRAA_ROCKPIS_PWM_COUNT  2
#define MRAA_ROCKPIS_AIO_COUNT  1
#define MRAA_ROCKPIS_PIN_COUNT  52

mraa_board_t *
        mraa_rockpi4();

#ifdef __cplusplus
}
#endif
