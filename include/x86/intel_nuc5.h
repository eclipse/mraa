/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_INTEL_NUC5_PINCOUNT 18

mraa_board_t*
mraa_intel_nuc5();

#ifdef __cplusplus
}
#endif
