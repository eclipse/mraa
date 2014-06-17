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

#include "maa_internal.h"
#include "intel_galileo_rev_d.h"
#include "intel_galileo_rev_g.h"
#include "gpio.h"
#include "version.h"

//static maa_pininfo_t* pindata;
static maa_board_t* plat = NULL;
static maa_platform_t platform_type = 99;

const char *
maa_get_version()
{
    return gVERSION;
}

#ifdef SWIG
maa_result_t
maa_init()
#else
maa_result_t __attribute__((constructor))
maa_init()
#endif
{
    /** Once more board definitions have been added,
     *  A method for detecting them will need to be devised.
     */
    if (plat != NULL) {
        return MAA_ERROR_PLATFORM_ALREADY_INITIALISED;
    }
#ifdef SWIGPYTHON
    // Initialise python threads, this allows use to grab the GIL when we are
    // required to do so
    Py_InitializeEx(0);
    PyEval_InitThreads();
#endif
    platform_type = MAA_UNKNOWN_PLATFORM;

    // detect a galileo gen2 board
    char *line = NULL;
    // let getline allocate memory for *line
    size_t len = 0;
    FILE *fh = fopen("/sys/devices/virtual/dmi/id/board_name", "r");
    if (fh != NULL) {
        if (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "GalileoGen2", 10) == 0) {
                platform_type = MAA_INTEL_GALILEO_GEN2;
            } else {
                platform_type = MAA_INTEL_GALILEO_GEN1;
            }
        }
    }
    free(line);
    fclose(fh);

    switch(platform_type) {
        case MAA_INTEL_GALILEO_GEN2:
            plat = maa_intel_galileo_gen2();
            break;
        default:
            plat = maa_intel_galileo_rev_d();
    }

    return MAA_SUCCESS;
}

int
maa_set_priority(const unsigned int priority)
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

static maa_result_t
maa_setup_mux_mapped(maa_pin_t meta)
{
    int mi;
    for (mi = 0; mi < meta.mux_total; mi++) {
        maa_gpio_context mux_i;
        mux_i = maa_gpio_init_raw(meta.mux[mi].pin);
        if (mux_i == NULL)
            return MAA_ERROR_INVALID_HANDLE;
        if (maa_gpio_dir(mux_i, MAA_GPIO_OUT) != MAA_SUCCESS)
            return MAA_ERROR_INVALID_RESOURCE;
        if (maa_gpio_write(mux_i, meta.mux[mi].value) != MAA_SUCCESS)
            return MAA_ERROR_INVALID_RESOURCE;
    }
    return MAA_SUCCESS;
}

unsigned int
maa_setup_gpio(int pin)
{
    if (plat == NULL)
        return -1;

    if (pin < 0 || pin > plat->phy_pin_count)
        return -1;

    if(plat->pins[pin].capabilites.gpio != 1)
      return -1;

    if (plat->pins[pin].gpio.mux_total > 0)
       if (maa_setup_mux_mapped(plat->pins[pin].gpio) != MAA_SUCCESS)
            return -1;
    return plat->pins[pin].gpio.pinmap;
}

unsigned int
maa_setup_aio(int aio)
{
    if (plat == NULL)
        return -3;

    if (aio < 0 || aio > plat->aio_count)
        return -1;

    int pin = aio + plat->gpio_count;

    if (plat->pins[pin].capabilites.aio != 1)
      return -1;

    if (plat->pins[pin].aio.mux_total > 0)
       if (maa_setup_mux_mapped(plat->pins[pin].aio) != MAA_SUCCESS)
            return -1;
    return plat->pins[pin].aio.pinmap;
}

unsigned int
maa_setup_i2c(int bus)
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
        if (maa_setup_mux_mapped(plat->pins[pos].i2c) != MAA_SUCCESS)
             return -2;

    pos = plat->i2c_bus[bus].scl;
    if (plat->pins[pos].i2c.mux_total > 0)
        if (maa_setup_mux_mapped(plat->pins[pos].i2c) != MAA_SUCCESS)
             return -2;

    return plat->i2c_bus[bus].bus_id;
}

maa_spi_bus_t*
maa_setup_spi(int bus)
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
        if (maa_setup_mux_mapped(plat->pins[pos].spi) != MAA_SUCCESS)
             return NULL;

    pos = plat->spi_bus[bus].mosi;
    if (plat->pins[pos].spi.mux_total > 0)
        if (maa_setup_mux_mapped(plat->pins[pos].spi) != MAA_SUCCESS)
             return NULL;

    pos = plat->spi_bus[bus].miso;
    if (plat->pins[pos].spi.mux_total > 0)
        if (maa_setup_mux_mapped(plat->pins[pos].spi) != MAA_SUCCESS)
             return NULL;

    maa_spi_bus_t *spi = &(plat->spi_bus[bus]);
    return spi;
}

maa_pin_t*
maa_setup_pwm(int pin)
{
    if (plat == NULL)
        return NULL;

    if (plat->pins[pin].capabilites.pwm != 1)
        return NULL;

    if (plat->pins[pin].capabilites.gpio == 1) {
        maa_gpio_context mux_i;
        mux_i = maa_gpio_init_raw(plat->pins[pin].gpio.pinmap);
        if (mux_i == NULL)
            return NULL;
        if (maa_gpio_dir(mux_i, MAA_GPIO_OUT) != MAA_SUCCESS)
            return NULL;
        // Current REV D quirk. //TODO GEN 2
        if (maa_gpio_write(mux_i, 1) != MAA_SUCCESS)
            return NULL;
        if (maa_gpio_close(mux_i) != MAA_SUCCESS)
            return NULL;
    }

    if (plat->pins[pin].pwm.mux_total > 0)
       if (maa_setup_mux_mapped(plat->pins[pin].pwm) != MAA_SUCCESS)
            return NULL;

    maa_pin_t *ret;
    ret = (maa_pin_t*) malloc(sizeof(maa_pin_t));
    ret->pinmap = plat->pins[pin].pwm.pinmap;
    ret->parent_id = plat->pins[pin].pwm.parent_id;
    return ret;
}

void
maa_result_print(maa_result_t result)
{
    switch (result) {
        case MAA_SUCCESS: fprintf(stderr, "MAA: SUCCESS\n");
                          break;
        case MAA_ERROR_FEATURE_NOT_IMPLEMENTED:
                          fprintf(stderr, "MAA: Feature not implemented.\n");
                          break;
        case MAA_ERROR_FEATURE_NOT_SUPPORTED:
                          fprintf(stderr, "MAA: Feature not supported by Hardware.\n");
                          break;
        case MAA_ERROR_INVALID_VERBOSITY_LEVEL:
                          fprintf(stderr, "MAA: Invalid verbosity level.\n");
                          break;
        case MAA_ERROR_INVALID_PARAMETER:
                          fprintf(stderr, "MAA: Invalid parameter.\n");
                          break;
        case MAA_ERROR_INVALID_HANDLE:
                          fprintf(stderr, "MAA: Invalid Handle.\n");
                          break;
        case MAA_ERROR_NO_RESOURCES:
                          fprintf(stderr, "MAA: No resources.\n");
                          break;
        case MAA_ERROR_INVALID_RESOURCE:
                          fprintf(stderr, "MAA: Invalid resource.\n");
                          break;
        case MAA_ERROR_INVALID_QUEUE_TYPE:
                          fprintf(stderr, "MAA: Invalid Queue Type.\n");
                          break;
        case MAA_ERROR_NO_DATA_AVAILABLE:
                          fprintf(stderr, "MAA: No Data available.\n");
                          break;
        case MAA_ERROR_INVALID_PLATFORM:
                          fprintf(stderr, "MAA: Platform not recognised.\n");
                          break;
        case MAA_ERROR_PLATFORM_NOT_INITIALISED:
                          fprintf(stderr, "MAA: Platform not initialised.\n");
                          break;
        case MAA_ERROR_PLATFORM_ALREADY_INITIALISED:
                          fprintf(stderr, "MAA: Platform already initialised.\n");
                          break;
        case MAA_ERROR_UNSPECIFIED:
                          fprintf(stderr, "MAA: Unspecified Error.\n");
                          break;
        default:     fprintf(stderr, "MAA: Unrecognised error.\n");
                          break;
    }
}

maa_boolean_t
maa_pin_mode_test(int pin, maa_pinmodes_t mode)
{
    if (plat == NULL) {
        maa_init();
        if (plat == NULL)
            return 0;
    }
    if (pin > plat->phy_pin_count || pin < 0)
        return 0;

    switch(mode) {
        case MAA_PIN_VALID:
            if (plat->pins[pin].capabilites.valid == 1)
                return 1;
            break;
        case MAA_PIN_GPIO:
            if (plat->pins[pin].capabilites.gpio ==1)
                return 1;
            break;
        case MAA_PIN_PWM:
            if (plat->pins[pin].capabilites.pwm ==1)
                return 1;
            break;
        case MAA_PIN_FAST_GPIO:
            if (plat->pins[pin].capabilites.fast_gpio ==1)
                return 1;
            break;
        case MAA_PIN_SPI:
            if (plat->pins[pin].capabilites.spi ==1)
                return 1;
            break;
        case MAA_PIN_I2C:
            if (plat->pins[pin].capabilites.i2c ==1)
                return 1;
            break;
        case MAA_PIN_AIO:
            if (pin < plat->aio_count)
                pin = pin + plat->gpio_count;
            if (plat->pins[pin].capabilites.aio ==1)
                return 1;
            break;
        default: break;
    }
    return 0;
}

maa_mmap_pin_t*
maa_setup_mmap_gpio(int pin)
{
    if (plat == NULL)
        return NULL;

    if (plat->pins[pin].capabilites.fast_gpio != 1)
        return NULL;

    if (plat->pins[pin].mmap.gpio.mux_total > 0)
       if (maa_setup_mux_mapped(plat->pins[pin].mmap.gpio) != MAA_SUCCESS)
            return NULL;

    if (maa_setup_mux_mapped(plat->pins[pin].mmap.gpio) != MAA_SUCCESS)
            return NULL;
    maa_mmap_pin_t *ret = &(plat->pins[pin].mmap);
    return ret;
}

maa_result_t
maa_swap_complex_gpio(int pin, int out)
{
    if (plat == NULL)
        return MAA_ERROR_INVALID_PLATFORM;

    printf("SWAP CALLED on %i with bool as %i", pin,out);

    switch (platform_type) {
        case MAA_INTEL_GALILEO_GEN2:
            printf("Intel Galileo Gen 2\n");
            if (plat->pins[pin].gpio.complex_cap.complex_pin != 1)
                return MAA_SUCCESS;
            if (plat->pins[pin].gpio.complex_cap.output_en == 1) {
                maa_gpio_context output_e;
                printf("Doing stuff here with %i", plat->pins[pin].gpio.output_enable);
                output_e = maa_gpio_init_raw(plat->pins[pin].gpio.output_enable);
                if (maa_gpio_dir(output_e, MAA_GPIO_OUT) != MAA_SUCCESS)
                    return MAA_ERROR_INVALID_RESOURCE;
                int output_val;
                if (plat->pins[pin].gpio.complex_cap.output_en_high == 1)
                    output_val = out;
                else
                    if (out == 1)
                        output_val = 0;
                    else
                        output_val = 1;
                if (maa_gpio_write(output_e, output_val) != MAA_SUCCESS)
                    return MAA_ERROR_INVALID_RESOURCE;
            }
            //if (plat->pins[pin].gpio.complex_cap.pullup_en == 1) {
            //    maa_gpio_context pullup_e;
            //    pullup_e = maa_gpio_init_raw(plat->pins[pin].gpio.pullup_enable);
            //    if (maa_gpio_mode(pullup_e, MAA_GPIO_HIZ) != MAA_SUCCESS)
            //        return MAA_ERROR_INVALID_RESOURCE;
            //}
            break;
        default: return MAA_SUCCESS;
    }
}
