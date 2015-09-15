/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
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
#include "x86/intel_galileo_rev_d.h"
#include "x86/intel_galileo_rev_g.h"
#include "x86/intel_edison_fab_c.h"
#include "x86/intel_de3815.h"
#include "x86/intel_nuc5.h"
#include "x86/intel_minnow_byt_compatible.h"

mraa_platform_t
mraa_x86_platform()
{
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
    char* line = NULL;
    // let getline allocate memory for *line
    size_t len = 0;
    FILE* fh = fopen("/sys/devices/virtual/dmi/id/board_name", "r");
    if (fh != NULL) {
        if (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "GalileoGen2", 11) == 0) {
                platform_type = MRAA_INTEL_GALILEO_GEN2;
                plat = mraa_intel_galileo_gen2();
            } else if (strncmp(line, "BODEGA BAY", 10) == 0) {
                platform_type = MRAA_INTEL_EDISON_FAB_C;
                plat = mraa_intel_edison_fab_c();
            } else if (strncmp(line, "SALT BAY", 8) == 0) {
                platform_type = MRAA_INTEL_EDISON_FAB_C;
                plat = mraa_intel_edison_fab_c();
            } else if (strncmp(line, "DE3815", 6) == 0) {
                platform_type = MRAA_INTEL_DE3815;
                plat = mraa_intel_de3815();
            } else if (strncmp(line, "NUC5i5MYBE", 10) == 0 || strncmp(line, "NUC5i3MYBE", 10) == 0) {
                platform_type = MRAA_INTEL_NUC5;
                plat = mraa_intel_nuc5();
            } else if (strncmp(line, "NOTEBOOK", 8) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible();
            } else if (strncasecmp(line, "MinnowBoard MAX", 15) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible();
            } else if (strncasecmp(line, "Galileo", 7) == 0) {
                platform_type = MRAA_INTEL_GALILEO_GEN1;
                plat = mraa_intel_galileo_rev_d();
            } else if (strncasecmp(line, "MinnowBoard Compatible", 22) == 0) {
		platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
		plat = mraa_intel_minnowboard_byt_compatible();
	    } else {
                syslog(LOG_ERR, "Platform not supported, not initialising");
                platform_type = MRAA_UNKNOWN_PLATFORM;
            }
            free(line);
        }
        fclose(fh);
    }

    return platform_type;
}
