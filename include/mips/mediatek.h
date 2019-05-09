/*
 * Author: Serge Vakulenko <vak@besm6.org>
 * Copyright (c) 2017 Serge Vakulenko.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

mraa_board_t *
        mraa_mtk_linkit();
mraa_board_t *
        mraa_mtk_omega2();

#ifdef __cplusplus
}
#endif
