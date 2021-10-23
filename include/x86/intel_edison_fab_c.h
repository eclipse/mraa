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

#define MRAA_INTEL_EDISON_PINCOUNT 25

mraa_board_t*
mraa_intel_edison_fab_c();

#ifdef __cplusplus
}
#endif
