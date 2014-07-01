/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Copyright (c) 2014 Intel Corporation.
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

#include <stddef.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>

#include "mraa_internal.h"
#include "intel_galileo_rev_d.h"
#include "intel_galileo_rev_g.h"
#include "gpio.h"
#include "version.h"

//static mraa_pininfo_t* pindata;
static mraa_board_t* plat = NULL;
static mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
static mraa_adv_func* advance = NULL;

const char *
mraa_get_version()
{
    return gVERSION;
}

#if 0
const mraa_adv_func*
mraa_get_advance()
{
    return (const mraa_adv_func*) advance;
}
#endif

mraa_result_t
mraa_init()
{
    /** Once more board definitions have been added,
     *  A method for detecting them will need to be devised.
     */
    if (plat != NULL) {
        return MRAA_ERROR_PLATFORM_ALREADY_INITIALISED;
    }
#ifdef SWIGPYTHON
    // Initialise python threads, this allows use to grab the GIL when we are
    // required to do so
    Py_InitializeEx(0);
    PyEval_InitThreads();
#endif
    // detect a galileo gen2 board
    char *line = NULL;
    // let getline allocate memory for *line
    size_t len = 0;
    FILE *fh = fopen("/sys/devices/virtual/dmi/id/board_name", "r");
    if (fh != NULL) {
        if (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "GalileoGen2", 10) == 0) {
                platform_type = MRAA_INTEL_GALILEO_GEN2;
            } else {
                platform_type = MRAA_INTEL_GALILEO_GEN1;
            }
        }
    }
    free(line);
    fclose(fh);

    advance = (mraa_adv_func*) malloc(sizeof(mraa_adv_func));
    switch(platform_type) {
        case MRAA_INTEL_GALILEO_GEN2:
            plat = mraa_intel_galileo_gen2(advance);
            break;
        case MRAA_INTEL_GALILEO_GEN1:
            plat = mraa_intel_galileo_rev_d(advance);
            break;
        default:
            plat = mraa_intel_galileo_rev_d(advance);
            fprintf(stderr, "Platform not found, initialising MRAA_INTEL_GALILEO_GEN1\n");
    }

    return MRAA_SUCCESS;
}

void
mraa_deinit()
{
    free(plat->pins);
    free(plat);
}

int
mraa_set_priority(const unsigned int priority)
{
    struct sched_param sched_s;

    memset(&sched_s, 0, sizeof(struct sched_param));
    if (priority > sched_get_priority_max(SCHED_RR)) {
        sched_s.sched_priority = sched_get_priority_max(SCHED_RR);
    }
    else {
        sched_s.sched_priority = priority;
    }

    return sched_setscheduler(0, SCHED_RR, &sched_s);
}

static mraa_result_t
mraa_setup_mux_mapped(mraa_pin_t meta)
{
    int mi;
    for (mi = 0; mi < meta.mux_total; mi++) {
        mraa_gpio_context mux_i;
        mux_i = mraa_gpio_init_raw(meta.mux[mi].pin);
        if (mux_i == NULL)
            return MRAA_ERROR_INVALID_HANDLE;
        mraa_gpio_dir(mux_i, MRAA_GPIO_OUT);
        if (mraa_gpio_write(mux_i, meta.mux[mi].value) != MRAA_SUCCESS)
            return MRAA_ERROR_INVALID_RESOURCE;
    }
    return MRAA_SUCCESS;
}

unsigned int
mraa_setup_gpio(int pin)
{
    if (plat == NULL)
        return -1;

    if (pin < 0 || pin > plat->phy_pin_count)
        return -1;

    if(plat->pins[pin].capabilites.gpio != 1)
      return -1;

    if (plat->pins[pin].gpio.mux_total > 0)
       if (mraa_setup_mux_mapped(plat->pins[pin].gpio) != MRAA_SUCCESS)
            return -1;
    return plat->pins[pin].gpio.pinmap;
}

unsigned int
mraa_setup_aio(int aio)
{
    if (plat == NULL)
        return -3;

    if (aio < 0 || aio > plat->aio_count)
        return -1;

    int pin = aio + plat->gpio_count;

    if (plat->pins[pin].capabilites.aio != 1)
      return -1;

    if (plat->pins[pin].aio.mux_total > 0)
       if (mraa_setup_mux_mapped(plat->pins[pin].aio) != MRAA_SUCCESS)
            return -1;
    return plat->pins[pin].aio.pinmap;
}

unsigned int
mraa_setup_i2c(int bus)
{
    if (plat == NULL)
        return -3;

    if (plat->i2c_bus_count >! 0) {
        fprintf(stderr, "No i2c buses defined in platform");
        return -1;
    }
    if (bus >= plat->i2c_bus_count) {
        fprintf(stderr, "Above i2c bus count");
        return -1;
    }

    int pos = plat->i2c_bus[bus].sda;
    if (plat->pins[pos].i2c.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS)
             return -2;

    pos = plat->i2c_bus[bus].scl;
    if (plat->pins[pos].i2c.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].i2c) != MRAA_SUCCESS)
             return -2;

    return plat->i2c_bus[bus].bus_id;
}

mraa_spi_bus_t*
mraa_setup_spi(int bus)
{
    if (plat == NULL)
        return NULL;

    if (plat->spi_bus_count >! 0) {
        fprintf(stderr, "No spi buses defined in platform");
        return NULL;
    }
    if (bus >= plat->spi_bus_count) {
        fprintf(stderr, "Above spi bus count");
        return NULL;
    }

    int pos = plat->spi_bus[bus].sclk;
    if (plat->pins[pos].spi.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].spi) != MRAA_SUCCESS)
             return NULL;

    pos = plat->spi_bus[bus].mosi;
    if (plat->pins[pos].spi.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].spi) != MRAA_SUCCESS)
             return NULL;

    pos = plat->spi_bus[bus].miso;
    if (plat->pins[pos].spi.mux_total > 0)
        if (mraa_setup_mux_mapped(plat->pins[pos].spi) != MRAA_SUCCESS)
             return NULL;

    mraa_spi_bus_t *spi = &(plat->spi_bus[bus]);
    return spi;
}

mraa_pin_t*
mraa_setup_pwm(int pin)
{
    if (plat == NULL)
        return NULL;

    if (plat->pins[pin].capabilites.pwm != 1)
        return NULL;

    if (plat->pins[pin].capabilites.gpio == 1) {
        mraa_gpio_context mux_i;
        mux_i = mraa_gpio_init_raw(plat->pins[pin].gpio.pinmap);
        if (mux_i == NULL)
            return NULL;
        if (mraa_gpio_dir(mux_i, MRAA_GPIO_OUT) != MRAA_SUCCESS)
            return NULL;
        // Current REV D quirk. //TODO GEN 2
        if (mraa_gpio_write(mux_i, 1) != MRAA_SUCCESS)
            return NULL;
        if (mraa_gpio_close(mux_i) != MRAA_SUCCESS)
            return NULL;
    }

    if (plat->pins[pin].pwm.mux_total > 0)
       if (mraa_setup_mux_mapped(plat->pins[pin].pwm) != MRAA_SUCCESS)
            return NULL;

    mraa_pin_t *ret;
    ret = (mraa_pin_t*) malloc(sizeof(mraa_pin_t));
    ret->pinmap = plat->pins[pin].pwm.pinmap;
    ret->parent_id = plat->pins[pin].pwm.parent_id;
    return ret;
}

void
mraa_result_print(mraa_result_t result)
{
    switch (result) {
        case MRAA_SUCCESS: fprintf(stderr, "MRAA: SUCCESS\n");
                          break;
        case MRAA_ERROR_FEATURE_NOT_IMPLEMENTED:
                          fprintf(stderr, "MRAA: Feature not implemented.\n");
                          break;
        case MRAA_ERROR_FEATURE_NOT_SUPPORTED:
                          fprintf(stderr, "MRAA: Feature not supported by Hardware.\n");
                          break;
        case MRAA_ERROR_INVALID_VERBOSITY_LEVEL:
                          fprintf(stderr, "MRAA: Invalid verbosity level.\n");
                          break;
        case MRAA_ERROR_INVALID_PARAMETER:
                          fprintf(stderr, "MRAA: Invalid parameter.\n");
                          break;
        case MRAA_ERROR_INVALID_HANDLE:
                          fprintf(stderr, "MRAA: Invalid Handle.\n");
                          break;
        case MRAA_ERROR_NO_RESOURCES:
                          fprintf(stderr, "MRAA: No resources.\n");
                          break;
        case MRAA_ERROR_INVALID_RESOURCE:
                          fprintf(stderr, "MRAA: Invalid resource.\n");
                          break;
        case MRAA_ERROR_INVALID_QUEUE_TYPE:
                          fprintf(stderr, "MRAA: Invalid Queue Type.\n");
                          break;
        case MRAA_ERROR_NO_DATA_AVAILABLE:
                          fprintf(stderr, "MRAA: No Data available.\n");
                          break;
        case MRAA_ERROR_INVALID_PLATFORM:
                          fprintf(stderr, "MRAA: Platform not recognised.\n");
                          break;
        case MRAA_ERROR_PLATFORM_NOT_INITIALISED:
                          fprintf(stderr, "MRAA: Platform not initialised.\n");
                          break;
        case MRAA_ERROR_PLATFORM_ALREADY_INITIALISED:
                          fprintf(stderr, "MRAA: Platform already initialised.\n");
                          break;
        case MRAA_ERROR_UNSPECIFIED:
                          fprintf(stderr, "MRAA: Unspecified Error.\n");
                          break;
        default:     fprintf(stderr, "MRAA: Unrecognised error.\n");
                          break;
    }
}

mraa_boolean_t
mraa_pin_mode_test(int pin, mraa_pinmodes_t mode)
{
    if (plat == NULL) {
        mraa_init();
        if (plat == NULL)
            return 0;
    }
    if (pin > plat->phy_pin_count || pin < 0)
        return 0;

    switch(mode) {
        case MRAA_PIN_VALID:
            if (plat->pins[pin].capabilites.valid == 1)
                return 1;
            break;
        case MRAA_PIN_GPIO:
            if (plat->pins[pin].capabilites.gpio ==1)
                return 1;
            break;
        case MRAA_PIN_PWM:
            if (plat->pins[pin].capabilites.pwm ==1)
                return 1;
            break;
        case MRAA_PIN_FAST_GPIO:
            if (plat->pins[pin].capabilites.fast_gpio ==1)
                return 1;
            break;
        case MRAA_PIN_SPI:
            if (plat->pins[pin].capabilites.spi ==1)
                return 1;
            break;
        case MRAA_PIN_I2C:
            if (plat->pins[pin].capabilites.i2c ==1)
                return 1;
            break;
        case MRAA_PIN_AIO:
            if (pin < plat->aio_count)
                pin = pin + plat->gpio_count;
            if (plat->pins[pin].capabilites.aio ==1)
                return 1;
            break;
        default: break;
    }
    return 0;
}

mraa_mmap_pin_t*
mraa_setup_mmap_gpio(int pin)
{
    if (plat == NULL)
        return NULL;

    if (plat->pins[pin].capabilites.fast_gpio != 1)
        return NULL;

    if (plat->pins[pin].mmap.gpio.mux_total > 0)
       if (mraa_setup_mux_mapped(plat->pins[pin].mmap.gpio) != MRAA_SUCCESS)
            return NULL;

    if (mraa_setup_mux_mapped(plat->pins[pin].mmap.gpio) != MRAA_SUCCESS)
            return NULL;
    mraa_mmap_pin_t *ret = &(plat->pins[pin].mmap);
    return ret;
}

mraa_result_t
mraa_swap_complex_gpio(int pin, int out)
{
    if (plat == NULL)
        return MRAA_ERROR_INVALID_PLATFORM;

    switch (platform_type) {
        case MRAA_INTEL_GALILEO_GEN2:
            if (plat->pins[pin].gpio.complex_cap.complex_pin != 1)
                return MRAA_SUCCESS;
            if (plat->pins[pin].gpio.complex_cap.output_en == 1) {
                mraa_gpio_context output_e;
                output_e = mraa_gpio_init_raw(plat->pins[pin].gpio.output_enable);
                if (mraa_gpio_dir(output_e, MRAA_GPIO_OUT) != MRAA_SUCCESS)
                    return MRAA_ERROR_INVALID_RESOURCE;
                int output_val;
                if (plat->pins[pin].gpio.complex_cap.output_en_high == 1)
                    output_val = out;
                else
                    if (out == 1)
                        output_val = 0;
                    else
                        output_val = 1;
                if (mraa_gpio_write(output_e, output_val) != MRAA_SUCCESS)
                    return MRAA_ERROR_INVALID_RESOURCE;
            }
            //if (plat->pins[pin].gpio.complex_cap.pullup_en == 1) {
            //    mraa_gpio_context pullup_e;
            //    pullup_e = mraa_gpio_init_raw(plat->pins[pin].gpio.pullup_enable);
            //    if (mraa_gpio_mode(pullup_e, MRAA_GPIO_HIZ) != MRAA_SUCCESS)
            //        return MRAA_ERROR_INVALID_RESOURCE;
            //}
            break;
        default: return MRAA_SUCCESS;
    }
}

mraa_platform_t mraa_get_platform_type()
{
    return platform_type;
}

unsigned int
mraa_adc_raw_bits()
{
    if (plat == NULL)
        return 0;

    if (plat->aio_count == 0)
        return 0;

    return plat->adc_raw;
}

unsigned int
mraa_adc_supported_bits()
{
    if (plat == NULL)
        return 0;

    if (plat->aio_count == 0)
        return 0;

    return plat->adc_supported;
}

mraa_adv_func*
mraa_get_advance() {
    return advance;
}
