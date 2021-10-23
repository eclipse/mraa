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

#define MRAA_INTEL_DE3815_PINCOUNT 18

mraa_board_t*
mraa_intel_de3815();

#ifdef __cplusplus
}
#endif
