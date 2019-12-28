/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2017 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

// 1x32 Arduino header, 1x40 GPIO_0/JP1, 1x40 GPIO_1/JP7,
// 9 LEDs (8 User + HPS_LED), 4 switches, 3 buttons (2 User + HPS_KEY)
// 10 ADC pins
#define MRAA_DE_NANO_SOC_PINCOUNT 138

mraa_board_t *
        mraa_de_nano_soc();

#ifdef __cplusplus
}
#endif
