/*
 * Author: Daniel Bovensiepen <oss@bovi.li>
 * Author: Zhu Jia Xing <jiaxing.zhu@siemens.com>
 * Copyright (c) 2022 Siemens Ltd. China.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "riscv/visionfive.h"
#include "mraa_internal.h"

mraa_platform_t
mraa_riscv_platform()
{
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
    if (mraa_file_contains("/proc/device-tree/compatible", "visionfive")) {
        platform_type = MRAA_VISIONFIVE;
    }

    switch (platform_type) {
        case MRAA_VISIONFIVE:
            plat = mraa_visionfive();
            break;

        default:
            plat = NULL;
            syslog(LOG_ERR, "Unknown Platform, currently not supported by MRAA");
    }
    return platform_type;
}
