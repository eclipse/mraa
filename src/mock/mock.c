/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>

#include "mraa_internal.h"

#include "mock/mock_board.h"

mraa_platform_t
mraa_mock_platform()
{
    mraa_platform_t platform_type = MRAA_MOCK_PLATFORM;
    plat = mraa_mock_board();

    if (plat == NULL) {
        syslog(LOG_ERR, "Was not able to initialize mock platform");
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
    }

    return platform_type;
}
