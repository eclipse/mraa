/*
 * Author: Karena Anum Kamaruzaman <karena.anum.kamaruzaman@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "common.h"
#include "x86/intel_cherryhills.h"

#define PLATFORM_NAME "Braswell Cherry Hill"

mraa_board_t*
mraa_intel_cherryhills()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = MRAA_INTEL_CHERRYHILLS_PINCOUNT;
    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * MRAA_INTEL_CHERRYHILLS_PINCOUNT);
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    int pos = 0;
    //Physical header where these pins are: J3E5
    strncpy(b->pins[pos].name, "GSUS6", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 416;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "GSUS8", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 409;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "GSUS7", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 414;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    //Physical header where these pins are: J3E3
    strncpy(b->pins[pos].name, "GSUS0", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 406;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    strncpy(b->pins[pos].name, "GSUS1", 8);
    b->pins[pos].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[pos].gpio.pinmap = 410;
    b->pins[pos].gpio.mux_total = 0;
    pos++;

    return b;
error:
    syslog(LOG_CRIT, "Cherryhills(Braswell): Platform failed to initialise");
    free(b);
    return NULL;
}
