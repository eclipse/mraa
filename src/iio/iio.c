/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
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

#include "iio.h"
#include "mraa_internal.h"
#include "dirent.h"
#include <string.h>
#include <poll.h>

#define MAX_SIZE 128
#define IIO_DEVICE "iio:device"
#define IIO_SCAN_ELEM "scan_elements"
#define IIO_SLASH_DEV "/dev/"IIO_DEVICE
#define IIO_SYSFS_DEVICE "/sys/bus/iio/devices/"IIO_DEVICE

mraa_iio_context
mraa_iio_init(int device)
{
    if (device > plat->iio_device_count) {
        return NULL;
    }

    mraa_iio_get_channel_data(&plat->iio_devices[device]);
    return &plat->iio_devices[device];
}

int
mraa_iio_read_size(mraa_iio_context dev)
{
    return dev->datasize;
}

mraa_iio_channel*
mraa_iio_get_channels(mraa_iio_context dev)
{
    return dev->channels;
}

int
mraa_iio_get_channel_count(mraa_iio_context dev)
{
    return dev->chan_num;
}

mraa_result_t
mraa_iio_get_channel_data(mraa_iio_context dev)
{
    const struct dirent *ent;
    DIR* dir;
    int chan_num = 0;
    char buf[MAX_SIZE];
    char readbuf[32];
    int fd;
    int ret = 0;
    int padint = 0;
    int curr_bytes = 0;
    char shortbuf, signchar;
    memset(buf, 0, MAX_SIZE);
    snprintf(buf, MAX_SIZE, IIO_SYSFS_DEVICE "%d/" IIO_SCAN_ELEM, dev->num);
    dir = opendir(buf);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name + strlen(ent->d_name) - strlen("_en"), "_en") == 0) {
                    chan_num++;
            }
        }
    }
    dev->chan_num = chan_num;
    mraa_iio_channel* chan;
    dev->channels = calloc(chan_num, sizeof(mraa_iio_channel));
    seekdir(dir, 0);
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name + strlen(ent->d_name) - strlen("_index"), "_index") == 0) {
            snprintf(buf, MAX_SIZE, IIO_SYSFS_DEVICE "%d/" IIO_SCAN_ELEM "/%s", dev->num, ent->d_name);
            fd = open(buf, O_RDONLY);
            if (fd > 0) {
                if (read(fd, readbuf, 2 * sizeof(char)) != 2) {
                    break;
                }
                chan_num = ((int) strtol(readbuf, NULL, 10));
                chan = &dev->channels[chan_num];
                chan->index = chan_num;
                close(fd);

                buf[(strlen(buf)-5)] = '\0';
                char* str = strdup(buf);
                snprintf(buf, MAX_SIZE, "%stype", str);
                fd = open(buf, O_RDONLY);
                if (fd > 0) {
                    read(fd, readbuf, 31 * sizeof(char));
                    ret = sscanf(readbuf, "%ce:%c%u/%u>>%u", &shortbuf,
                                    &signchar, &chan->bits_used,
                                    &padint, &chan->shift);
                    chan->bytes = padint / 8;
                    if (curr_bytes % chan->bytes == 0) {
                        chan->location = curr_bytes;
                    } else {
                        chan->location = curr_bytes - curr_bytes%chan->bytes + chan->bytes;
                    }
                    curr_bytes = chan->location + chan->bytes;
                    if (ret < 0) {
                        // probably should be 5?
                        return MRAA_IO_SETUP_FAILURE;
                    }
                    chan->signedd = (signchar == 's');
                    chan->lendian = (shortbuf == 'l');
                    if (chan->bits_used == 64) {
                        chan->mask = ~0;
                    } else {
                        chan->mask = (1 << chan->bits_used) - 1;
                    }

                    close(fd);
                }
                // todo - read scale & _en attributes
            }
        }
    }
    dev->datasize = curr_bytes;

    return MRAA_SUCCESS;
}

const char*
mraa_iio_get_device_name(mraa_iio_context dev)
{
    return dev->name;
}

mraa_result_t
mraa_iio_read(mraa_iio_context dev, const char* attr_chan, float* data)
{
    char buf[64];
    snprintf(buf, 64, IIO_SYSFS_DEVICE "%d/%s", dev->num, attr_chan);
    int fd = open(buf, O_RDONLY);
    if (fd != -1) {
        int len = read(fd, &buf, 64);
        *data = strtol(buf, NULL, 10);
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
mraa_iio_write(mraa_iio_context dev, const char* attr_chan)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_iio_wait_event(int fd, char* data)
{
    struct pollfd pfd;

    if (fd < 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    pfd.fd = fd;
    pfd.events = POLLIN;

    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    int x = poll(&pfd, 1, -1);

    memset(data, 0, 100);
    read(fd, data, 100);

    return MRAA_SUCCESS;
}

static void*
mraa_iio_trigger_handler(void* arg)
{
    mraa_iio_context dev = (mraa_iio_context) arg;
    char data[MAX_SIZE*100];

    for (;;) {
        if (mraa_iio_wait_event(dev->fp, &data[0]) == MRAA_SUCCESS) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            dev->isr(&data);
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        } else {
            // we must have got an error code so die nicely
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            return NULL;
        }
    }
}

mraa_result_t
mraa_iio_trigger_buffer(mraa_iio_context dev, void (*fptr)(char* data), void* args)
{
    char bu[MAX_SIZE];
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    sprintf(bu, IIO_SLASH_DEV "%d", dev->num);
    dev->fp = open(bu, O_RDONLY | O_NONBLOCK);
    if (dev->fp == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->isr = fptr;
    pthread_create(&dev->thread_id, NULL, mraa_iio_trigger_handler, (void*) dev);

    return MRAA_SUCCESS;
}

#if 0
// does stop make any sense on iio devices?
mraa_result_t
mraa_iio_stop(mraa_iio_context dev)
{
    return MRAA_SUCCESS;
}
#endif

