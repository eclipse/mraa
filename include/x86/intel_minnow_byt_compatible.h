/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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
#define MRAA_INTEL_MINNOW_MAX_PINCOUNT (26 + 1)
#define MRAA_INTEL_MINNOW_TURBOT_PINCOUNT (27 + 1)

mraa_board_t*
mraa_intel_minnowboard_byt_compatible(mraa_boolean_t);

#ifdef __cplusplus
}
#endif
