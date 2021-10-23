#ifndef _EXPORT_USERS_NECK_MRAA_FT4222_SRC_USB_FT4222_FTDI_FT4222_HPP
#define _EXPORT_USERS_NECK_MRAA_FT4222_SRC_USB_FT4222_FTDI_FT4222_HPP

/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {

#include <dlfcn.h>
#include "mraa_internal.h"

/**
 * Attempt to initialize a mraa_board_t for a UMFT4222EV module
 *
 * @param board Pointer to valid board structure.  If a mraa_board_t
 * is initialized, it will be added to board->sub_platform
 *
 * @return MRAA_SUCCESS if a valid subplaform has been initialized,
 * otherwise return MRAA_ERROR_PLATFORM_NOT_INITIALISED
 */
mraa_result_t mraa_usb_platform_extender(mraa_board_t* board);

}
#endif

#endif
