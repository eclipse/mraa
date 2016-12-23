/*
 * Author: Constantin Musca <constantin.musca@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014-2016 Intel Corporation.
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

#include <ctype.h>
#include <errno.h>
#include <string.h>
#include "mraa_internal.h"
#include "gpio.h"
#include "version.h"
#include <peripheralmanager/peripheral_manager_client.h>

BPeripheralManagerClient *client = NULL;
char **gpios = NULL;
int gpios_count = 0;
char **i2c_busses = NULL;
int i2c_busses_count = 0;
char **spi_busses = NULL;
int spi_busses_count = 0;
char **uart_devices = NULL;
int uart_busses_count = 0;
mraa_lang_func_t* lang_func = NULL;

const char*
mraa_get_version()
{
    return gVERSION;
}

mraa_result_t
mraa_set_log_level(int level)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return MRAA_ERROR_INVALID_PARAMETER;
}

static void free_resources(char ***resources, int count)
{
    int i;

    if (*resources != NULL) {
        for(i = 0; i < count; i++) {
            free((*resources)[i]);
        }
        free(*resources);
    }

    *resources = NULL;
}

mraa_result_t
mraa_init()
{
    if (client != NULL) {
        BPeripheralManagerClient_delete(client);
    }

    client = BPeripheralManagerClient_new();
    if (client == NULL) {
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
    }

    gpios = BPeripheralManagerClient_listGpio(client, &gpios_count);
    i2c_busses = BPeripheralManagerClient_listI2cBuses(client, &i2c_busses_count);
    spi_busses = BPeripheralManagerClient_listSpiBuses(client, &spi_busses_count);
    uart_devices = BPeripheralManagerClient_listUartDevices(client, &uart_busses_count);
    //lang_func
    lang_func = (mraa_lang_func_t*) calloc(1, sizeof(mraa_lang_func_t));
    if (lang_func == NULL) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    return MRAA_SUCCESS;
}

void
mraa_deinit()
{
    free_resources(&uart_devices, uart_busses_count);
    free_resources(&spi_busses, spi_busses_count);
    free_resources(&i2c_busses, i2c_busses_count);
    free_resources(&gpios, gpios_count);

    if (client != NULL) {
        BPeripheralManagerClient_delete(client);
        client = NULL;
    }
}


mraa_platform_t
mraa_get_platform_type()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

int mraa_get_default_i2c_bus(uint8_t platform_offset) {
    // return first bus available
    return 0;
}

unsigned int
mraa_adc_raw_bits()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;

}

unsigned int
mraa_adc_supported_bits()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

int
mraa_set_priority(const int priority)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

const char*
mraa_get_platform_name()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return NULL;
}

const char*
mraa_get_platform_version(int platform_offset)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return NULL;
}

unsigned int
mraa_get_pin_count()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

int
mraa_get_i2c_bus_count()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

int
mraa_get_i2c_bus_id(int i2c_bus)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

mraa_boolean_t
mraa_pin_mode_test(int pin, mraa_pinmodes_t mode)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

char*
mraa_get_pin_name(int pin)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

mraa_boolean_t
mraa_has_sub_platform()
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

mraa_boolean_t
mraa_is_sub_platform_id(int pin_or_bus)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return 0;
}

int
mraa_get_sub_platform_id(int pin_or_bus)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

int
mraa_get_sub_platform_index(int pin_or_bus)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return -1;
}

mraa_result_t
mraa_add_subplatform(mraa_platform_t subplatformtype, const char* uart_dev)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    return MRAA_ERROR_INVALID_PARAMETER;
}

mraa_result_t
mraa_remove_subplatform(mraa_platform_t subplatformtype)
{
    return MRAA_ERROR_INVALID_PARAMETER;
}

void
mraa_result_print(mraa_result_t result)
{
    syslog(LOG_WARNING, "mraa: function not implementedin PMRAA");
    fprintf(stdout, "MRAA: Unrecognised error.");
}

void
mraa_to_upper(char* s)
{
    char* t = s;
    for (; *t; ++t) {
        *t = toupper(*t);
    }
}

mraa_result_t
mraa_atoi(char* intStr, int* value)
{
    char* end;
    // here 10 determines the number base in which strol is to work
    long val = strtol(intStr, &end, 10);
    if (*end != '\0' || errno == ERANGE || end == intStr || val > INT_MAX || val < INT_MIN) {
        *value = 0;
        return MRAA_ERROR_UNSPECIFIED;
    }
    *value = (int) val;
    return MRAA_SUCCESS;
}

void*
mraa_init_io(const char* desc)
{
    void* dev;
    int rc;
    const char* delim = "-";
    int length = 0;
    // 256 denotes the maximum size of our buffer
    // 8 denotes the maximum size of our type rounded to the nearest power of 2
    // max size is 4 + 1 for the \0 = 5 rounded to 8
    char buffer[256] = { 0 }, type[8] = { 0 };
    char *token = 0, *str = 0;
    if (desc == NULL) {
        return NULL;
    }
    length = strlen(desc);
    // Check to see the length is less than or equal to 255 which means
    // byte 256 is supposed to be \0
    if (length > 255 || length == 0) {
        return NULL;
    }
    strncpy(buffer, desc, length);

    str = buffer;
    token = strsep(&str, delim);
    length = strlen(token);
    // Check to see they haven't given us a type whose length is greater than the
    // largest type we know about
    if (length > 4) {
        syslog(LOG_ERR, "mraa_init_io: An invalid IO type was provided");
        return NULL;
    }
    strncpy(type, token, length);
    mraa_to_upper(type);
    token = strsep(&str, delim);
    // Check that they've given us more information than just the type
    if (token == NULL) {
        syslog(LOG_ERR, "mraa_init_io: Missing information after type");
        return NULL;
    }

    if (strncmp(type, "GPIO", 4) == 0) {
        if ((dev = calloc(sizeof(struct _gpio), 1)) == NULL) {
            syslog(LOG_ERR, "mraa_init_io: cannot allocate memory for GPIO %s", token);
            return NULL;
        }

        rc = BPeripheralManagerClient_openGpio(client, token, &((mraa_gpio_context)dev)->bgpio);
        if (rc != 0) {
            syslog(LOG_ERR, "mraa_init_io: cannot open GPIO %s", token);
            free(dev);
            return NULL;
        }

        return dev;
    } else if (strncmp(type, "I2C", 3) == 0) {
        dev = calloc(1, sizeof(struct _i2c));
        if (dev == NULL) {
            syslog(LOG_ERR, "mraa_init_io: cannot allocate memory for I2C %s", token);
            return NULL;
        }

        strncpy(((mraa_i2c_context)dev)->bus_name, token, MAX_I2C_BUS_NAME - 1);
        return dev;
    } else if (strncmp(type, "SPI", 3) == 0) {
        dev = calloc(1, sizeof(struct _spi));
        if (dev == NULL) {
            syslog(LOG_ERR, "mraa_init_io: cannot allocate memory for SPI %s", token);
            return NULL;
        }

        rc = BPeripheralManagerClient_openSpiDevice(client, token, &((mraa_spi_context)dev)->bspi);
        if (rc != 0) {
            syslog(LOG_ERR, "mraa_init_io: cannot open SPI %s", token);
            free(dev);
            return NULL;
        }

        return dev;
    } else if (strncmp(type, "UART", 4) == 0) {
        dev = calloc(1, sizeof(struct _uart));
        if (dev == NULL) {
            syslog(LOG_ERR, "mraa_init_io: cannot allocate memory for UART %s", token);
            return NULL;
        }

        rc = BPeripheralManagerClient_openUartDevice(client, token, &((mraa_uart_context)dev)->buart);
        if (rc != 0) {
            syslog(LOG_ERR, "mraa_init_io: cannot open UART %s", token);
            free(dev);
            return NULL;
        }
    }

    syslog(LOG_ERR, "mraa_init_io: Invalid IO type given.");
    return NULL;
}

#ifndef JSONPLAT
mraa_result_t
mraa_init_json_platform(const char* desc)
{
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}
#endif
