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
#define IIO_EVENTS "events"

mraa_iio_context
mraa_iio_init(int device)
{
    if (device > plat_iio->iio_device_count) {
        return NULL;
    }

    mraa_iio_get_channel_data(&plat_iio->iio_devices[device]);
	mraa_iio_get_event_data(&plat_iio->iio_devices[device]);
	
    return &plat_iio->iio_devices[device];
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
	
	dev->datasize = 0;

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
	//no need proceed if no channel found
	if (chan_num == 0)
		return MRAA_SUCCESS;
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
                // grab the type of the buffer
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
                    // probably should be 5?
                    if (ret < 0) {
                        // cleanup
                        free(str);
                        close(fd);
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
                // grab the enable flag of channel
                snprintf(buf, MAX_SIZE, "%sen", str);
                fd = open(buf, O_RDONLY);
                if (fd > 0) {
                    if (read(fd, readbuf, 2 * sizeof(char)) != 2) {
                        syslog(LOG_ERR, "iio: Failed to read a sensible value from sysfs");
                        return -1;
                    }
                    chan->enabled = (int) strtol(readbuf, NULL, 10);
					
					//only calculate enable buffer size for trigger buffer extract data
					if (chan->enabled)
						dev->datasize += chan->bytes;
						
                    close(fd);
                }
                // clean up str var
                free(str);
            }
        }
    }

    return MRAA_SUCCESS;
}

const char*
mraa_iio_get_device_name(mraa_iio_context dev)
{
    return dev->name;
}

int
mraa_iio_get_device_num_by_name(const char* name)
{
    int i;

    if (plat_iio == NULL) {
        syslog(LOG_ERR, "iio: platform IIO structure is not initialized");
        return -1;
    }

    if (name == NULL) {
        syslog(LOG_ERR, "iio: device name is NULL, unable to find its number");
        return -1;
    }

    for (i = 0; i < plat_iio->iio_device_count; i++) {
        struct _iio* device;
        device = &plat_iio->iio_devices[i];
        // we want to check for exact match
        if (strncmp(device->name, name, strlen(device->name)+1) == 0) {
            return device->num;
        }
    }

    return -1;
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
mraa_iio_write(mraa_iio_context dev, const char* attr_chan, const char* data)
{
    char buf[128];
    snprintf(buf, 128, IIO_SYSFS_DEVICE "%d/%s", dev->num, attr_chan);
    int fd = open(buf, O_WRONLY);
    if (fd != -1) {
		write(fd, data, ( strlen(data) +1 ));
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_UNSPECIFIED;
}

static mraa_result_t
mraa_iio_wait_event(int fd, char* data, int *read_size)
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
    *read_size = read(fd, data, 100);

    return MRAA_SUCCESS;
}

static void*
mraa_iio_trigger_handler(void* arg)
{
    mraa_iio_context dev = (mraa_iio_context) arg;
		int i;
    char data[MAX_SIZE*100];
		int read_size;

    for (;;) {
        if (mraa_iio_wait_event(dev->fp, &data[0], &read_size) == MRAA_SUCCESS) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
			//only can process if readsize >= enabled channel's datasize
			for (i=0; i<(read_size/dev->datasize); i++)
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

mraa_result_t
mraa_iio_get_event_data(mraa_iio_context dev)
{
    const struct dirent *ent;
    DIR* dir;
    int event_num = 0;
    char buf[MAX_SIZE];
    char readbuf[32];
    int fd;
    int ret = 0;
    int padint = 0;
    int curr_bytes = 0;
    char shortbuf, signchar;
    memset(buf, 0, MAX_SIZE);
    snprintf(buf, MAX_SIZE, IIO_SYSFS_DEVICE "%d/" IIO_EVENTS, dev->num);
    dir = opendir(buf);
    if (dir != NULL) {
        while ((ent = readdir(dir)) != NULL) {
            if (strcmp(ent->d_name + strlen(ent->d_name) - strlen("_en"), "_en") == 0) {
                event_num++;
            }
        }
        dev->event_num = event_num;
		//no need proceed if no event found
		if (event_num == 0)
			return MRAA_SUCCESS;
        mraa_iio_event* event;
		dev->events = calloc(event_num, sizeof(mraa_iio_event));
		if ( dev->events == NULL)
		{
			closedir(dir);
			return MRAA_ERROR_UNSPECIFIED;
		}
		rewinddir(dir);
		event_num = 0;
		while ((ent = readdir(dir)) != NULL) 
		{
            if (strcmp(ent->d_name + strlen(ent->d_name) - strlen("_en"), "_en") == 0) { 
				event = &dev->events[event_num];
                event->name = strdup(ent->d_name);
				snprintf(buf, MAX_SIZE, IIO_SYSFS_DEVICE "%d/" IIO_EVENTS "/%s", dev->num, ent->d_name);
				fd = open(buf, O_RDONLY);
				if (fd > 0) {
					if (read(fd, readbuf, 2 * sizeof(char)) != 2) {
						break;
					}
					close(fd);								
				}
                event->enabled = ((int) strtol(readbuf, NULL, 10));
				//Todo, read other event info.
				event_num++;
            }
        }
		closedir(dir);
    }
    return MRAA_SUCCESS;	
}

mraa_result_t
mraa_iio_event_read(mraa_iio_context dev, const char* attribute, float* data)
{
    char buf[64];
    snprintf(buf, 64, IIO_SYSFS_DEVICE "%d/" IIO_EVENTS "/%s", dev->num, attribute);
    int fd = open(buf, O_RDONLY);
    if (fd != -1) {
        int len = read(fd, &buf, 64);
        *data = strtol(buf, NULL, 10);
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_UNSPECIFIED;
}

mraa_result_t
mraa_iio_event_write(mraa_iio_context dev, const char* attribute,  const char* data)
{
	int len;
    char buf[128];
    snprintf(buf, 128, IIO_SYSFS_DEVICE "%d/" IIO_EVENTS "/%s", dev->num, attribute);
    int fd = open(buf, O_WRONLY);
    if (fd != -1) {
		int len = write(fd, data, ( strlen(data) +1 ));
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_UNSPECIFIED;
}

static mraa_result_t
mraa_iio_event_poll_nonblock(int fd, struct iio_event_data* data)
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

    read(fd, data, sizeof(struct iio_event_data));

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_iio_event_poll(mraa_iio_context dev, struct iio_event_data* data)
{
    char bu[MAX_SIZE];
    int ret;
	int event_fd;
	int fd;
	
	sprintf(bu, IIO_SLASH_DEV "%d", dev->num);
    fd = open(bu, 0);
	ret = ioctl(fd, IIO_GET_EVENT_FD_IOCTL, &event_fd);
	close(fd);
	
	if (ret == -1 || event_fd == -1) 
		return MRAA_ERROR_UNSPECIFIED;		

	ret = read(event_fd, data, sizeof(struct iio_event_data));
	
	close(event_fd);
    return MRAA_SUCCESS;
}

static void*
mraa_iio_event_handler(void* arg)
{
	struct iio_event_data data;
    mraa_iio_context dev = (mraa_iio_context) arg;

    for (;;) {
        if (mraa_iio_event_poll_nonblock(dev->fp_event, &data) == MRAA_SUCCESS) {
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            dev->isr_event(&data);
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
        } else {
            // we must have got an error code so die nicely
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
            return NULL;
        }
    }
}

mraa_result_t 
mraa_iio_event_setup_callback(mraa_iio_context dev, void (*fptr)(struct iio_event_data* data), void* args)
{
	int ret;
    char bu[MAX_SIZE];
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    sprintf(bu, IIO_SLASH_DEV "%d", dev->num);
    dev->fp = open(bu, O_RDONLY | O_NONBLOCK);
    if (dev->fp == -1) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
	ret = ioctl(dev->fp, IIO_GET_EVENT_FD_IOCTL, &dev->fp_event);
	close(dev->fp);
	
	if (ret == -1 || dev->fp_event == -1) 
	{
		return MRAA_ERROR_UNSPECIFIED;	
	}
		
    dev->isr_event = fptr;
    pthread_create(&dev->thread_id, NULL, mraa_iio_event_handler, (void*) dev);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_iio_event_extract_event(struct iio_event_data* event, int* chan_type, int* modifier, int* type, int* direction, int* channel, int* channel2, int* different)
{
	*chan_type = IIO_EVENT_CODE_EXTRACT_CHAN_TYPE(event->id);
	*modifier= IIO_EVENT_CODE_EXTRACT_MODIFIER(event->id);
	*type = IIO_EVENT_CODE_EXTRACT_TYPE(event->id);
	*direction = IIO_EVENT_CODE_EXTRACT_DIR(event->id);
	*channel = IIO_EVENT_CODE_EXTRACT_CHAN(event->id);
	*channel2 = IIO_EVENT_CODE_EXTRACT_CHAN2(event->id);
	*different = IIO_EVENT_CODE_EXTRACT_DIFF(event->id);
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

