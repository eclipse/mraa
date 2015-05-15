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
#include <pwd.h>
#include <glob.h>


#include "mraa_internal.h"
#include "gpio.h"
#include "version.h"

mraa_board_t* plat = NULL;
static mraa_platform_t platform_type = MRAA_UNKNOWN_PLATFORM;
mraa_adv_func_t* advance_func;

const char*
mraa_get_version()
{
    return gVERSION;
}

mraa_result_t
mraa_set_log_level(int level)
{
    if (level <= 7 && level >= 0) {
        setlogmask(LOG_UPTO(level));
        return MRAA_SUCCESS;
    }
    return MRAA_ERROR_INVALID_PARAMETER;
}

#if (defined SWIGPYTHON) || (defined SWIG)
mraa_result_t
#else
mraa_result_t __attribute__((constructor))
#endif
mraa_init()
{
    if (plat != NULL) {
        return MRAA_ERROR_PLATFORM_ALREADY_INITIALISED;
    }

    uid_t proc_euid = geteuid();
    struct passwd* proc_user = getpwuid(proc_euid);

#ifdef DEBUG
    setlogmask(LOG_UPTO(LOG_DEBUG));
#else
    setlogmask(LOG_UPTO(LOG_NOTICE));
#endif

    openlog("libmraa", LOG_CONS | LOG_PID | LOG_NDELAY, LOG_LOCAL1);
    syslog(LOG_NOTICE, "libmraa version %s initialised by user '%s' with EUID %d",
           mraa_get_version(), (proc_user != NULL) ? proc_user->pw_name : "<unknown>", proc_euid);

#ifdef SWIGPYTHON
    // Initialise python threads, this allows use to grab the GIL when we are
    // required to do so
    Py_InitializeEx(0);
    PyEval_InitThreads();
#endif
    advance_func = (mraa_adv_func_t*) malloc(sizeof(mraa_adv_func_t));
    memset(advance_func, 0, sizeof(mraa_adv_func_t));

#ifdef X86PLAT
    // Use runtime x86 platform detection
    platform_type = mraa_x86_platform();
#elif ARMPLAT
    // Use runtime ARM platform detection
    platform_type = mraa_arm_platform();
#else
#error mraa_ARCH NOTHING
#endif

    if (plat == NULL) {
        printf("mraa: FATAL error, failed to initialise platform\n");
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
    }

    syslog(LOG_INFO, "libmraa initialised for platform '%s' of type %d", mraa_get_platform_name(), platform_type);
    return MRAA_SUCCESS;
}

void
mraa_deinit()
{
    if (plat != NULL) {
        if (plat->pins != NULL) {
            free(plat->pins);
        }
        free(plat);
    }
    closelog();
}

int
mraa_set_priority(const unsigned int priority)
{
    struct sched_param sched_s;

    memset(&sched_s, 0, sizeof(struct sched_param));
    if (priority > sched_get_priority_max(SCHED_RR)) {
        sched_s.sched_priority = sched_get_priority_max(SCHED_RR);
    } else {
        sched_s.sched_priority = priority;
    }

    return sched_setscheduler(0, SCHED_RR, &sched_s);
}

mraa_result_t
mraa_setup_mux_mapped(mraa_pin_t meta)
{
    int mi;

    for (mi = 0; mi < meta.mux_total; mi++) {
        mraa_gpio_context mux_i;
        mux_i = mraa_gpio_init_raw(meta.mux[mi].pin);
        if (mux_i == NULL) {
            return MRAA_ERROR_INVALID_HANDLE;
        }
        // this function will sometimes fail, however this is not critical as
        // long as the write succeeds - Test case galileo gen2 pin2
        mraa_gpio_dir(mux_i, MRAA_GPIO_OUT);
        mraa_gpio_owner(mux_i, 0);

        if (mraa_gpio_write(mux_i, meta.mux[mi].value) != MRAA_SUCCESS) {
            mraa_gpio_close(mux_i);
            return MRAA_ERROR_INVALID_RESOURCE;
        }
        mraa_gpio_close(mux_i);
    }

    return MRAA_SUCCESS;
}

void
mraa_result_print(mraa_result_t result)
{
    switch (result) {
        case MRAA_SUCCESS:
            fprintf(stdout, "MRAA: SUCCESS\n");
            break;
        case MRAA_ERROR_FEATURE_NOT_IMPLEMENTED:
            fprintf(stdout, "MRAA: Feature not implemented.\n");
            break;
        case MRAA_ERROR_FEATURE_NOT_SUPPORTED:
            fprintf(stdout, "MRAA: Feature not supported by Hardware.\n");
            break;
        case MRAA_ERROR_INVALID_VERBOSITY_LEVEL:
            fprintf(stdout, "MRAA: Invalid verbosity level.\n");
            break;
        case MRAA_ERROR_INVALID_PARAMETER:
            fprintf(stdout, "MRAA: Invalid parameter.\n");
            break;
        case MRAA_ERROR_INVALID_HANDLE:
            fprintf(stdout, "MRAA: Invalid Handle.\n");
            break;
        case MRAA_ERROR_NO_RESOURCES:
            fprintf(stdout, "MRAA: No resources.\n");
            break;
        case MRAA_ERROR_INVALID_RESOURCE:
            fprintf(stdout, "MRAA: Invalid resource.\n");
            break;
        case MRAA_ERROR_INVALID_QUEUE_TYPE:
            fprintf(stdout, "MRAA: Invalid Queue Type.\n");
            break;
        case MRAA_ERROR_NO_DATA_AVAILABLE:
            fprintf(stdout, "MRAA: No Data available.\n");
            break;
        case MRAA_ERROR_INVALID_PLATFORM:
            fprintf(stdout, "MRAA: Platform not recognised.\n");
            break;
        case MRAA_ERROR_PLATFORM_NOT_INITIALISED:
            fprintf(stdout, "MRAA: Platform not initialised.\n");
            break;
        case MRAA_ERROR_PLATFORM_ALREADY_INITIALISED:
            fprintf(stdout, "MRAA: Platform already initialised.\n");
            break;
        case MRAA_ERROR_UNSPECIFIED:
            fprintf(stdout, "MRAA: Unspecified Error.\n");
            break;
        default:
            fprintf(stdout, "MRAA: Unrecognised error.\n");
            break;
    }
}

mraa_boolean_t
mraa_pin_mode_test(int pin, mraa_pinmodes_t mode)
{
    if (plat == NULL) {
        return 0;
    }
    if (pin > (plat->phy_pin_count - 1) || pin < 0)
        return 0;

    switch (mode) {
        case MRAA_PIN_VALID:
            if (plat->pins[pin].capabilites.valid == 1)
                return 1;
            break;
        case MRAA_PIN_GPIO:
            if (plat->pins[pin].capabilites.gpio == 1)
                return 1;
            break;
        case MRAA_PIN_PWM:
            if (plat->pins[pin].capabilites.pwm == 1)
                return 1;
            break;
        case MRAA_PIN_FAST_GPIO:
            if (plat->pins[pin].capabilites.fast_gpio == 1)
                return 1;
            break;
        case MRAA_PIN_SPI:
            if (plat->pins[pin].capabilites.spi == 1)
                return 1;
            break;
        case MRAA_PIN_I2C:
            if (plat->pins[pin].capabilites.i2c == 1)
                return 1;
            break;
        case MRAA_PIN_AIO:
            if (plat->pins[pin].capabilites.aio == 1)
                return 1;
            break;
        case MRAA_PIN_UART:
            if (plat->pins[pin].capabilites.uart == 1)
                return 1;
            break;
        default:
            syslog(LOG_NOTICE, "requested pinmode invalid");
            break;
    }
    return 0;
}

mraa_platform_t
mraa_get_platform_type()
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

char*
mraa_get_platform_name()
{
    if (plat == NULL) {
        return NULL;
    }
    return (char*) plat->platform_name;
}

unsigned int
mraa_get_pin_count()
{
    if (plat == NULL) {
        return 0;
    }
    return plat->phy_pin_count;
}

char*
mraa_get_pin_name(int pin)
{
    if (plat == NULL) {
        return NULL;
    }
    if (pin > (plat->phy_pin_count - 1) || pin < 0)
        return NULL;
    return (char*) plat->pins[pin].name;
}

mraa_boolean_t
mraa_file_exist(const char* filename)
{
    glob_t results;
    results.gl_pathc = 0;
    glob(filename, 0, NULL, &results);
    int file_found = results.gl_pathc == 1;
    globfree(&results);
    return file_found;
}

mraa_boolean_t
mraa_file_contains(const char* filename, const char* content)
{
    mraa_boolean_t found = 0;
    if ((filename == NULL) || (content == NULL)) {
        return 0;
    }

    char* file = mraa_file_unglob(filename);
    if (file != NULL) {
        size_t len = 1024;
        char* line = malloc(len);
        FILE* fh = fopen(file, "r");
        while ((getline(&line, &len, fh) != -1) && (found == 0)) {
            if (strstr(line, content)) {
                found = 1;
                break;
            }
        }
        fclose(fh);
        free(file);
        free(line);
    }
    return found;
}

mraa_boolean_t
mraa_file_contains_both(const char* filename, const char* content, const char* content2)
{
    mraa_boolean_t found = 0;
    if ((filename == NULL) || (content == NULL)) {
        return 0;
    }

    char* file = mraa_file_unglob(filename);
    if (file != NULL) {
        size_t len = 1024;
        char* line = malloc(len);
        FILE* fh = fopen(file, "r");
        while ((getline(&line, &len, fh) != -1) && (found == 0)) {
            if (strstr(line, content) && strstr(line, content2)) {
                found = 1;
                break;
            }
        }
        fclose(fh);
        free(file);
        free(line);
    }
    return found;
}

char*
mraa_file_unglob(const char* filename)
{
    glob_t results;
    char* res = NULL;
    results.gl_pathc = 0;
    glob(filename, 0, NULL, &results);
    if (results.gl_pathc == 1)
        res = strdup(results.gl_pathv[0]);
    globfree(&results);
    return res;
}

mraa_boolean_t
mraa_link_targets(const char* filename, const char* targetname)
{
    int size = 100;
    int nchars = 0;
    char* buffer = NULL;
    while (nchars == 0) {
        buffer = (char*) realloc(buffer, size);
        if (buffer == NULL)
            return 0;
        nchars = readlink(filename, buffer, size);
        if (nchars < 0) {
            free(buffer);
            return 0;
        }
        if (nchars >= size) {
            size *= 2;
            nchars = 0;
        }
    }
    if (strstr(buffer, targetname)) {
        free(buffer);
        return 1;
    } else {
        free(buffer);
        return 0;
    }
}
