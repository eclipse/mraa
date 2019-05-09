/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>
#include <string.h>
#include <syslog.h>
//! [Interesting]
#include "mraa.h"

int
main(int argc, char** argv)
{
    const char* board_name = mraa_get_platform_name();
    fprintf(stdout, "hello mraa\n Version: %s\n Running on %s\n", mraa_get_version(), board_name);

    mraa_deinit();

    return MRAA_SUCCESS;
}
//! [Interesting]
