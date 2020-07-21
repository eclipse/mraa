/*
 * Author: Brendan Le Foll
 * Copyright (c) 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 *
 * Example usage: Sets accelerometer scale, enables the x axis measurement and sets the trigger.
 *                Waits 100 seconds for the buffer trigger to happen.
 */

/* standard headers */
#include <stdlib.h>
#include <unistd.h>
#include <endian.h>

/* mraa header */
#include "mraa/iio.h"

/* IIO device */
#define IIO_DEV 0

static void
printword(uint16_t input, mraa_iio_channel* chan)
{
    int16_t res;

    if (!chan->lendian) {
        input = be16toh(input);
    } else {
        input = le16toh(input);
    }

    input >>= chan->shift;
    input &= chan->mask;
    if (chan->signedd) {
        res = (int16_t)(input << (16 - chan->bits_used)) >> (16 - chan->bits_used);
    } else {
        res = input;
    }
    fprintf(stdout, "  value = %05f\n", (float) res);
}

void
interrupt(char* data, void* args)
{
    mraa_iio_context thisdevice = (mraa_iio_context) args;
    mraa_iio_channel* channels = mraa_iio_get_channels(thisdevice);
    int i = 0;

    for (; i < mraa_iio_get_channel_count(thisdevice); i++) {
        if (channels[i].enabled) {
            fprintf(stdout, "channel %d - bytes %d\n", channels[i].index, channels[i].bytes);
            switch (channels[i].bytes) {
                case 2:
                    printword(*(uint16_t*) (data + channels[i].location), &channels[i]);
            }
        }
    }
}

void
event_interrupt(struct iio_event_data* data)
{
    int chan_type;
    int modifier;
    int type;
    int direction;
    int channel;
    int channel2;
    int different;
    mraa_iio_event_extract_event(data, &chan_type, &modifier, &type, &direction, &channel, &channel2, &different);

    fprintf(stdout,
            "event time %lld id %lld extracted chan_type %d modifier %d type %d direction %d "
            "channel %d channel2 %d different %d\n",
            data->timestamp, data->id, chan_type, modifier, type, direction, channel, channel2, different);
}

int
main(void)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_iio_context iio;
    float iio_float;
    int iio_int;

    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize iio */
    iio = mraa_iio_init(IIO_DEV);
    if (iio == NULL) {
        fprintf(stderr, "Failed to initialize IIO device\n");
        mraa_deinit();
        return EXIT_FAILURE;
    }

    status = mraa_iio_write_float(iio, "in_accel_scale", 0.019163);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to write scale to IIO device\n");
        goto err_exit;
    }

    status = mraa_iio_read_float(iio, "in_accel_scale", &iio_float);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to read scale from IIO device\n");
        goto err_exit;
    }

    fprintf(stdout, "IIO scale: %f\n", iio_float);

    status = mraa_iio_write_int(iio, "scan_elements/in_accel_x_en", 1);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to enable axis of the IIO device\n");
        goto err_exit;
    }

    status = mraa_iio_read_int(iio, "scan_elements/in_accel_x_en", &iio_int);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to read enable status from IIO device\n");
        goto err_exit;
    }

    fprintf(stdout, "IIO enable status: %d\n", iio_int);

    status = mraa_iio_trigger_buffer(iio, interrupt, (void*) iio);
    if (status != MRAA_SUCCESS) {
        fprintf(stderr, "Failed to set IIO buffer trigger\n");
        goto err_exit;
    }

    /* wait for the trigger */
    fprintf(stdout, "Waiting for the trigger...\n");
    sleep(100);

    /* close IIO device */
    mraa_iio_close(iio);

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* close IIO device */
    mraa_iio_close(iio);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
