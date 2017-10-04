/*
 * Author: Serge Vakulenko <vak@besm6.org>
 * Copyright (c) 2017 Serge Vakulenko.
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
