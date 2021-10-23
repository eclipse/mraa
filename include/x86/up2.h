/*
 * Author: Javier Arteaga <javier@emutex.com>
 * Based on work from: Dan O'Donovan <dan@emutex.com>
 *                     Nicola Lunghi <nicola.lunghi@emutex.com>
 * Copyright (c) 2017 Emutex Ltd.
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
#define MRAA_UP2_PINCOUNT    (40 + 1)

mraa_board_t*
mraa_up2_board();

#ifdef __cplusplus
}
#endif
