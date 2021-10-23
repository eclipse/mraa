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

#define MRAA_BANANA_PI_PINCOUNT 35
#define MRAA_BANANA_PRO_PINCOUNT 41

mraa_board_t* mraa_banana();

#ifdef __cplusplus
}
#endif
