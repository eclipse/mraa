/*
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

#define _GNU_SOURCE
#if !defined(_XOPEN_SOURCE) || _XOPEN_SOURCE < 600
#define _XOPEN_SOURCE 600 /* Get nftw() and S_IFSOCK declarations */
#endif

#include <stddef.h>
#include <stdlib.h>
#include <sched.h>
#include <string.h>
#include <pwd.h>
#include <glob.h>
#include <ftw.h>
#include <dirent.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>

#include "mraa_internal.h"
#include "gpio.h"
#include "version.h"

#define IIO_DEVICE_WILDCARD "iio:device*"
mraa_board_t* plat = NULL;
mraa_iio_info_t* plat_iio = NULL;

static char* platform_name = NULL;
static char* platform_long_name = NULL;

static int num_i2c_devices = 0;
static int num_iio_devices = 0;

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
        syslog(LOG_DEBUG, "Loglevel %d is set", level);
        return MRAA_SUCCESS;
    }
    syslog(LOG_NOTICE, "Invalid loglevel %d requested", level);
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

    mraa_platform_t platform_type;
#if defined(X86PLAT)
    // Use runtime x86 platform detection
    platform_type = mraa_x86_platform();
#elif defined(ARMPLAT)
    // Use runtime ARM platform detection
    platform_type = mraa_arm_platform();
#else
#error mraa_ARCH NOTHING
#endif

    if (plat != NULL) {
        plat->platform_type = platform_type;
    } else {
        platform_name = NULL;
    }

#if defined(USBPLAT)
    // This is a platform extender so create null base platform if one doesn't already exist
    if (plat == NULL) {
        plat = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
        if (plat != NULL) {
            plat->platform_type = MRAA_NULL_PLATFORM;
            plat->platform_name = "Unknown platform";
        }
    }
    // Now detect sub platform
    if (plat != NULL) {
        mraa_platform_t usb_platform_type = mraa_usb_platform_extender(plat);
        if (plat->platform_type == MRAA_UNKNOWN_PLATFORM && usb_platform_type != MRAA_UNKNOWN_PLATFORM) {
            plat->platform_type = usb_platform_type;
        } else {
            return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
        }
    }
    if (plat == NULL) {
        printf("mraa: FATAL error, failed to initialise platform\n");
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
    }
#endif

    // Look for IIO devices
    mraa_iio_detect();

    if (plat != NULL) {
        int length = strlen(plat->platform_name) + 1;
        if (mraa_has_sub_platform()) {
            length += strlen(plat->sub_platform->platform_name);
        }
        platform_name = calloc(length, sizeof(char));
        if (mraa_has_sub_platform()) {
            snprintf(platform_name, length, "%s + %s", plat->platform_name, plat->sub_platform->platform_name);
        } else {
            strncpy(platform_name, plat->platform_name, length);
        }
    }

    syslog(LOG_NOTICE, "libmraa initialised for platform '%s' of type %d", mraa_get_platform_name(), mraa_get_platform_type());
    return MRAA_SUCCESS;
}

void
mraa_deinit()
{
    if (plat != NULL) {
        if (plat->pins != NULL) {
            free(plat->pins);
        }
        mraa_board_t* sub_plat = plat->sub_platform;
        if (sub_plat != NULL) {
            if (sub_plat->pins != NULL) {
                free(sub_plat->pins);
            }
            free(sub_plat);
        }
        free(plat);

    }
    if (plat_iio != NULL) {
        free(plat_iio);
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

static int
mraa_count_iio_devices(const char* path, const struct stat* sb, int flag, struct FTW* ftwb)
{
    // we are only interested in files with specific names
    if (fnmatch(IIO_DEVICE_WILDCARD, basename(path), 0) == 0) {
        num_iio_devices++;
    }
    return 0;
}

mraa_result_t
mraa_iio_detect()
{
    plat_iio = (mraa_iio_info_t*) calloc(1, sizeof(mraa_iio_info_t));
    plat_iio->iio_device_count = num_iio_devices;
    // Now detect IIO devices, linux only
    // find how many iio devices we have if we haven't already
    if (num_iio_devices == 0) {
        if (nftw("/sys/bus/iio/devices", &mraa_count_iio_devices, 20, FTW_PHYS) == -1) {
            return MRAA_ERROR_UNSPECIFIED;
        }
    }
    char name[64], filepath[64];
    int fd, len, i;
    plat_iio->iio_device_count = num_iio_devices;
    plat_iio->iio_devices = calloc(num_iio_devices, sizeof(struct _iio));
    struct _iio* device;
    for (i=0; i < num_iio_devices; i++) {
        device = &plat_iio->iio_devices[i];
        device->num = i;
        snprintf(filepath, 64, "/sys/bus/iio/devices/iio:device%d/name", i);
        fd = open(filepath, O_RDONLY);
        if (fd != -1) {
            len = read(fd, &name, 64);
            if (len > 1) {
                // remove any trailing CR/LF symbols
                name[strcspn(name, "\r\n")] = '\0';
                len = strlen(name);
                // use strndup
                device->name = malloc((sizeof(char) * len) + sizeof(char));
                strncpy(device->name, name, len+1);
            }
            close(fd);
        }
    }
    return MRAA_SUCCESS;
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
mraa_has_sub_platform()
{
    return (plat != NULL) && (plat->sub_platform != NULL);
}

mraa_boolean_t
mraa_pin_mode_test(int pin, mraa_pinmodes_t mode)
{
    if (plat == NULL)
        return 0;

    mraa_board_t* current_plat = plat;
    if (mraa_is_sub_platform_id(pin)) {
        current_plat = plat->sub_platform;
        if (current_plat == NULL) {
            syslog(LOG_ERR, "mraa_pin_mode_test: Sub platform Not Initialised");
            return 0;
        }
        pin = mraa_get_sub_platform_index(pin);
    }

    if (current_plat == NULL || current_plat->platform_type == MRAA_UNKNOWN_PLATFORM) {
        return 0;
    }
    if (pin > (current_plat->phy_pin_count - 1) || pin < 0)
        return 0;

    switch (mode) {
        case MRAA_PIN_VALID:
            if (current_plat->pins[pin].capabilites.valid == 1)
                return 1;
            break;
        case MRAA_PIN_GPIO:
            if (current_plat->pins[pin].capabilites.gpio == 1)
                return 1;
            break;
        case MRAA_PIN_PWM:
            if (current_plat->pins[pin].capabilites.pwm == 1)
                return 1;
            break;
        case MRAA_PIN_FAST_GPIO:
            if (current_plat->pins[pin].capabilites.fast_gpio == 1)
                return 1;
            break;
        case MRAA_PIN_SPI:
            if (current_plat->pins[pin].capabilites.spi == 1)
                return 1;
            break;
        case MRAA_PIN_I2C:
            if (current_plat->pins[pin].capabilites.i2c == 1)
                return 1;
            break;
        case MRAA_PIN_AIO:
            if (current_plat->pins[pin].capabilites.aio == 1)
                return 1;
            break;
        case MRAA_PIN_UART:
            if (current_plat->pins[pin].capabilites.uart == 1)
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
    if (plat == NULL)
        return MRAA_UNKNOWN_PLATFORM;
    return plat->platform_type;
}

int
mraa_get_platform_combined_type()
{
    int type = mraa_get_platform_type();
    int sub_type = mraa_has_sub_platform() ? plat->sub_platform->platform_type : MRAA_UNKNOWN_PLATFORM;
    return type | (sub_type << 8);
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
mraa_get_platform_adc_raw_bits(uint8_t platform_offset)
{
    if (platform_offset == MRAA_MAIN_PLATFORM_OFFSET)
        return mraa_adc_raw_bits();
    else {
        if (!mraa_has_sub_platform())
            return 0;

        if (plat->sub_platform->aio_count == 0)
            return 0;

        return plat->sub_platform->adc_raw;
    }
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

unsigned int
mraa_get_platform_adc_supported_bits(int platform_offset)
{
    if (platform_offset == MRAA_MAIN_PLATFORM_OFFSET)
        return mraa_adc_supported_bits();
    else {
        if (!mraa_has_sub_platform())
            return 0;

        if (plat->sub_platform->aio_count == 0)
            return 0;

        return plat->sub_platform->adc_supported;
    }
}

const char*
mraa_get_platform_name()
{
    return platform_name;
}

const char*
mraa_get_platform_version(int platform_offset)
{
    if (plat == NULL) {
        return NULL;
    }
    if (platform_offset == MRAA_MAIN_PLATFORM_OFFSET) {
        return plat->platform_version;
    } else {
        return plat->sub_platform->platform_version;
    }
}

int
mraa_get_i2c_bus_count()
{
    if (plat == NULL) {
        return -1;
    }
    return plat->i2c_bus_count;
}

int
mraa_get_i2c_bus_id(unsigned i2c_bus)
{
    if (plat == NULL) {
        return -1;
    }

    if (i2c_bus >= plat->i2c_bus_count) {
        return -1;
    }

    return plat->i2c_bus[i2c_bus].bus_id;
}

unsigned int
mraa_get_pin_count()
{
    if (plat == NULL) {
        return 0;
    }
    return plat->phy_pin_count;
}

unsigned int
mraa_get_platform_pin_count(uint8_t platform_offset)
{
    if (platform_offset == MRAA_MAIN_PLATFORM_OFFSET)
        return mraa_get_pin_count();
    else {
        if (mraa_has_sub_platform())
           return plat->sub_platform->phy_pin_count;
        else
           return 0;
    }
}


char*
mraa_get_pin_name(int pin)
{
    if (plat == NULL)
        return 0;

    mraa_board_t* current_plat = plat;
    if (mraa_is_sub_platform_id(pin)) {
        current_plat = plat->sub_platform;
        if (current_plat == NULL) {
            syslog(LOG_ERR, "mraa_get_pin_name: Sub platform Not Initialised");
            return 0;
        }
        pin = mraa_get_sub_platform_index(pin);
    }

    if (pin > (current_plat->phy_pin_count - 1) || pin < 0)
        return NULL;
    return (char*) current_plat->pins[pin].name;
}

int
mraa_get_default_i2c_bus(uint8_t platform_offset)
{
    if (plat == NULL)
        return -1;
    if (platform_offset == MRAA_MAIN_PLATFORM_OFFSET) {
        return plat->def_i2c_bus;
    } else {
        if (mraa_has_sub_platform())
           return plat->sub_platform->def_i2c_bus;
        else
           return -1;
    }
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
        char* line = calloc(len, sizeof(char));
        if (line == NULL) {
            free(file);
            return 0;
        }
        FILE* fh = fopen(file, "r");
        if (fh == NULL) {
            free(file);
            free(line);
            return 0;
        }
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
        char* line = calloc(len, sizeof(char));
        if (line == NULL) {
            free(file);
            return 0;
        }
        FILE* fh = fopen(file, "r");
        if (fh == NULL) {
            free(file);
            free(line);
            return 0;
        }
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
        } else {
            buffer[nchars] = '\0';
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

static int
mraa_count_i2c_files(const char* path, const struct stat* sb, int flag, struct FTW* ftwb)
{
    switch (sb->st_mode & S_IFMT) {
        case S_IFLNK:
            num_i2c_devices++;
            break;
    }
    return 0;
}

int
mraa_find_i2c_bus(const char* devname, int startfrom)
{
    char path[64];
    int fd;
    int i = startfrom;
    int ret = -1;

    // because feeding mraa_find_i2c_bus result back into the function is
    // useful treat -1 as 0
    if (startfrom < 0) {
        startfrom = 0;
    }

    // find how many i2c buses we have if we haven't already
    if (num_i2c_devices == 0) {
        if (nftw("/sys/class/i2c-dev/", &mraa_count_i2c_files, 20, FTW_PHYS) == -1) {
            return -1;
        }
    }

    // i2c devices are numbered numerically so 0 must exist otherwise there is
    // no i2c-dev loaded
    if (mraa_file_exist("/sys/class/i2c-dev/i2c-0")) {
        for (i; i < num_i2c_devices; i++) {
            off_t size, err;
            snprintf(path, 64, "/sys/class/i2c-dev/i2c-%u/name", i);
            fd = open(path, O_RDONLY);
            if (fd < 0) {
                break;
            }
            size = lseek(fd, 0, SEEK_END);
            if (size < 0) {
                syslog(LOG_WARNING, "mraa: failed to seek i2c filename file");
                close(fd);
                break;
            }
            err = lseek(fd, 0, SEEK_SET);
            if (err < 0) {
                syslog(LOG_WARNING, "mraa: failed to seek i2c filename file");
                close(fd);
                break;
            }
            char* value = malloc(size);
            if (value == NULL) {
                syslog(LOG_ERR, "mraa: failed to allocate memory for i2c file");
                close(fd);
                break;
            }
            ssize_t r = read(fd, value, size);
            if (r > 0) {
                if (strcasestr(value, devname) != NULL) {
                    free(value);
                    close(fd);
                    return i;
                }
            } else {
                syslog(LOG_ERR, "mraa: sysfs i2cdev failed");
            }
            free(value);
            close(fd);
        }
    } else {
        syslog(LOG_WARNING, "mraa: no i2c-dev detected, load i2c-dev");
    }

    return ret;
}

mraa_boolean_t
mraa_is_sub_platform_id(int pin_or_bus)
{
    return (pin_or_bus & MRAA_SUB_PLATFORM_MASK) != 0;
}

int
mraa_get_sub_platform_id(int pin_or_bus)
{
    return pin_or_bus | MRAA_SUB_PLATFORM_MASK;
}

int
mraa_get_sub_platform_index(int pin_or_bus)
{
    return pin_or_bus & (~MRAA_SUB_PLATFORM_MASK);
}

int
mraa_get_iio_device_count()
{
    return plat_iio->iio_device_count;
}

int
mraa_find_iio_device(const char* devicename)
{
    int i = 0;
    for (i; i < plat_iio->iio_device_count; i++) {
#if 0
        // compare with devices array
        if (!strcmp() {
        }
#endif
    }
    return 0;
}
