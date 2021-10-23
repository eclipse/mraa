/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

/*gpio modes */
#define G_MODE_STRONG "mode_strong"
#define G_MODE_PULLUP "mode_pullup"
#define G_MODE_PULLDOWN "mode_pulldown"
#define G_MODE_HIZ "mode_hiz"
#define G_MODE_ACTIVE_LOW "mode_active_low"
#define G_MODE_OPEN_DRAIN "mode_open_drain"
#define G_MODE_OPEN_SOURCE "mode_open_source"

/*gpio direction modes */
#define G_DIR_OUT "out"
#define G_DIR_IN "in"
#define G_DIR_OUT_HIGH "out_high"
#define G_DIR_OUT_LOW "out_low"

/*gpio edge modes */
#define G_EDGE_NONE "edge_none"
#define G_EDGE_BOTH "edge_both"
#define G_EDGE_RISING "edge_rising"
#define G_EDGE_FALLING "edge_falling"

/*gpio input modes */
#define G_INPUT_ACTIVE_HIGH "input_high"
#define G_INPUT_ACTIVE_LOW "input_low"

/*gpio output driver modes */
#define G_OUTPUT_OPEN_DRAIN "output_open_drain"
#define G_OUTPUT_PUSH_PULL "output_push_pull"
/*---------------------------------------------*/

/* i2c modes */
#define I_MODE_STD "std"
#define I_MODE_FAST "fast"
#define I_MODE_HIGH "high"
/*---------------------------------------------*/

/* spi modes */
#define S_MODE_0 "mode0"
#define S_MODE_1 "mode1"
#define S_MODE_2 "mode2"
#define S_MODE_3 "mode3"
/*---------------------------------------------*/

/* uart parity types */
#define U_PARITY_NONE "N"
#define U_PARITY_EVEN "E"
#define U_PARITY_ODD "O"
#define U_PARITY_MARK "M"
#define U_PARITY_SPACE "S"
/*---------------------------------------------*/

#ifdef __cplusplus
}
#endif