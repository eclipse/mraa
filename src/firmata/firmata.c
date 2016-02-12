/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2016 Intel Corporation.
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

mraa_board_t
mraa_firmata_init(mraa_uart_context uart_dev)
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }
    b->platform_name = MRAA_FIRMATA;
    b->platform_version = "2.3";
}

mraa_platform_t
mraa_firmata_platform(mraa_board_t* board, mraa_uart_context uart_dev)
{
   /** 
    * Firmata boards are not something we can detect so we just trust the user
    * to initialise them themselves and is the only platform type not to be
    * initialised from mraa_init(). Good luck!
    */
    mraa_board_t* sub_plat = NULL;
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;

    sub_plat = mraa_firmata();

    if (sub_plat != NULL) {
        sub_plat->platform_type = platform_type;
        board->sub_platform = sub_plat;
    }

    return platform_type;
}

