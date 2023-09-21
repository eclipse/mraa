/*
 * Author: Daniel Bovensiepen <oss@bovi.li>
 * Author: Zhu Jia Xing <jiaxing.zhu@siemens.com>
 * Copyright (c) 2022 Siemens Ltd. China.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

#define MRAA_VISIONFIVE_PINCOUNT 41

mraa_board_t *
        mraa_visionfive();

#ifdef __cplusplus
}
#endif
