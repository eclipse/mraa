/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_INTEL_GALILEO_GEN_2_PINCOUNT 25

mraa_board_t*
mraa_intel_galileo_gen2();

#ifdef __cplusplus
}
#endif
