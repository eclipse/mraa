/*
 * Author: Brendan Le Foll
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <unistd.h>

#include "mraa.h"
#include "mraa/firmata.h"

#define FIRMATA_START_SYSEX 0xF0
#define FIRMATA_END_SYSEX 0xF7
#define FIRMATA_CURIE_IMU 0x11
#define FIRMATA_CURIE_IMU_READ_ACCEL 0x00

void
interrupt(uint8_t* buf, int length)
{
    printf("reg read returned: %d, with buffer size %d\n", ((buf[6] & 0x7f) | ((buf[7] & 0x7f) << 7)), length);
}

int
main()
{
    mraa_init();
    //! [Interesting]

    /**
     * This example reads from the FirmataCurieIMU plugin
     */

    mraa_add_subplatform(MRAA_GENERIC_FIRMATA, "/dev/ttyACM0");
    mraa_firmata_context firm = mraa_firmata_init(FIRMATA_CURIE_IMU);
    if (firm == NULL) {
        return EXIT_FAILURE;
    }

    mraa_firmata_response(firm, interrupt);

    char* buffer = calloc(4, 0);
    if (buffer == NULL) {
        free(firm);
        return EXIT_FAILURE;
    }
    buffer[0] = FIRMATA_START_SYSEX;
    buffer[1] = FIRMATA_CURIE_IMU;
    buffer[2] = FIRMATA_CURIE_IMU_READ_ACCEL;
    buffer[3] = FIRMATA_END_SYSEX;

    mraa_firmata_write_sysex(firm, buffer, 4);

    sleep(1);

    // stop the isr and set it again
    mraa_firmata_response_stop(firm);
    mraa_firmata_response(firm, interrupt);
    mraa_firmata_write_sysex(firm, buffer, 4);

    sleep(1);

    // close everything and try again
    mraa_firmata_close(firm);
    firm = mraa_firmata_init(FIRMATA_CURIE_IMU);
    mraa_firmata_response(firm, interrupt);
    mraa_firmata_write_sysex(firm, buffer, 4);

    sleep(10);

    mraa_firmata_close(firm);
    //! [Interesting]

    return EXIT_SUCCESS;
}
