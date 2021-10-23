/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdio.h>

#include "mraa/initio.h"

int
main()
{
    printf("Starting example\n");

    mraa_io_descriptor* desc;
    if (mraa_io_init("g:3:1-upm_stuff", &desc) != MRAA_SUCCESS) {
        printf("Error in mraa_io_init()\n");
    }

    printf("Leftover string = %s\n", desc->leftover_str);

    /* Check value set in mraa_io_init. */
    printf("Gpio value = %d\n", mraa_gpio_read(desc->gpios[0]));

    if (mraa_io_close(desc) != MRAA_SUCCESS) {
        printf("failed to close mraa io descriptor\n");
    }

    printf("Done\n");
}