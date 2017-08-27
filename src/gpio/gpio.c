/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
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
#include "gpio.h"
#include "mraa_internal.h"
#include "linux/gpio.h"
#include <dirent.h>

#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <errno.h>
#include <sys/ioctl.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

typedef struct {
    int chip_fd;
    struct gpiochip_info chip_info;
} mraa_gpiod_chip_info;

typedef struct gpioline_info mraa_gpiod_line_info;

mraa_gpiod_chip_info* mraa_get_chip_info_by_path(const char *path);
mraa_gpiod_chip_info* mraa_get_chip_info_by_name(const char *name);
mraa_gpiod_chip_info* mraa_get_chip_info_by_label(const char *label);
mraa_gpiod_chip_info* mraa_get_chip_info_by_number(unsigned number);

mraa_gpiod_line_info* mraa_get_line_info_by_chip_number(unsigned chip_number, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_name(const char* chip_name, unsigned line_number);
mraa_gpiod_line_info* mraa_get_line_info_by_chip_label(const char* chip_label, unsigned line_number);

int mraa_get_lines_handle(int chip_fd, unsigned line_offsets[], unsigned num_lines, unsigned flags, unsigned default_value);
int mraa_set_line_values(int line_handle, unsigned int num_lines, unsigned char input_values[]);
int mraa_get_line_values(int line_handle, unsigned int num_lines, unsigned char output_values[]);

/* Multiple gpio support. */
typedef struct _gpio_group* mraa_gpiod_group_t;

int mraa_get_number_of_gpio_chips();
void mraa_free_gpio_groups(mraa_gpio_context dev);

int gpiod_ioctl(int fd, unsigned long gpio_request, void *data)
{
    int status;

    status = ioctl(fd, gpio_request, data);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() error %s", strerror(errno));
    }

    return status;
}

int mraa_get_lines_handle(int chip_fd, unsigned line_offsets[], unsigned num_lines, unsigned flags, unsigned default_value)
{
    int status;
    struct gpiohandle_request __gpio_hreq;

    __gpio_hreq.lines = num_lines;
    memcpy(__gpio_hreq.lineoffsets, line_offsets, num_lines * sizeof __gpio_hreq.lineoffsets[0]);

    if (flags & GPIOHANDLE_REQUEST_OUTPUT) {
        memset(__gpio_hreq.default_values, 0, num_lines * sizeof __gpio_hreq.default_values[0]);
    }
    __gpio_hreq.flags = flags;

    status = gpiod_ioctl(chip_fd, GPIO_GET_LINEHANDLE_IOCTL, &__gpio_hreq);
    if (status < 0) {
        syslog(LOG_ERR, "gpiod: ioctl() fail");
        return status;
    }

    if (__gpio_hreq.fd <= 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid file descriptor");
    }

    return __gpio_hreq.fd;
}

static mraa_result_t
mraa_gpio_get_valfp(mraa_gpio_context dev)
{
    char bu[MAX_SIZE];
    snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
    dev->value_fp = open(bu, O_RDWR);
    if (dev->value_fp == -1) {
        syslog(LOG_ERR, "gpio%i: Failed to open 'value': %s", dev->pin, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_gpio_context
mraa_gpio_init_internal(mraa_adv_func_t* func_table, int pin)
{
    if (pin < 0)
        return NULL;

    mraa_result_t status = MRAA_SUCCESS;

    mraa_gpio_context dev = (mraa_gpio_context) calloc(1, sizeof(struct _gpio));
    if (dev == NULL) {
        syslog(LOG_CRIT, "gpio%i: Failed to allocate memory for context", pin);
        return NULL;
    }

    dev->advance_func = func_table;
    dev->pin = pin;

    if (IS_FUNC_DEFINED(dev, gpio_init_internal_replace)) {
        status = dev->advance_func->gpio_init_internal_replace(dev, pin);
        if (status == MRAA_SUCCESS)
            return dev;
        else
            goto init_internal_cleanup;
    }

    if (IS_FUNC_DEFINED(dev, gpio_init_pre)) {
        status = dev->advance_func->gpio_init_pre(pin);
        if (status != MRAA_SUCCESS)
            goto init_internal_cleanup;
    }

    dev->value_fp = -1;
    dev->isr_value_fp = -1;
#ifndef HAVE_PTHREAD_CANCEL
    dev->isr_control_pipe[0] = dev->isr_control_pipe[1] = -1;
#endif
    dev->isr_thread_terminating = 0;
    dev->phy_pin = -1;

    if (!plat->chardev_capable) {
        char bu[MAX_SIZE];
        int length;

        // then check to make sure the pin is exported.
        char directory[MAX_SIZE];
        snprintf(directory, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/", dev->pin);
        struct stat dir;
        if (stat(directory, &dir) == 0 && S_ISDIR(dir.st_mode)) {
            dev->owner = 0; // Not Owner
        } else {
            int export = open(SYSFS_CLASS_GPIO "/export", O_WRONLY);
            if (export == -1) {
                syslog(LOG_ERR, "gpio%i: init: Failed to open 'export' for writing: %s", pin, strerror(errno));
                status = MRAA_ERROR_INVALID_RESOURCE;
                goto init_internal_cleanup;
            }
            length = snprintf(bu, sizeof(bu), "%d", dev->pin);
            if (write(export, bu, length * sizeof(char)) == -1) {
                syslog(LOG_ERR, "gpio%i: init: Failed to write to 'export': %s", pin, strerror(errno));
                close(export);
                status = MRAA_ERROR_INVALID_RESOURCE;
                goto init_internal_cleanup;
            }
            dev->owner = 1;
            close(export);
        }
    }

init_internal_cleanup:
    if (status != MRAA_SUCCESS) {
        if (dev != NULL)
            free(dev);
        return NULL;
    }
    return dev;
}

mraa_gpio_context
mraa_gpio_init(int pin)
{
    mraa_board_t* board = plat;
    if (board == NULL) {
        syslog(LOG_ERR, "gpio%i: init: platform not initialised", pin);
        return NULL;
    }

    if (mraa_is_sub_platform_id(pin)) {
        syslog(LOG_NOTICE, "gpio%i: init: Using sub platform", pin);
        board = board->sub_platform;
        if (board == NULL) {
            syslog(LOG_ERR, "gpio%i: init: Sub platform not initialised", pin);
            return NULL;
        }
        pin = mraa_get_sub_platform_index(pin);
    }

    if (pin < 0 || pin >= board->phy_pin_count) {
        syslog(LOG_ERR, "gpio: init: pin %i beyond platform pin count (%i)", pin, board->phy_pin_count);
        return NULL;
    }
    if (board->pins[pin].capabilities.gpio != 1) {
        syslog(LOG_ERR, "gpio: init: pin %i not capable of gpio", pin);
        return NULL;
    }
    if (board->pins[pin].gpio.mux_total > 0) {
        if (mraa_setup_mux_mapped(board->pins[pin].gpio) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "gpio%i: init: unable to setup muxes", pin);
            return NULL;
        }
    }

    mraa_gpio_context r = mraa_gpio_init_internal(board->adv_func, board->pins[pin].gpio.pinmap);

    if (r == NULL) {
        return NULL;
    }

    if (board->chardev_capable) {
        mraa_gpiod_chip_info* cinfo = mraa_get_chip_info_by_number(board->pins[pin].gpio.gpio_chip);
        if (!cinfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio chip info");
            free(r);
            return NULL;
        }
        r->dev_fd = cinfo->chip_fd;
        r->gpio_chip = board->pins[pin].gpio.gpio_chip;
        r->gpio_line = board->pins[pin].gpio.gpio_line;
        r->gpiod_handle = -1;
        free(cinfo);
    }

    if (r->phy_pin == -1)
        r->phy_pin = pin;

    if (IS_FUNC_DEFINED(r, gpio_init_post)) {
        mraa_result_t ret = r->advance_func->gpio_init_post(r);
        if (ret != MRAA_SUCCESS) {
            free(r);
            return NULL;
        }
    }

    /* Just one gpio line. */
    r->num_pins = 1;

    return r;
}

mraa_gpio_context mraa_gpio_init_multiple(int pins[], int num_pins) {
    int chip_id, line_offset;
    mraa_gpio_context dev;
    mraa_board_t* board = plat;
    mraa_gpiod_group_t gpio_group;

    if (board == NULL) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: platform not initialised");
        return NULL;
    }

    if (board->chardev_capable) {

        dev = (mraa_gpio_context) calloc(1, sizeof(struct _gpio));
        if (dev == NULL) {
            syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for context");
            return NULL;
        }

        dev->pin_to_gpio_table = malloc(num_pins * sizeof(int));

        dev->num_chips = mraa_get_number_of_gpio_chips();
        if (dev->num_chips <= 0) {
            free(dev);
            return NULL;
        }

        dev->num_pins = num_pins;

        gpio_group = calloc(dev->num_chips, sizeof(struct _gpio_group));
        for (int i = 0; i < dev->num_chips; ++i) {
            gpio_group[i].gpio_chip = i;
            /* Just to be sure realloc has the desired behaviour. */
            gpio_group[i].gpio_lines = NULL;
        }

        for (int i = 0; i < num_pins; ++i) {
            if (mraa_is_sub_platform_id(pins[i])) {
                syslog(LOG_NOTICE, "[GPIOD_INTERFACE]: init: Using sub platform for %d", pins[i]);
                board = board->sub_platform;
                if (board == NULL) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: Sub platform not initialised for pin %d", pins[i]);
                    return NULL;
                }
                pins[i] = mraa_get_sub_platform_index(pins[i]);
            }

            if (pins[i] < 0 || pins[i] >= board->phy_pin_count) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: pin %d beyond platform pin count (%d)", pins[i], board->phy_pin_count);
                return NULL;
            }

            if (board->pins[pins[i]].capabilities.gpio != 1) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: pin %d not capable of gpio", pins[i]);
                return NULL;
            }

            if (board->pins[pins[i]].gpio.mux_total > 0) {
                if (mraa_setup_mux_mapped(board->pins[pins[i]].gpio) != MRAA_SUCCESS) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: unable to setup muxes for pin %d", pins[i]);
                    return NULL;
                }
            }

            chip_id = board->pins[pins[i]].gpio.gpio_chip;
            line_offset = board->pins[pins[i]].gpio.gpio_line;

            /* Map pin to _gpio_group structure. */
            dev->pin_to_gpio_table[i] = chip_id;

            if (!gpio_group[chip_id].is_required) {
                mraa_gpiod_chip_info* cinfo = mraa_get_chip_info_by_number(chip_id);
                if (!cinfo) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio_chip_info for chip %d", chip_id);
                    mraa_free_gpio_groups(dev);
                    free(dev);
                    return NULL;
                }

                gpio_group[chip_id].dev_fd = cinfo->chip_fd;
                gpio_group[chip_id].is_required = 1;
                gpio_group[chip_id].gpiod_handle = -1;

                free(cinfo);
            }

            int line_in_group;
            line_in_group = gpio_group[chip_id].num_gpio_lines;
            gpio_group[chip_id].gpio_lines = realloc(
                gpio_group[chip_id].gpio_lines,
                (line_in_group + 1) * sizeof(unsigned int));

            gpio_group[chip_id].gpio_lines[line_in_group] = line_offset;
            gpio_group[chip_id].num_gpio_lines++;
        }

        /* Initialize rw_values for read / write multiple functions.
        * Also, allocate memory for inverse map: */
        for (int i = 0; i < dev->num_chips; ++i) {
            gpio_group[i].rw_values = calloc(gpio_group[i].num_gpio_lines, sizeof(unsigned char));
            gpio_group[i].gpio_group_to_pins_table = calloc(gpio_group[i].num_gpio_lines, sizeof(int));

            /* Set event handle arrays for all lines contained on a chip to NULL. */
            gpio_group[i].event_handles = NULL;
        }

        /* Finally map the inverse relation between a gpio group and its original pin numbers provided by user. */
        int *counters = calloc(dev->num_chips, sizeof(int));
        for (int i = 0; i < num_pins; ++i) {
            int chip = dev->pin_to_gpio_table[i];
            gpio_group[chip].gpio_group_to_pins_table[counters[chip]] = i;
            counters[chip]++;
        }
        free(counters);

        dev->gpio_group = gpio_group;

        return dev;
    } else {
        mraa_gpio_context head = NULL, current, tmp;

        for (int i = 0; i < num_pins; ++i) {
            tmp = mraa_gpio_init(pins[i]);
            if (!tmp) {
                syslog(LOG_ERR, "mraa_gpio_init_multiple() error initializing pin %i", pins[i]);
                continue;
            }

            if (head == NULL) {
                head = tmp;
                current = tmp;
            } else {
                current->next = tmp;
                current = tmp;
            }

            current->next = NULL;
        }

        return head;
    }
}

mraa_gpio_context
mraa_gpio_init_raw(int pin)
{
    return mraa_gpio_init_internal(plat == NULL ? NULL : plat->adv_func , pin);
}


static mraa_result_t
mraa_gpio_wait_interrupt(int fd
#ifndef HAVE_PTHREAD_CANCEL
        , int control_fd
#endif
        )
{
    unsigned char c;
#ifdef HAVE_PTHREAD_CANCEL
    struct pollfd pfd[1];
#else
    struct pollfd pfd[2];

    if (control_fd < 0) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }
#endif

    if (fd < 0) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    // setup poll on POLLPRI
    pfd[0].fd = fd;
    if (plat->chardev_capable) {
        pfd[0].events = POLLIN;
    } else {
        pfd[0].events = POLLPRI;
    }


    // do an initial read to clear interrupt
    lseek(fd, 0, SEEK_SET);
    read(fd, &c, 1);

#ifdef HAVE_PTHREAD_CANCEL
    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    poll(pfd, 1, -1);
#else
    // setup poll on the controling fd
    pfd[1].fd = control_fd;
    pfd[1].events = 0; //  POLLHUP, POLLERR, and POLLNVAL

    // Wait for it forever or until control fd is closed
    poll(pfd, 2, -1);
#endif

    if (plat->chardev_capable) {
        if (pfd[0].revents & POLLIN) {
            struct gpioevent_data event_data;
            // do a final read to clear interrupt
            read(fd, &event_data, sizeof(event_data));
        }
    } else {
        // do a final read to clear interrupt
        read(fd, &c, 1);
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_wait_interrupt_multiple(int fds[], int num_fds)
{
    struct pollfd pfd[num_fds];
    struct gpioevent_data event_data;

    if (!fds) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    for (int i = 0; i < num_fds; ++i) {
        pfd[i].fd = fds[i];
        pfd[i].events = POLLIN;

        lseek(fds[i], 0, SEEK_SET);

        read(fds[i], &event_data, sizeof(event_data));
    }

    poll(pfd, num_fds, -1);

    for (int i = 0; i < num_fds; ++i) {
        if (pfd[i].revents & POLLIN) {
            read(fds[i], &event_data, sizeof(event_data));
        }
    }

    return MRAA_SUCCESS;
}

static void*
mraa_gpio_interrupt_handler(void* arg)
{
    if (arg == NULL) {
        syslog(LOG_ERR, "gpio: interrupt_handler: context is invalid");
        return NULL;
    }

    mraa_gpio_context dev = (mraa_gpio_context) arg;
    int fp;
    mraa_result_t ret;

    if (IS_FUNC_DEFINED(dev, gpio_interrupt_handler_init_replace)) {
        if (dev->advance_func->gpio_interrupt_handler_init_replace(dev) != MRAA_SUCCESS)
            return NULL;
    } else {
        if (plat->chardev_capable) {
            fp = dev->gpiod_handle;
        } else {
            // open gpio value with open(3)
            char bu[MAX_SIZE];
            snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", dev->pin);
            fp = open(bu, O_RDONLY);
            if (fp < 0) {
                syslog(LOG_ERR, "gpio%i: interrupt_handler: failed to open 'value' : %s", dev->pin, strerror(errno));
                return NULL;
            }
        }
    }

#ifndef HAVE_PTHREAD_CANCEL
    if (pipe(dev->isr_control_pipe)) {
        syslog(LOG_ERR, "gpio%i: interrupt_handler: failed to create isr control pipe: %s", dev->pin, strerror(errno));
        close(fp);
        return NULL;
    }
#endif

    dev->isr_value_fp = fp;

    if (lang_func->java_attach_thread != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            if (lang_func->java_attach_thread() != MRAA_SUCCESS) {
                close(dev->isr_value_fp);
                dev->isr_value_fp = -1;
                return NULL;
            }
        }
    }

    for (;;) {
        if (IS_FUNC_DEFINED(dev, gpio_wait_interrupt_replace)) {
            ret = dev->advance_func->gpio_wait_interrupt_replace(dev);
        } else {
            ret = mraa_gpio_wait_interrupt(dev->isr_value_fp
#ifndef HAVE_PTHREAD_CANCEL
                , dev->isr_control_pipe[0]
#endif
                );
        }
        if (ret == MRAA_SUCCESS && !dev->isr_thread_terminating) {
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
            if (lang_func->python_isr != NULL) {
                lang_func->python_isr(dev->isr, dev->isr_args);
            } else {
                dev->isr(dev->isr_args);
            }
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif
        } else {
            // we must have got an error code or exit request so die nicely
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
            if (fp != -1) {
                close(dev->isr_value_fp);
                dev->isr_value_fp = -1;
            }

            if (lang_func->java_detach_thread != NULL && lang_func->java_delete_global_ref != NULL) {
                if (dev->isr == lang_func->java_isr_callback) {
                    lang_func->java_delete_global_ref(dev->isr_args);
                    lang_func->java_detach_thread();
                }
            }

            return NULL;
        }
    }
}

/* In it's simplest form, for now. */
static void*
mraa_gpio_interrupt_handler_multiple(void* arg)
{
    if (arg == NULL) {
        syslog(LOG_ERR, "gpio: interrupt_handler: context is invalid");
        return NULL;
    }

    mraa_gpio_context dev = (mraa_gpio_context) arg;
    mraa_gpiod_group_t gpio_group;
    mraa_result_t ret;
    int idx = 0;

    int *fps = malloc(dev->num_pins * sizeof(int));
    if (!fps) {
        syslog(LOG_ERR, "mraa_gpio_interrupt_handler_multiple() malloc error");
        return NULL;
    }

    for_each_gpio_group(gpio_group, dev) {
        for (int i = 0; i < gpio_group->num_gpio_lines; ++i) {
            fps[idx++] = gpio_group->event_handles[i];
        }
    }

    for (;;) {
        ret = mraa_gpio_wait_interrupt_multiple(fps, idx);

        if (ret == MRAA_SUCCESS && !dev->isr_thread_terminating) {
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
            dev->isr(dev->isr_args);
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_ENABLE, NULL);
#endif
        } else {
#ifdef HAVE_PTHREAD_CANCEL
            pthread_setcancelstate(PTHREAD_CANCEL_DISABLE, NULL);
#endif
            if (fps) {
                for (int i = 0; i < idx; ++i) {
                    close(fps[i]);
                }
                free(fps);
            }

            return NULL;
        }
    }
}

mraa_result_t
mraa_gpio_edge_mode(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: edge_mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_edge_mode_replace))
        return dev->advance_func->gpio_edge_mode_replace(dev, mode);

    if (plat->chardev_capable) {
        int status;

        if (dev->gpiod_handle != -1) {
            close(dev->gpiod_handle);
            dev->gpiod_handle = -1;
        }

        struct gpioevent_request req;

        switch (mode) {
            case MRAA_GPIO_EDGE_NONE:
                return MRAA_SUCCESS;
            case MRAA_GPIO_EDGE_BOTH:
                req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
                break;
            case MRAA_GPIO_EDGE_RISING:
                req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
                break;
            case MRAA_GPIO_EDGE_FALLING:
                req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
                break;
            default:
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }


        req.lineoffset = dev->gpio_line;
        req.handleflags = GPIOHANDLE_REQUEST_INPUT;
        status = gpiod_ioctl(dev->dev_fd, GPIO_GET_LINEEVENT_IOCTL, &req);
        if (status < 0) {
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }

        dev->gpiod_handle = req.fd;
    } else {
        if (dev->value_fp != -1) {
            close(dev->value_fp);
            dev->value_fp = -1;
        }

        char filepath[MAX_SIZE];
        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", dev->pin);

        int edge = open(filepath, O_RDWR);
        if (edge == -1) {
            syslog(LOG_ERR, "gpio%i: edge_mode: Failed to open 'edge' for writing: %s", dev->pin, strerror(errno));
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        char bu[MAX_SIZE];
        int length;
        switch (mode) {
            case MRAA_GPIO_EDGE_NONE:
                length = snprintf(bu, sizeof(bu), "none");
                break;
            case MRAA_GPIO_EDGE_BOTH:
                length = snprintf(bu, sizeof(bu), "both");
                break;
            case MRAA_GPIO_EDGE_RISING:
                length = snprintf(bu, sizeof(bu), "rising");
                break;
            case MRAA_GPIO_EDGE_FALLING:
                length = snprintf(bu, sizeof(bu), "falling");
                break;
            default:
                close(edge);
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }
        if (write(edge, bu, length * sizeof(char)) == -1) {
            syslog(LOG_ERR, "gpio%i: edge_mode: Failed to write to 'edge': %s", dev->pin, strerror(errno));
            close(edge);
            return MRAA_ERROR_UNSPECIFIED;
        }

        close(edge);
    }

    return MRAA_SUCCESS;
}

mraa_result_t mraa_gpio_edge_mode_multiple(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: edge_mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (!plat->chardev_capable) {
        syslog(LOG_ERR, "mraa_gpio_isr_multiple() not supported for old sysfs interface");
        return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    int status;
    mraa_gpiod_group_t gpio_group;

    struct gpioevent_request req;

    switch (mode) {
        case MRAA_GPIO_EDGE_NONE:
            return MRAA_SUCCESS;
        case MRAA_GPIO_EDGE_BOTH:
            req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            break;
        case MRAA_GPIO_EDGE_RISING:
            req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
            break;
        case MRAA_GPIO_EDGE_FALLING:
            req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            break;
        default:
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    for_each_gpio_group(gpio_group, dev) {
        if (gpio_group->gpiod_handle != -1) {
            close(gpio_group->gpiod_handle);
            gpio_group->gpiod_handle = -1;
        }

        gpio_group->event_handles = malloc(gpio_group->num_gpio_lines * sizeof(int));
        if (!gpio_group->event_handles) {
            syslog(LOG_ERR, "mraa_gpio_edge_mode_multiple(): malloc error!");
            return MRAA_ERROR_NO_RESOURCES;
        }

        for (int i = 0; i < gpio_group->num_gpio_lines; ++i) {
            req.lineoffset = gpio_group->gpio_lines[i];
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;

            status = gpiod_ioctl(gpio_group->dev_fd, GPIO_GET_LINEEVENT_IOCTL, &req);
            if (status < 0) {
                syslog(LOG_ERR, "error getting line event handle for line %i", gpio_group->gpio_lines[i]);
                return MRAA_ERROR_INVALID_RESOURCE;
            }

            gpio_group->event_handles[i] = req.fd;
        }
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_isr(mraa_gpio_context dev, mraa_gpio_edge_t mode, void (*fptr)(void*), void* args)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: isr: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_isr_replace)) {
        return dev->advance_func->gpio_isr_replace(dev, mode, fptr, args);
    }

    // we only allow one isr per mraa_gpio_context
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    mraa_result_t ret = mraa_gpio_edge_mode(dev, mode);
    if (ret != MRAA_SUCCESS) {
        return ret;
    }

    dev->isr = fptr;

    /* Most UPM sensors use the C API, the Java global ref must be created here. */
    /* The reason for checking the callback function is internal callbacks. */
    if (lang_func->java_create_global_ref != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            args = lang_func->java_create_global_ref(args);
        }
    }

    dev->isr_args = args;

    pthread_create(&dev->thread_id, NULL, mraa_gpio_interrupt_handler, (void*) dev);

    return MRAA_SUCCESS;
}

mraa_result_t mraa_gpio_isr_multiple(mraa_gpio_context dev, mraa_gpio_edge_t mode, void (*fptr)(void*), void* args)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: isr: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    // we only allow one isr per mraa_gpio_context
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    //TODO: Check that this function is applied only when multiple lines are given as input in the init phase.

    mraa_result_t ret = mraa_gpio_edge_mode_multiple(dev, mode);
    if (ret != MRAA_SUCCESS) {
        return ret;
    }

    dev->isr = fptr;

    /* Most UPM sensors use the C API, the Java global ref must be created here. */
    /* The reason for checking the callback function is internal callbacks. */
    if (lang_func->java_create_global_ref != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            args = lang_func->java_create_global_ref(args);
        }
    }

    dev->isr_args = args;

    pthread_create(&dev->thread_id, NULL, mraa_gpio_interrupt_handler_multiple, (void*) dev);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_isr_exit(mraa_gpio_context dev)
{
    mraa_result_t ret = MRAA_SUCCESS;

    if (dev == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_isr_exit_replace)) {
        return dev->advance_func->gpio_isr_exit_replace(dev);
    }

    // wasting our time, there is no isr to exit from
    if (dev->thread_id == 0 && dev->isr_value_fp == -1) {
        return ret;
    }
    // mark the beginning of the thread termination process for interested parties
    dev->isr_thread_terminating = 1;

    // stop isr being useful
    ret = mraa_gpio_edge_mode(dev, MRAA_GPIO_EDGE_NONE);

    if ((dev->thread_id != 0)) {
#ifdef HAVE_PTHREAD_CANCEL
        if ((pthread_cancel(dev->thread_id) != 0) || (pthread_join(dev->thread_id, NULL) != 0)) {
            ret = MRAA_ERROR_INVALID_RESOURCE;
        }
#else
        close(dev->isr_control_pipe[1]);
        if (pthread_join(dev->thread_id, NULL) != 0)
            ret = MRAA_ERROR_INVALID_RESOURCE;

        close(dev->isr_control_pipe[0]);
        dev->isr_control_pipe[0] =  dev->isr_control_pipe[1] = -1;
#endif
    }

    // close the filehandle in case it's still open
    if (dev->isr_value_fp != -1) {
        if (close(dev->isr_value_fp) != 0) {
            ret = MRAA_ERROR_INVALID_RESOURCE;
        }
    }

    // assume our thread will exit either way we just lost it's handle
    dev->thread_id = 0;
    dev->isr_value_fp = -1;
    dev->isr_thread_terminating = 0;

    if (plat->chardev_capable) {
        if (dev->gpiod_handle != -1) {
            close(dev->gpiod_handle);
            dev->gpiod_handle = -1;
        }
    }

    return ret;
}

mraa_result_t
mraa_gpio_mode(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    if (plat->chardev_capable) {
        unsigned flags = 0;
        int line_handle;

        if (dev->gpiod_handle != -1) {
            close(dev->gpiod_handle);
            dev->gpiod_handle = -1;
        }

        mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(dev->gpio_chip, dev->gpio_line);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        flags = linfo->flags;
        free(linfo);

        /* Without changing the API, for now, we can request only one mode per call. */
        switch (mode) {
            case MRAA_GPIOD_ACTIVE_LOW:
                flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;
                break;
            case MRAA_GPIOD_OPEN_DRAIN:
                flags = GPIOHANDLE_REQUEST_OPEN_DRAIN;
                break;
            case MRAA_GPIOD_OPEN_SOURCE:
                flags = GPIOHANDLE_REQUEST_OPEN_SOURCE;
                break;
            default:
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }

        unsigned int line_offsets[1] = {dev->gpio_line};
        line_handle = mraa_get_lines_handle(dev->dev_fd, line_offsets, 1, flags, 0);
        if (line_handle <= 0) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line handle");
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        dev->gpiod_handle = line_handle;
    } else {
        if (IS_FUNC_DEFINED(dev, gpio_mode_replace))
            return dev->advance_func->gpio_mode_replace(dev, mode);

        if (IS_FUNC_DEFINED(dev, gpio_mode_pre)) {
            mraa_result_t pre_ret = (dev->advance_func->gpio_mode_pre(dev, mode));
            if (pre_ret != MRAA_SUCCESS)
                return pre_ret;
        }

        if (dev->value_fp != -1) {
            close(dev->value_fp);
            dev->value_fp = -1;
        }

        char filepath[MAX_SIZE];
        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/drive", dev->pin);

        int drive = open(filepath, O_WRONLY);
        if (drive == -1) {
            syslog(LOG_ERR, "gpio%i: mode: Failed to open 'drive' for writing: %s", dev->pin, strerror(errno));
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        char bu[MAX_SIZE];
        int length;
        switch (mode) {
            case MRAA_GPIO_STRONG:
                length = snprintf(bu, sizeof(bu), "strong");
                break;
            case MRAA_GPIO_PULLUP:
                length = snprintf(bu, sizeof(bu), "pullup");
                break;
            case MRAA_GPIO_PULLDOWN:
                length = snprintf(bu, sizeof(bu), "pulldown");
                break;
            case MRAA_GPIO_HIZ:
                length = snprintf(bu, sizeof(bu), "hiz");
                break;
            default:
                close(drive);
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }
        if (write(drive, bu, length * sizeof(char)) == -1) {
            syslog(LOG_ERR, "gpio%i: mode: Failed to write to 'drive': %s", dev->pin, strerror(errno));
        close(drive);
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        close(drive);
        if (IS_FUNC_DEFINED(dev, gpio_mode_post))
            return dev->advance_func->gpio_mode_post(dev, mode);
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_dir(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: dir: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (plat->chardev_capable) {
        int line_handle;
        unsigned flags = 0;

        if (dev->gpiod_handle != -1) {
            close(dev->gpiod_handle);
            dev->gpiod_handle = -1;
        }

        mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(dev->gpio_chip, dev->gpio_line);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        flags = linfo->flags;
        free(linfo);

        switch (dir) {
            case MRAA_GPIO_OUT:
                flags |= GPIOHANDLE_REQUEST_OUTPUT;
                flags &= ~GPIOHANDLE_REQUEST_INPUT;
                break;
            case MRAA_GPIO_IN:
                flags |= GPIOHANDLE_REQUEST_INPUT;
                flags &= ~GPIOHANDLE_REQUEST_OUTPUT;
                break;
            default:
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }

        unsigned int line_offsets[1] = {dev->gpio_line};
        line_handle = mraa_get_lines_handle(dev->dev_fd, line_offsets, 1, flags, 0);
        if (line_handle <= 0) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line handle");
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        dev->gpiod_handle = line_handle;
    } else {
        if (IS_FUNC_DEFINED(dev, gpio_dir_replace)) {
            return dev->advance_func->gpio_dir_replace(dev, dir);
        }

        if (IS_FUNC_DEFINED(dev, gpio_dir_pre)) {
            mraa_result_t pre_ret = (dev->advance_func->gpio_dir_pre(dev, dir));
            if (pre_ret != MRAA_SUCCESS) {
                return pre_ret;
            }
        }

        if (dev->value_fp != -1) {
            close(dev->value_fp);
            dev->value_fp = -1;
        }
        char filepath[MAX_SIZE];
        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);

        int direction = open(filepath, O_RDWR);

        if (direction == -1) {
            // Direction Failed to Open. If HIGH or LOW was passed will try and set
            // If not fail as usual.
            switch (dir) {
                case MRAA_GPIO_OUT_HIGH:
                    return mraa_gpio_write(dev, 1);
                case MRAA_GPIO_OUT_LOW:
                    return mraa_gpio_write(dev, 0);
                default:
                    syslog(LOG_ERR, "gpio%i: dir: Failed to open 'direction' for writing: %s", dev->pin, strerror(errno));
                    return MRAA_ERROR_INVALID_RESOURCE;
            }
        }

        char bu[MAX_SIZE];
        int length;
        switch (dir) {
            case MRAA_GPIO_OUT:
                length = snprintf(bu, sizeof(bu), "out");
                break;
            case MRAA_GPIO_IN:
                length = snprintf(bu, sizeof(bu), "in");
                break;
            case MRAA_GPIO_OUT_HIGH:
                length = snprintf(bu, sizeof(bu), "high");
                break;
            case MRAA_GPIO_OUT_LOW:
                length = snprintf(bu, sizeof(bu), "low");
                break;
            default:
                close(direction);
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }

        if (write(direction, bu, length * sizeof(char)) == -1) {
            close(direction);
            syslog(LOG_ERR, "gpio%i: dir: Failed to write to 'direction': %s", dev->pin, strerror(errno));
            return MRAA_ERROR_UNSPECIFIED;
        }

        close(direction);
        if (IS_FUNC_DEFINED(dev, gpio_dir_post))
            return dev->advance_func->gpio_dir_post(dev, dir);
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_dir_multiple(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    if (plat->chardev_capable) {
        mraa_gpiod_group_t gpio_group;

        for (int i = 0; i < dev->num_chips; ++i) {

            gpio_group = &dev->gpio_group[i];
            if (!gpio_group->is_required) {
                continue;
            }

            int line_handle;
            unsigned flags = 0;

            if (gpio_group->gpiod_handle != -1) {
                close(gpio_group->gpiod_handle);
                gpio_group->gpiod_handle = -1;
            }

            /* TODO: we can't actually get past line(s) info and meld them all in one request. */
            /*mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(gpio_group->gpio_chip, dev->gpio_line);
            if (!linfo) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
                return MRAA_ERROR_UNSPECIFIED;
            }
            flags |= linfo->flags;
            free(linfo);*/

            switch (dir) {
                case MRAA_GPIO_OUT:
                    flags |= GPIOHANDLE_REQUEST_OUTPUT;
                    flags &= ~GPIOHANDLE_REQUEST_INPUT;
                    break;
                case MRAA_GPIO_IN:
                    flags |= GPIOHANDLE_REQUEST_INPUT;
                    flags &= ~GPIOHANDLE_REQUEST_OUTPUT;
                    break;
                default:
                    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
            }

            line_handle = mraa_get_lines_handle(gpio_group->dev_fd, gpio_group->gpio_lines, gpio_group->num_gpio_lines, flags, 0);
            if (line_handle <= 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line handle");
                return MRAA_ERROR_INVALID_RESOURCE;
            }

            gpio_group->gpiod_handle = line_handle;
        }
    } else {
        mraa_gpio_context it = dev;

        while (it) {
            mraa_gpio_dir(it, dir);
            it = it->next;
        }
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_read_dir(mraa_gpio_context dev, mraa_gpio_dir_t *dir)
{
    mraa_result_t result = MRAA_SUCCESS;

    if (plat->chardev_capable) {
        mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(dev->gpio_chip, dev->gpio_line);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        /* This is overkill for now, but in the future we can extract much more info from a line using the above method. */
        *dir = linfo->flags & GPIOLINE_FLAG_IS_OUT ? MRAA_GPIO_OUT : MRAA_GPIO_IN;
        free(linfo);
    } else {
        char value[5];
        char filepath[MAX_SIZE];
        int fd, rc;

        if (dev == NULL) {
            syslog(LOG_ERR, "gpio: read_dir: context is invalid");
            return MRAA_ERROR_INVALID_HANDLE;
        }

        if (dir == NULL) {
            syslog(LOG_ERR, "gpio: read_dir: output parameter for dir is invalid");
            return MRAA_ERROR_INVALID_HANDLE;
        }

        if (IS_FUNC_DEFINED(dev, gpio_read_dir_replace)) {
            return dev->advance_func->gpio_read_dir_replace(dev, dir);
        }

        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);
        fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            syslog(LOG_ERR, "gpio%i: read_dir: Failed to open 'direction' for reading: %s", dev->pin, strerror(errno));
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        memset(value, '\0', sizeof(value));
        rc = read(fd, value, sizeof(value));
        close(fd);
        if (rc <= 0) {
            syslog(LOG_ERR, "gpio%i: read_dir: Failed to read 'direction': %s", dev->pin, strerror(errno));
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        if (strcmp(value, "out\n") == 0) {
            *dir = MRAA_GPIO_OUT;
        } else if (strcmp(value, "in\n") == 0) {
            *dir = MRAA_GPIO_IN;
        } else {
            syslog(LOG_ERR, "gpio%i: read_dir: unknown direction: %s", dev->pin, value);
            result = MRAA_ERROR_UNSPECIFIED;
        }
    }

    return result;
}

int
mraa_gpio_read(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read: context is invalid");
        return -1;
    }

    if (plat->chardev_capable) {
        int status;
        unsigned flags = 0;

        mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(dev->gpio_chip, dev->gpio_line);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        flags = linfo->flags;
        free(linfo);

        flags = (flags | GPIOHANDLE_REQUEST_INPUT) & ~GPIOHANDLE_REQUEST_OUTPUT;

        if (dev->gpiod_handle <= 0) {
            unsigned int line_offsets[1] = {dev->gpio_line};
            dev->gpiod_handle = mraa_get_lines_handle(dev->dev_fd, line_offsets, 1, flags, 0);
            if (dev->gpiod_handle <= 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                return -1;
            }
        }

        unsigned char values[1] = {0};
        status = mraa_get_line_values(dev->gpiod_handle, 1, values);
        if (status < 0) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error reading gpio");
            return -1;
        }

        return values[0];
    } else {
        if (IS_FUNC_DEFINED(dev, gpio_read_replace))
            return dev->advance_func->gpio_read_replace(dev);

        if (dev->mmap_read != NULL)
            return dev->mmap_read(dev);

        if (dev->value_fp == -1) {
            if (mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
                return -1;
            }
        } else {
            // if value_fp is new this is pointless
            lseek(dev->value_fp, 0, SEEK_SET);
        }
        char bu[2];
        if (read(dev->value_fp, bu, 2 * sizeof(char)) != 2) {
            syslog(LOG_ERR, "gpio%i: read: Failed to read a sensible value from sysfs: %s", dev->pin, strerror(errno));
            return -1;
        }
        lseek(dev->value_fp, 0, SEEK_SET);

        return (int) strtol(bu, NULL, 10);
    }
}

mraa_result_t
mraa_gpio_read_multiple(mraa_gpio_context dev, int output_values[])
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read multiple: context is invalid");
        return -1;
    }

    if (plat->chardev_capable) {
        memset(output_values, 0, dev->num_pins * sizeof(int));

        mraa_gpiod_group_t gpio_group;

        for (int i = 0; i < dev->num_chips; ++i) {
            gpio_group = &dev->gpio_group[i];
            if (!gpio_group->is_required) {
                continue;
            }

            int status;
            unsigned flags = GPIOHANDLE_REQUEST_INPUT;

            if (gpio_group->gpiod_handle <= 0) {
                gpio_group->gpiod_handle = mraa_get_lines_handle(gpio_group->dev_fd, gpio_group->gpio_lines, gpio_group->num_gpio_lines, flags, 0);
                if (gpio_group->gpiod_handle <= 0) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                    return MRAA_ERROR_INVALID_HANDLE;
                }
            }

            status = mraa_get_line_values(gpio_group->gpiod_handle, gpio_group->num_gpio_lines, gpio_group->rw_values);
            if (status < 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error writing gpio");
                return MRAA_ERROR_INVALID_RESOURCE;
            }

            /* Write values back to the user provided array. */
            for (int j = 0; j < gpio_group->num_gpio_lines; ++j) {
                /* Use the internal reverse mapping table. */
                output_values[gpio_group->gpio_group_to_pins_table[j]] = gpio_group->rw_values[j];
            }
        }
    } else {
        mraa_gpio_context it = dev;
        int i = 0;

        while(it) {
            output_values[i] = mraa_gpio_read(it);
            if (output_values[i] == -1) {
                syslog(LOG_ERR, "gpio: read_multiple: failed to read multiple gpio pins");
                return MRAA_ERROR_INVALID_RESOURCE;
            }
            i++;
            it = it->next;
        }
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_write(mraa_gpio_context dev, int value)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: write: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (plat->chardev_capable) {
        int status;
        unsigned flags = 0;

        mraa_gpiod_line_info *linfo = mraa_get_line_info_by_chip_number(dev->gpio_chip, dev->gpio_line);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        flags = linfo->flags;
        free(linfo);

        flags = (flags | GPIOHANDLE_REQUEST_OUTPUT) & ~GPIOHANDLE_REQUEST_INPUT;

        if (dev->gpiod_handle <= 0) {
            unsigned int line_offsets[1] = {dev->gpio_line};
            dev->gpiod_handle = mraa_get_lines_handle(dev->dev_fd, line_offsets, 1, flags, 0);
            if (dev->gpiod_handle <= 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                return -1;
            }
        }

        unsigned char values[1] = {value};
        status = mraa_set_line_values(dev->gpiod_handle, 1, values);
        if (status < 0) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error writing gpio");
            return MRAA_ERROR_INVALID_RESOURCE;
        }
    } else {
        if (dev->mmap_write != NULL)
            return dev->mmap_write(dev, value);

        if (IS_FUNC_DEFINED(dev, gpio_write_pre)) {
            mraa_result_t pre_ret = (dev->advance_func->gpio_write_pre(dev, value));
            if (pre_ret != MRAA_SUCCESS)
                return pre_ret;
        }

        if (IS_FUNC_DEFINED(dev, gpio_write_replace)) {
            return dev->advance_func->gpio_write_replace(dev, value);
        }

        if (dev->value_fp == -1) {
            if (mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
                return MRAA_ERROR_INVALID_RESOURCE;
            }
        }

        if (lseek(dev->value_fp, 0, SEEK_SET) == -1) {
            syslog(LOG_ERR, "gpio%i: write: Failed to lseek 'value': %s", dev->pin, strerror(errno));
            return MRAA_ERROR_UNSPECIFIED;
        }

        char bu[MAX_SIZE];
        int length = snprintf(bu, sizeof(bu), "%d", value);
        if (write(dev->value_fp, bu, length * sizeof(char)) == -1) {
            syslog(LOG_ERR, "gpio%i: write: Failed to write to 'value': %s", dev->pin, strerror(errno));
            return MRAA_ERROR_UNSPECIFIED;
        }

        if (IS_FUNC_DEFINED(dev, gpio_write_post)) {
            return dev->advance_func->gpio_write_post(dev, value);
        }
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_write_multiple(mraa_gpio_context dev, int input_values[])
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: write: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (plat->chardev_capable) {
        mraa_gpiod_group_t gpio_group;

        /* First, let's copy input_values into an internal specific gpio_group structure.
        We copy just values which have the index same as the pin belonging to the gpio_group structure. */
        /* TODO: can move counters to internal memory of strucutre, instead of allocating here all the time. */
        int *counters = calloc(dev->num_chips, sizeof(int));
        for (int i = 0; i < dev->num_pins; ++i) {
            int chip_id = dev->pin_to_gpio_table[i];
            gpio_group = &dev->gpio_group[chip_id];

            gpio_group->rw_values[counters[chip_id]] = input_values[i];
            counters[chip_id]++;
        }
        free(counters);

        for (int i = 0; i < dev->num_chips; ++i) {
            gpio_group = &dev->gpio_group[i];
            if (!gpio_group->is_required) {
                continue;
            }

            int status;
            unsigned flags = GPIOHANDLE_REQUEST_OUTPUT;

            if (gpio_group->gpiod_handle <= 0) {
                gpio_group->gpiod_handle = mraa_get_lines_handle(gpio_group->dev_fd, gpio_group->gpio_lines, gpio_group->num_gpio_lines, flags, 0);
                if (gpio_group->gpiod_handle <= 0) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                    return MRAA_ERROR_INVALID_HANDLE;
                }
            }

            status = mraa_set_line_values(gpio_group->gpiod_handle, gpio_group->num_gpio_lines, gpio_group->rw_values);
            if (status < 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error writing gpio");
                return MRAA_ERROR_INVALID_RESOURCE;
            }
        }
    } else {
        mraa_gpio_context it = dev;
        int i = 0;
        mraa_result_t status;

        while (it) {
            status = mraa_gpio_write(it, input_values[i++]);
            if (status != MRAA_SUCCESS) {
                syslog(LOG_ERR, "gpio: read_multiple: failed to write to multiple gpio pins");
                return status;
            }
            it = it->next;
        }
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_unexport_force(mraa_gpio_context dev)
{
    int unexport = open(SYSFS_CLASS_GPIO "/unexport", O_WRONLY);
    if (unexport == -1) {
        syslog(LOG_ERR, "gpio%i: Failed to open 'unexport' for writing: %s", dev->pin, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length = snprintf(bu, sizeof(bu), "%d", dev->pin);
    if (write(unexport, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "gpio%i: Failed to write to 'unexport': %s", dev->pin, strerror(errno));
        close(unexport);
        return MRAA_ERROR_UNSPECIFIED;
    }

    close(unexport);
    mraa_gpio_isr_exit(dev);
    return MRAA_SUCCESS;
}
static mraa_result_t
mraa_gpio_unexport(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: unexport: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (dev->owner) {
        return mraa_gpio_unexport_force(dev);
    }
    return MRAA_ERROR_INVALID_PARAMETER;
}

mraa_result_t
mraa_gpio_close(mraa_gpio_context dev)
{
    mraa_result_t result = MRAA_SUCCESS;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: close: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_close_replace)) {
        return dev->advance_func->gpio_close_replace(dev);
    }


    if (IS_FUNC_DEFINED(dev, gpio_close_pre)) {
        result = dev->advance_func->gpio_close_pre(dev);
    }

    if (dev->value_fp != -1) {
        close(dev->value_fp);
    }

    if (plat->chardev_capable) {
        if (dev->gpiod_handle != -1) {
            close(dev->gpiod_handle);
        }
        close(dev->dev_fd);
    } else {
        mraa_gpio_unexport(dev);
    }

    free(dev);

    return result;
}

mraa_result_t
mraa_gpio_close_multiple(mraa_gpio_context dev)
{
    mraa_result_t result = MRAA_SUCCESS;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: close: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (plat->chardev_capable) {
        mraa_free_gpio_groups(dev);

        free(dev);
    } else {
        mraa_gpio_context it = dev, tmp;

        while(it) {
            tmp = it->next;
            if (mraa_gpio_close(it) != MRAA_SUCCESS) {
                result = MRAA_ERROR_UNSPECIFIED;
            }
            it = tmp;
        }
    }

    return result;
}

mraa_result_t
mraa_gpio_owner(mraa_gpio_context dev, mraa_boolean_t own)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: owner: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    syslog(LOG_DEBUG, "gpio%i: owner: Set owner to %d", dev->pin, (int) own);
    dev->owner = own;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_use_mmaped(mraa_gpio_context dev, mraa_boolean_t mmap_en)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: use_mmaped: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_mmap_setup)) {
        return dev->advance_func->gpio_mmap_setup(dev, mmap_en);
    }

    syslog(LOG_ERR, "gpio%i: use_mmaped: mmap not implemented on this platform", dev->pin);
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

int
mraa_gpio_get_pin(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: get_pin: context is invalid");
        return -1;
    }
    return dev->phy_pin;
}

int
mraa_gpio_get_pin_raw(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: get_pin: context is invalid");
        return -1;
    }
    return dev->pin;
}

mraa_result_t
mraa_gpio_input_mode(mraa_gpio_context dev, mraa_gpio_input_mode_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: in_mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    char filepath[MAX_SIZE];
    snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/active_low", dev->pin);

    int active_low = open(filepath, O_WRONLY);
    if (active_low == -1) {
        syslog(LOG_ERR, "gpio%i: mode: Failed to open 'active_low' for writing: %s", dev->pin, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    char bu[MAX_SIZE];
    int length;
    switch (mode) {
        case MRAA_GPIO_ACTIVE_HIGH:
            length = snprintf(bu, sizeof(bu), "%d", 0);
            break;
        case MRAA_GPIO_ACTIVE_LOW:
            length = snprintf(bu, sizeof(bu), "%d", 1);
            break;
        default:
            close(active_low);
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    if (write(active_low, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "gpio%i: mode: Failed to write to 'active_low': %s", dev->pin, strerror(errno));
        close(active_low);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    close(active_low);

    return MRAA_SUCCESS;
}


mraa_result_t
mraa_gpio_out_driver_mode(mraa_gpio_context dev, mraa_gpio_out_driver_mode_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: write: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_out_driver_mode_replace)) {
        return dev->advance_func->gpio_out_driver_mode_replace(dev, mode);
    }
    else {
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }
}

/* TODO: GPIOD INTERFACE */

#define DEV_DIR "/dev/"
#define CHIP_DEV_PREFIX "gpiochip"
#define STR_SIZE 64

mraa_gpiod_chip_info* mraa_get_chip_info_by_path(const char *path)
{
    mraa_gpiod_chip_info* cinfo;
    int chip_fd, status;

    if (path == NULL) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid device path");
        return NULL;
    }

    chip_fd = open(path, O_RDWR | O_CLOEXEC);
    if (chip_fd < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: could not open device file %s", path);
        return NULL;
    }

    cinfo = malloc(sizeof *cinfo);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        close(chip_fd);
        return NULL;
    }
    cinfo->chip_fd = chip_fd;

    status = gpiod_ioctl(cinfo->chip_fd, GPIO_GET_CHIPINFO_IOCTL, &cinfo->chip_info);
    if (status < 0) {
        close(cinfo->chip_fd);
        free(cinfo);
        return NULL;
    }

    return cinfo;
}

mraa_gpiod_chip_info* mraa_get_chip_info_by_name(const char *name)
{
    mraa_gpiod_chip_info* cinfo;
    char *full_path;

    /* TODO: check for string lengths first. */

    full_path = malloc(STR_SIZE * sizeof *full_path);
    if (!full_path) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    snprintf(full_path, STR_SIZE, "%s%s", DEV_DIR, name);

    cinfo =  mraa_get_chip_info_by_path(full_path);
    free(full_path);

    return cinfo;
}

mraa_gpiod_chip_info* mraa_get_chip_info_by_label(const char *label)
{
    /* TODO */
    return NULL;
}

mraa_gpiod_chip_info* mraa_get_chip_info_by_number(unsigned number)
{
    mraa_gpiod_chip_info* cinfo;
    char *full_path;

    /* TODO: check for string lengths first. */

    full_path = malloc(STR_SIZE * sizeof *full_path);
    if (!full_path) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    snprintf(full_path, STR_SIZE, "%s%s%u", DEV_DIR, CHIP_DEV_PREFIX, number);

    cinfo =  mraa_get_chip_info_by_path(full_path);
    free(full_path);

    return cinfo;
}

mraa_gpiod_line_info* mraa_get_line_info_from_descriptor(int chip_fd, unsigned line_number)
{
    int status;
    mraa_gpiod_line_info* linfo;

    linfo =  malloc(sizeof *linfo);

    if (!linfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: malloc() fail");
        return NULL;
    }

    linfo->line_offset = line_number;
    status = gpiod_ioctl(chip_fd, GPIO_GET_LINEINFO_IOCTL, linfo);
    if (status < 0) {
        free(linfo);
        return NULL;
    }

    return linfo;
}

mraa_gpiod_line_info* mraa_get_line_info_by_chip_number(unsigned chip_number, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_number(chip_number);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    free(cinfo);

    return linfo;
}

mraa_gpiod_line_info* mraa_get_line_info_by_chip_name(const char* chip_name, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_name(chip_name);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    free(cinfo);

    return linfo;
}

mraa_gpiod_line_info* mraa_get_line_info_by_chip_label(const char* chip_label, unsigned line_number)
{
    mraa_gpiod_line_info* linfo;
    mraa_gpiod_chip_info* cinfo;

    cinfo = mraa_get_chip_info_by_label(chip_label);
    if (!cinfo) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: invalid chip number");
        return NULL;
    }

    linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, line_number);

    free(cinfo);

    return linfo;
}

int mraa_set_line_values(int line_handle, unsigned int num_lines, unsigned char input_values[])
{
    int status;
    struct gpiohandle_data __hdata;

    memcpy(__hdata.values, input_values, num_lines * sizeof(unsigned char));

    status = gpiod_ioctl(line_handle, GPIOHANDLE_SET_LINE_VALUES_IOCTL, &__hdata);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() fail");
    }

    return status;
}

int mraa_get_line_values(int line_handle, unsigned int num_lines, unsigned char output_values[])
{
    int status;
    struct gpiohandle_data __hdata;

    status = gpiod_ioctl(line_handle, GPIOHANDLE_GET_LINE_VALUES_IOCTL, &__hdata);
    if (status < 0) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: ioctl() fail");
        return status;
    }

    memcpy(output_values, __hdata.values, num_lines * sizeof(unsigned char));

    return status;
}

int mraa_get_number_of_gpio_chips()
{
    int num_chips = 0;
    DIR *dev_dir;
    struct dirent *dir;
    const unsigned int len = strlen(CHIP_DEV_PREFIX);

    dev_dir = opendir(DEV_DIR);
    if (dev_dir) {
        while ((dir = readdir(dev_dir)) != NULL) {
            if (!strncmp(dir->d_name, CHIP_DEV_PREFIX, len)) {
                num_chips++;
            }
        }
        closedir(dev_dir);
    } else {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: opendir() error");
        return -1;
    }

    /* Assume opendir() error. */
    return num_chips;
}

void mraa_free_gpio_groups(mraa_gpio_context dev)
{
    mraa_gpiod_group_t gpio_group = dev->gpio_group;

    for (int i = 0; i < dev->num_chips; ++i) {
        if (gpio_group[i].is_required) {
            free(gpio_group[i].gpio_lines);
            free(gpio_group[i].rw_values);
            free(gpio_group[i].gpio_group_to_pins_table);

            if (gpio_group[i].gpiod_handle != -1) {
                close(gpio_group[i].gpiod_handle);
            }

            if (gpio_group[i].event_handles != NULL) {
                for (int j = 0; j < gpio_group[i].num_gpio_lines; ++j) {
                    close(gpio_group[i].event_handles[j]);
                }

                free(gpio_group[i].event_handles);
            }

            close(gpio_group[i].dev_fd);
        }
    }

    free(gpio_group);

    /* Also delete the pin to gpio chip mapping. */
    free(dev->pin_to_gpio_table);
}
