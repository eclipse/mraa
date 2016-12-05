/*
 * Author: Brendan Le Foll
 * Copyright (c) 2015 Intel Corporation.
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

#include <unistd.h>
#include "mraa/iio.h"

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
    printf("  value = %05f\n", (float) res);
}

mraa_iio_context iio_device0;
mraa_iio_context iio_device6;

void
interrupt(char* data)
{
    mraa_iio_channel* channels = mraa_iio_get_channels(iio_device0);
    int i = 0;

    for (i; i < mraa_iio_get_channel_count(iio_device0); i++) {
        if (channels[i].enabled) {
            printf("channel %d - bytes %d\n", channels[i].index, channels[i].bytes);
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

    printf("event time %lld id %lld extracted chan_type %d modifier %d type %d direction %d "
           "channel %d channel2 %d different %d\n",
           data->timestamp, data->id, chan_type, modifier, type, direction, channel, channel2, different);
}

int
main()
{
    //! [Interesting]
    iio_device0 = mraa_iio_init(0);
    if (iio_device0 == NULL) {
        return EXIT_FAILURE;
    }

    float iio_float;
    int iio_int;
    mraa_result_t ret;

    ret = mraa_iio_write_float(iio_device0, "in_accel_scale", 0.019163);
    if (ret == MRAA_SUCCESS) {
        fprintf(stdout, "IIO write success\n");
    }

    ret = mraa_iio_read_float(iio_device0, "in_accel_scale", &iio_float);
    if (ret == MRAA_SUCCESS) {
        fprintf(stdout, "IIO read %f\n", iio_float);
    }

    ret = mraa_iio_write_int(iio_device0, "scan_elements/in_accel_x_en", 1);
    if (ret == MRAA_SUCCESS) {
        fprintf(stdout, "IIO write success\n");
    }

    ret = mraa_iio_read_int(iio_device0, "scan_elements/in_accel_x_en", &iio_int);
    if (ret == MRAA_SUCCESS) {
        fprintf(stdout, "IIO read %d\n", iio_int);
    }

    if (mraa_iio_trigger_buffer(iio_device0, interrupt, NULL) == MRAA_SUCCESS) {
        sleep(100);
        return EXIT_SUCCESS;
    }

    /*
    struct iio_event_data event;
    iio_device6 = mraa_iio_init(6);
    if (iio_device6 == NULL) {
        return EXIT_FAILURE;
    }
    mraa_iio_write_int(iio_device6, "events/in_proximity2_thresh_either_en", 1);


    // Blocking until event fired
    if (mraa_iio_event_poll(iio_device6, &event) == MRAA_SUCCESS) {
        event_interrupt(&event); //just to show data
    }

    if (mraa_iio_event_setup_callback(iio_device6, event_interrupt, NULL) == MRAA_SUCCESS) {
        sleep(100);
        return EXIT_SUCCESS;
    }*/

    //! [Interesting]
    return EXIT_FAILURE;
}
