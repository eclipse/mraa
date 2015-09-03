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

#include <stdlib.h>
#include <string.h>

#include "mraa_internal.h"
#include "usb/ftdi_ft4222.h"


mraa_platform_t
mraa_usb_platform_extender(mraa_board_t* board)
{
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
    if (mraa_ftdi_ft4222_init() == MRAA_SUCCESS) {
        unsigned int versionChip, versionLib;
        if (mraa_ftdi_ft4222_get_version(&versionChip, &versionLib) == MRAA_SUCCESS) {
            // TODO: Add ft4222 version checks
            platform_type = MRAA_FTDI_FT4222;        
        }
    }  
    switch (platform_type) {
        case MRAA_FTDI_FT4222:
            mraa_ftdi_ft4222(board);
            break;
        default:
            syslog(LOG_ERR, "Unknown USB Platform Extender, currently not supported by MRAA");
    }
    return platform_type;
}
