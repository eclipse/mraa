/*
 * Author: Serge Vakulenko <vak@besm6.org>
 * Copyright (c) 2017 Serge Vakulenko.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "mraa_internal.h"
#include "mips/mediatek.h"

mraa_platform_t
mraa_mips_platform()
{
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
    size_t len = 100;
    char* line = malloc(len);
    FILE* fh = fopen("/proc/cpuinfo", "r");
    if (fh != NULL) {
        while (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "machine", 7) == 0) {
                if (strstr(line, "MediaTek LinkIt Smart 7688")) {
                    platform_type = MRAA_MTK_LINKIT;
                }
                if (strstr(line, "Onion Omega2")) {
                    platform_type = MRAA_MTK_OMEGA2;
                }
            }
        }
        fclose(fh);
    }
    free(line);

    switch (platform_type) {
        case MRAA_MTK_LINKIT:
            plat = mraa_mtk_linkit();
            break;
        case MRAA_MTK_OMEGA2:
            plat = mraa_mtk_omega2();
            break;
        default:
            plat = NULL;
            syslog(LOG_ERR, "Unknown Platform, currently not supported by MRAA");
    }
    return platform_type;
}
