/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 *         Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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
#include "x86/intel_sofia_3gr.h"
#include "x86/intel_cherryhills.h"
#include "x86/intel_ilk.h"
#include "x86/up.h"
#include "x86/up2.h"
#include "x86/intel_joule_expansion.h"
#include "x86/iei_tank.h"
#include "x86/intel_adlink_lec_al.h"
#include "x86/up_xtreme.h"

mraa_platform_t
mraa_x86_platform()
{
#ifndef MRAA_PLATFORM_FORCE
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;

    char* line = NULL, buffer[20] = {0};
    // let getline allocate memory for *line
    size_t len = 0;
    FILE* fh = fopen("/sys/devices/virtual/dmi/id/board_name", "r");
    int fd;

    if (fh != NULL) {
        if (getline(&line, &len, fh) != -1) {
            // Sanitize input by terminating at any of possible end of line chars
            line[strcspn(line, "\r\n")] = 0;

            if (strncmp(line, "GalileoGen2", strlen("GalileoGen2") + 1) == 0 ||
                strncmp(line, "SIMATIC IOT2000", strlen("SIMATIC IOT2000") + 1) == 0) {
                platform_type = MRAA_INTEL_GALILEO_GEN2;
                plat = mraa_intel_galileo_gen2();
            } else if (strncmp(line, "BODEGA BAY", strlen("BODEGA BAY") + 1) == 0) {
                platform_type = MRAA_INTEL_EDISON_FAB_C;
                plat = mraa_intel_edison_fab_c();
            } else if (strncmp(line, "SALT BAY", strlen("SALT BAY") + 1) == 0) {
                platform_type = MRAA_INTEL_EDISON_FAB_C;
                plat = mraa_intel_edison_fab_c();
            } else if (strncmp(line, "DE3815", strlen("DE3815") + 1) == 0) {
                platform_type = MRAA_INTEL_DE3815;
                plat = mraa_intel_de3815();
            } else if (strncmp(line, "NUC5i5MYBE", strlen("NUC5i5MYBE") + 1) == 0 ||
                       strncmp(line, "NUC5i3MYBE", strlen("NUC5i3MYBE") + 1) == 0) {
                platform_type = MRAA_INTEL_NUC5;
                plat = mraa_intel_nuc5();
            } else if (strncmp(line, "NOTEBOOK", strlen("NOTEBOOK") + 1) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible(0);
            } else if (strncasecmp(line, "MinnowBoard MAX", strlen("MinnowBoard MAX") + 1) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible(0);
            } else if (strncasecmp(line, "Galileo", strlen("Galileo") + 1) == 0) {
                platform_type = MRAA_INTEL_GALILEO_GEN1;
                plat = mraa_intel_galileo_rev_d();
            } else if (strncasecmp(line, "MinnowBoard Compatible", strlen("MinnowBoard Compatible") + 1) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible(1);
            } else if (strncasecmp(line, "MinnowBoard Turbot", strlen("MinnowBoard Turbot") + 1) == 0) {
                platform_type = MRAA_INTEL_MINNOWBOARD_MAX;
                plat = mraa_intel_minnowboard_byt_compatible(1);
            } else if (strncasecmp(line, "Braswell Cherry Hill", strlen("Braswell Cherry Hill") + 1) == 0) {
                platform_type = MRAA_INTEL_CHERRYHILLS;
                plat = mraa_intel_cherryhills();
            } else if (strncasecmp(line, "Intel Learning Kit", strlen("Intel Learning Kit") + 1) == 0) {
                platform_type = MRAA_INTEL_ILK;
                plat = mraa_intel_ilk();
            } else if (strncasecmp(line, "UP-CHT01", strlen("UP-CHT01") + 1) == 0) {
                platform_type = MRAA_UP;
                plat = mraa_up_board();
            } else if (strncasecmp(line, "UP-APL01", strlen("UP-APL01") + 1) == 0) {
                platform_type = MRAA_UP2;
                plat = mraa_up2_board();
            } else if (strncasecmp(line, "RVP", strlen("RVP") + 1) == 0) {
                platform_type = MRAA_INTEL_JOULE_EXPANSION;
                plat = mraa_joule_expansion_board();
            } else if (strncasecmp(line, "SDS", strlen("SDS") + 1) == 0) {
                platform_type = MRAA_INTEL_JOULE_EXPANSION;
                plat = mraa_joule_expansion_board();
            } else if ((strncasecmp(line, "SAF3", strlen("SAF3") + 1) == 0) ) {
                platform_type = MRAA_IEI_TANK;
                plat = mraa_iei_tank();
            } else if ((strncasecmp(line, "LEC-ALAI", strlen("LEC-ALAI") + 1) == 0) ) {
                platform_type = MRAA_ADLINK_LEC_AL;
                plat = mraa_lec_al_board();
	    } else if ((strncasecmp(line, "LEC-AL", strlen("LEC-AL") + 1) == 0) ) {
                platform_type = MRAA_ADLINK_LEC_AL;
                plat = mraa_lec_al_board();
            } else if (strncasecmp(line, "UP-WHL01", strlen("UP-WHL01") + 1) == 0) {
                platform_type = MRAA_UPXTREME;
                plat = mraa_upxtreme_board();
            } else {
                syslog(LOG_ERR, "Platform not supported, not initialising");
                platform_type = MRAA_UNKNOWN_PLATFORM;
            }
            free(line);
        }
        fclose(fh);
    } else {
        fh = fopen("/proc/cmdline", "r");
        if (fh != NULL) {
            if (getline(&line, &len, fh) != -1) {
                if (strstr(line, "sf3gr_mrd_version=P2.0")) {
                    platform_type = MRAA_INTEL_SOFIA_3GR;
                    plat = mraa_intel_sofia_3gr();
                }
                free(line);
            }
            fclose(fh);
        }
    }

    if( (fd = open("/sys/devices/virtual/dmi/id/product_name", O_RDONLY)) != -1) {
	    syslog(LOG_ERR, "Checking additional Platform support for LEC-AL iPI");
	    if(read(fd, buffer, 10) > 0) {
		    if ((strncasecmp(buffer, "LEC-AL-AI", strlen("LEC-AL-AI")) == 0)) {
			    syslog(LOG_ERR, "LEC-AL AI IPi found. starting MRAA");
			    platform_type = MRAA_ADLINK_LEC_AL_AI;
			    plat = mraa_lec_al_board();
		    }
		    else if ((strncasecmp(buffer, "LEC-AL", strlen("LEC-AL")) == 0)) {
			    syslog(LOG_ERR, "LEC-AL IPi found. starting MRAA");
			    platform_type = MRAA_ADLINK_LEC_AL;
			    plat = mraa_lec_al_board();
		    }
		    close(fd);
	    }
    }

    return platform_type;
#else
    #if defined(xMRAA_INTEL_GALILEO_GEN2)
    plat = mraa_intel_galileo_gen2();
    #elif defined(xMRAA_INTEL_EDISON_FAB_C)
    plat = mraa_intel_edison_fab_c();
    #elif defined(xMRAA_INTEL_DE3815)
    plat = mraa_intel_de3815();
    #elif defined(xMRAA_INTEL_MINNOWBOARD_MAX)
    plat = mraa_intel_minnowboard_byt_compatible();
    #elif defined(xMRAA_INTEL_GALILEO_GEN1)
    plat = mraa_intel_galileo_rev_d();
    #elif defined(xMRAA_INTEL_NUC5)
    plat = mraa_intel_nuc5();
    #elif defined(xMRAA_INTEL_SOFIA_3GR)
    plat = mraa_intel_sofia_3gr();
    #elif defined(xMRAA_INTEL_CHERRYHILLS)
    plat = mraa_intel_cherryhills();
    #elif defined(xMRAA_UP)
    plat = mraa_up_board();
    #elif defined(xMRAA_UP2)
    plat = mraa_up2_board();
    #elif defined(xMRAA_INTEL_JOULE_EXPANSION)
    plat = mraa_joule_expansion_board();
    #elif defined(xMRAA_IEI_TANK)
    plat = mraa_iei_tank();
    #elif defined(xMRAA_UPXTREME)
    plat = mraa_upxtreme_board();
    #else
        #error "Not using a valid platform value from mraa_platform_t - cannot compile"
    #endif
    return MRAA_PLATFORM_FORCE;
#endif
}
