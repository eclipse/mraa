/*
 * Author: Mihai Tudor Panu <mihai.tudor.panu@intel.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_IEI_TANK_PINCOUNT  9
#define MRAA_IEI_TANK_UARTCOUNT 6

mraa_board_t*
mraa_iei_tank();

#ifdef __cplusplus
}
#endif
