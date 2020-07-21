/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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
// we have 20 useless pins then the 4 LEDS and the 2 LEDs on the module.
// There is another pin for the built-in button.
    #define MRAA_INTEL_JOULE_EXPANSION_PINCOUNT (40 * 2 + 23 + 1 + 2 + 1)

mraa_board_t*
mraa_joule_expansion_board();

#ifdef __cplusplus
}
#endif
