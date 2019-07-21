/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_RASPBERRY_PI_B_REV_1_PINCOUNT 27
#define MRAA_RASPBERRY_PI_AB_REV_2_PINCOUNT 35
#define MRAA_RASPBERRY_PI2_B_REV_1_PINCOUNT 41
#define MRAA_RASPBERRY_PI_AB_PLUS_PINCOUNT 41
#define MRAA_RASPBERRY_PI_COMPUTE_MODULE_PINCOUNT 41
#define MRAA_RASPBERRY_PI_ZERO_PINCOUNT 41
#define MRAA_RASPBERRY_PI3_B_PINCOUNT 41
#define MRAA_RASPBERRY_PI_ZERO_W_PINCOUNT 41
#define MRAA_RASPBERRY_PI3_B_PLUS_PINCOUNT 41
#define MRAA_RASPBERRY_PI3_A_PLUS_PINCOUNT 41
#define MRAA_RASPBERRY_PI4_B_PINCOUNT 41

mraa_board_t *
        mraa_raspberry_pi();

#ifdef __cplusplus
}
#endif
