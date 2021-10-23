/*
 * Author: Karena Anum Kamaruzaman <karena.anum.kamaruzaman@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// +1 as pins are "1 indexed"
#define MRAA_INTEL_CHERRYHILLS_PINCOUNT (5 + 1)

mraa_board_t*
mraa_intel_cherryhills();

#ifdef __cplusplus
}
#endif
