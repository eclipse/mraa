/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "arm/96boards.h"
#include "arm/rockpi4.h"
#include "arm/de_nano_soc.h"
#include "arm/banana.h"
#include "arm/beaglebone.h"
#include "arm/phyboard.h"
#include "arm/raspberry_pi.h"
#include "arm/adlink_ipi.h"
#include "mraa_internal.h"


mraa_platform_t
mraa_arm_platform()
{
    mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
    size_t len = 100;
    char* line = malloc(len);
    FILE* fh = fopen("/proc/cpuinfo", "r");

    if (fh != NULL) {
        while (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "Hardware", 8) == 0) {
                if (strstr(line, "BCM2708")) {
                    platform_type = MRAA_RASPBERRY_PI;
                } else if (strstr(line, "BCM2709")) {
                    platform_type = MRAA_RASPBERRY_PI;
                } else if (strstr(line, "BCM2835")) {
                    platform_type = MRAA_RASPBERRY_PI;
                } else if (strstr(line, "Generic AM33XX")) {
                    if(mraa_file_contains("/proc/device-tree/model", "phyBOARD-WEGA")) {
                        platform_type = MRAA_PHYBOARD_WEGA;
                    } else {
                        platform_type = MRAA_BEAGLEBONE;
                    }
                } else if (strstr(line, "HiKey Development Board")) {
                    platform_type = MRAA_96BOARDS;
                } else if (strstr(line, "s900")) {
                    platform_type = MRAA_96BOARDS;
                } else if (strstr(line, "sun7i")) {
                    if (mraa_file_contains("/proc/device-tree/model", "Banana Pro")) {
                        platform_type = MRAA_BANANA;
                    } else if (mraa_file_contains("/proc/device-tree/model",
                                                  "Banana Pi")) {
                        platform_type = MRAA_BANANA;
                    }
                    // For old kernels
                    else if (mraa_file_exist("/sys/class/leds/green:ph24:led1")) {
                        platform_type = MRAA_BANANA;
                    }
                } else if (strstr(line, "DE0/DE10-Nano-SoC")) {
                        platform_type = MRAA_DE_NANO_SOC;
                // For different kernel version(s) of DE10-Nano
                } else if (strstr(line, "Altera SOCFPGA")) {
                        platform_type = MRAA_DE_NANO_SOC;
                }
            }
        }
        fclose(fh);
    }
    free(line);

    /* Get compatible string from Device tree for boards that dont have enough info in /proc/cpuinfo
     */
    if (platform_type == MRAA_UNKNOWN_PLATFORM) {
        if (mraa_file_contains("/proc/device-tree/model", "s900"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/compatible", "qcom,apq8016-sbc"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/compatible", "arrow,apq8096-db820c"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model",
                                    "HiKey Development Board"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model", "HiKey960"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model", "ROCK960"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model", "ZynqMP ZCU100 RevC"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model", "Avnet Ultra96 Rev1"))
            platform_type = MRAA_96BOARDS;
        else if (mraa_file_contains("/proc/device-tree/model", "ROCK Pi 4")  ||
                 mraa_file_contains("/proc/device-tree/model", "ROCK PI 4A") ||
                 mraa_file_contains("/proc/device-tree/model", "ROCK PI 4B")
                 )
            platform_type = MRAA_ROCKPI4;
        else if (mraa_file_contains("/proc/device-tree/compatible", "raspberrypi,"))
            platform_type = MRAA_RASPBERRY_PI;
        else if (mraa_file_contains("/proc/device-tree/model", "ADLINK ARM, LEC-PX30"))
            platform_type = MRAA_ADLINK_IPI;
    }

    switch (platform_type) {
        case MRAA_RASPBERRY_PI:
            plat = mraa_raspberry_pi();
            break;
        case MRAA_BEAGLEBONE:
            plat = mraa_beaglebone();
            break;
        case MRAA_PHYBOARD_WEGA:
            plat = mraa_phyboard();
            break;
        case MRAA_BANANA:
            plat = mraa_banana();
            break;
        case MRAA_96BOARDS:
            plat = mraa_96boards();
	    break;
        case MRAA_ROCKPI4:
	    plat = mraa_rockpi4();
            break;
        case MRAA_DE_NANO_SOC:
            plat = mraa_de_nano_soc();
            break;
	case MRAA_ADLINK_IPI:
	    plat = mraa_adlink_ipi();
	    break;
        default:
            plat = NULL;
            syslog(LOG_ERR, "Unknown Platform, currently not supported by MRAA");
    }
    return platform_type;
}
