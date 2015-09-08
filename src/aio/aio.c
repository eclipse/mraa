/*
 * Author: Nandkishor Sonar
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014, 2015 Intel Corporation.
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
#include <fcntl.h>
#include <errno.h>

#include "aio.h"
#include "mraa_internal.h"

#define DEFAULT_BITS 10

static int raw_bits;

static mraa_result_t
aio_get_valid_fp(mraa_aio_context dev)
{
    if (IS_FUNC_DEFINED(dev, aio_get_valid_fp)) {
        return dev->advance_func->aio_get_valid_fp(dev);
    }

    char file_path[64] = "";

    // Open file Analog device input channel raw voltage file for reading.
    snprintf(file_path, 64, "/sys/bus/iio/devices/iio:device0/in_voltage%d_raw", dev->channel);

    dev->adc_in_fp = open(file_path, O_RDONLY);
    if (dev->adc_in_fp == -1) {
        syslog(LOG_ERR, "aio: Failed to open input raw file %s for reading!", file_path);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_aio_context
mraa_aio_init_internal(mraa_adv_func_t* func_table)
{
    mraa_aio_context dev = malloc(sizeof(struct _aio));
    if (dev == NULL) {
        return NULL;
    }
    dev->advance_func = func_table;

    return dev;
}

mraa_aio_context
mraa_aio_init(unsigned int aio)
{
    if (plat == NULL) {
        syslog(LOG_ERR, "aio: Platform not initialised");
        return NULL;
    }
    if (mraa_is_sub_platform_id(aio)) {
        syslog(LOG_NOTICE, "aio: Using sub platform is not supported");
        return NULL;
    }

    // Create ADC device connected to specified channel
    mraa_aio_context dev = mraa_aio_init_internal(plat->adv_func);
    if (dev == NULL) {
        syslog(LOG_ERR, "aio: Insufficient memory for specified input channel %d", aio);
        return NULL;
    }
    int pin = aio + plat->gpio_count;
    dev->channel = plat->pins[pin].aio.pinmap;
    dev->value_bit = DEFAULT_BITS;

    if (IS_FUNC_DEFINED(dev, aio_init_pre)) {
        mraa_result_t pre_ret = (dev->advance_func->aio_init_pre(aio));
        if (pre_ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }
    if (aio > plat->aio_count) {
        syslog(LOG_ERR, "aio: requested channel out of range");
        free(dev);
        return NULL;
    }

    if (plat->pins[pin].capabilites.aio != 1) {
        syslog(LOG_ERR, "aio: pin uncapable of aio");
        free(dev);
        return NULL;
    }

    if (plat->pins[pin].aio.mux_total > 0) {
        if (mraa_setup_mux_mapped(plat->pins[pin].aio) != MRAA_SUCCESS) {
            free(dev);
            syslog(LOG_ERR, "aio: unable to setup multiplexers for pin");
            return NULL;
        }
    }

    // Open valid  analog input file and get the pointer.
    if (MRAA_SUCCESS != aio_get_valid_fp(dev)) {
        free(dev);
        return NULL;
    }
    raw_bits = mraa_adc_raw_bits();

    if (IS_FUNC_DEFINED(dev, aio_init_post)) {
        mraa_result_t ret = dev->advance_func->aio_init_post(dev);
        if (ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }

    return dev;
}

unsigned int
mraa_aio_read(mraa_aio_context dev)
{
    char buffer[17];
    unsigned int shifter_value = 0;

    if (dev->adc_in_fp == -1) {
        if (aio_get_valid_fp(dev) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "aio: Failed to get to the device");
            return 0;
        }
    }

    lseek(dev->adc_in_fp, 0, SEEK_SET);
    if (read(dev->adc_in_fp, buffer, sizeof(buffer)) < 1) {
        syslog(LOG_ERR, "aio: Failed to read a sensible value");
    }
    // force NULL termination of string
    buffer[16] = '\0';
    lseek(dev->adc_in_fp, 0, SEEK_SET);

    errno = 0;
    char* end;
    unsigned int analog_value = (unsigned int) strtoul(buffer, &end, 10);
    if (end == &buffer[0]) {
        syslog(LOG_ERR, "aio: Value is not a decimal number");
    } else if (errno != 0) {
        syslog(LOG_ERR, "aio: Errno was set");
    }

    if (dev->value_bit != raw_bits) {
        /* Adjust the raw analog input reading to supported resolution value*/
        if (raw_bits > dev->value_bit) {
            shifter_value = raw_bits - dev->value_bit;
            analog_value = analog_value >> shifter_value;
        } else {
            shifter_value = dev->value_bit - raw_bits;
            analog_value = analog_value << shifter_value;
        }
    }

    return analog_value;
}

float
mraa_aio_read_float(mraa_aio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "aio: Device not valid");
        return 0.0;
    }

    float max_analog_value = (1 << dev->value_bit) - 1;
    unsigned int analog_value_int = mraa_aio_read(dev);

    return analog_value_int / max_analog_value;
}

mraa_result_t
mraa_aio_close(mraa_aio_context dev)
{
    if (NULL != dev) {
        if (dev->adc_in_fp != -1)
            close(dev->adc_in_fp);
        free(dev);
    }

    return (MRAA_SUCCESS);
}

mraa_result_t
mraa_aio_set_bit(mraa_aio_context dev, int bits)
{
    if (dev == NULL || bits < 1) {
        syslog(LOG_ERR, "aio: Device not valid");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    dev->value_bit = bits;
    return MRAA_SUCCESS;
}

int
mraa_aio_get_bit(mraa_aio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "aio: Device not valid");
        return 0;
    }
    return dev->value_bit;
}
