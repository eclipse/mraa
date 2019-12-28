/*
 * Author: Dan O'Donovan <dan@emutex.com>
 * Copyright (c) 2015 Emutex Ltd.
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// +1 as pins are "1 indexed"
#define MRAA_UP_PINCOUNT (40 + 1)
#define MRAA_UP_GPIOCOUNT (28)

mraa_board_t*
mraa_up_board();

#ifdef __cplusplus
}
#endif
