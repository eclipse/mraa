#ifndef _EXPORT_USERS_NECK_MRAA_FT4222_SRC_USB_FT4222_FTDI_FT4222_HPP
#define _EXPORT_USERS_NECK_MRAA_FT4222_SRC_USB_FT4222_FTDI_FT4222_HPP

/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2015 Intel Corporation.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
