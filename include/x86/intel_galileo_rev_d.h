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

#define MRAA_INTEL_GALILEO_REV_D_PINCOUNT 25

mraa_board_t*
mraa_intel_galileo_rev_d();

#ifdef __cplusplus
}
#endif
