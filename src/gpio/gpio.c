/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2014-2018 Intel Corporation.
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
#include "linux/gpio.h"
#include "mraa_internal.h"
#include "gpio/gpio_chardev.h"

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/time.h>

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define POLL_TIMEOUT

static mraa_result_t
_mraa_gpio_get_valfp(mraa_gpio_context dev)
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

void
mraa_gpio_close_event_handles_sysfs(int fds[], int num_fds)
{
    if ((fds == NULL) || (num_fds <= 0)) {
        syslog(LOG_CRIT, "failed to close and free sysfs event handles");
        return;
    }

    for (int i = 0; i < num_fds; ++i) {
        close(fds[i]);
    }

    free(fds);
}

static mraa_gpio_context
mraa_gpio_init_internal(mraa_adv_func_t* func_table, int pin)
{
    if (pin < 0) {
        return NULL;
    }

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
                syslog(LOG_ERR, "gpio%i: init: Failed to open 'export' for writing: %s",
                       pin, strerror(errno));
                status = MRAA_ERROR_INVALID_RESOURCE;
                goto init_internal_cleanup;
            }
            length = snprintf(bu, sizeof(bu), "%d", dev->pin);
            if (write(export, bu, length * sizeof(char)) == -1) {
                syslog(LOG_ERR, "gpio%i: init: Failed to write to 'export': %s",
                       pin, strerror(errno));
                close(export);
                status = MRAA_ERROR_INVALID_RESOURCE;
                goto init_internal_cleanup;
            }
            dev->owner = 1;
            close(export);
        }
    }

    /* We only have one pin. No need for multiple pin legacy support. */
    dev->num_pins = 1;
    dev->next = NULL;

    /* Initialize events array. */
    dev->events = NULL;

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

    /* If this pin belongs to a subplatform, then the pin member will contain
     * the mraa pin index and the phy_pin will contain the subplatform's
     * pin index.
     *      example:  pin 515, dev->pin = 515, dev->phy_pin = 3
     */
    if (mraa_is_sub_platform_id(pin) && (board->sub_platform != NULL)) {
        syslog(LOG_NOTICE, "gpio%i: initialised on sub platform '%s' physical pin: %i",
                pin,
                board->sub_platform->platform_name != NULL ?
                board->sub_platform->platform_name : "",
                mraa_get_sub_platform_index(pin));

        board = board->sub_platform;
        if (board == NULL) {
            syslog(LOG_ERR, "gpio%i: init: Sub platform not initialised", pin);
            return NULL;
        }
        pin = mraa_get_sub_platform_index(pin);
    }

    if (board->chardev_capable) {
        int pins[1] = { pin };
        return mraa_gpio_init_multi(pins, 1);
    }

    if (pin < 0 || pin >= board->phy_pin_count) {
        syslog(LOG_ERR, "gpio: init: pin %i beyond platform pin count (%i)",
               pin, board->phy_pin_count);
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

    if (r->phy_pin == -1)
        r->phy_pin = pin;

    if (IS_FUNC_DEFINED(r, gpio_init_post)) {
        mraa_result_t ret = r->advance_func->gpio_init_post(r);
        if (ret != MRAA_SUCCESS) {
            free(r);
            return NULL;
        }
    }

    return r;
}

mraa_gpio_context
mraa_gpio_chardev_init(int pins[], int num_pins)
{
    int chip_id, line_offset;
    mraa_gpio_context dev;
    mraa_gpiod_group_t gpio_group;

    mraa_board_t* board = plat;

    dev = (mraa_gpio_context) calloc(1, sizeof(struct _gpio));
    if (dev == NULL) {
        syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for context");
        return NULL;
    }

    dev->pin_to_gpio_table = malloc(num_pins * sizeof(int));
    if (dev->pin_to_gpio_table == NULL) {
        syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
        mraa_gpio_close(dev);
        return NULL;
    }

    dev->num_chips = mraa_get_number_of_gpio_chips();
    if (dev->num_chips <= 0) {
        mraa_gpio_close(dev);
        return NULL;
    }

    dev->num_pins = num_pins;

    gpio_group = calloc(dev->num_chips, sizeof(struct _gpio_group));
    if (gpio_group == NULL) {
        syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
        mraa_gpio_close(dev);
        return NULL;
    }

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
                mraa_gpio_close(dev);
                return NULL;
            }
            pins[i] = mraa_get_sub_platform_index(pins[i]);
        }

        if (pins[i] < 0 || pins[i] >= board->phy_pin_count) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: pin %d beyond platform pin count (%d)",
                   pins[i], board->phy_pin_count);
            mraa_gpio_close(dev);
            return NULL;
        }

        if (board->pins[pins[i]].capabilities.gpio != 1) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: pin %d not capable of gpio", pins[i]);
            mraa_gpio_close(dev);
            return NULL;
        }

        if (board->pins[pins[i]].gpio.mux_total > 0) {
            if (mraa_setup_mux_mapped(board->pins[pins[i]].gpio) != MRAA_SUCCESS) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: unable to setup muxes for pin %d", pins[i]);
                mraa_gpio_close(dev);
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
                mraa_gpio_close(dev);
                return NULL;
            }

            gpio_group[chip_id].dev_fd = cinfo->chip_fd;
            gpio_group[chip_id].is_required = 1;
            gpio_group[chip_id].gpiod_handle = -1;

            free(cinfo);
        }

        int line_in_group;
        line_in_group = gpio_group[chip_id].num_gpio_lines;
        gpio_group[chip_id].gpio_lines = realloc(gpio_group[chip_id].gpio_lines,
                                                 (line_in_group + 1) * sizeof(unsigned int));
        if (gpio_group[chip_id].gpio_lines == NULL) {
            syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
            mraa_gpio_close(dev);
            return NULL;
        }

        gpio_group[chip_id].gpio_lines[line_in_group] = line_offset;
        gpio_group[chip_id].num_gpio_lines++;
    }

    /* Initialize rw_values for read / write multiple functions.
     * Also, allocate memory for inverse map: */
    for (int i = 0; i < dev->num_chips; ++i) {
        gpio_group[i].rw_values = calloc(gpio_group[i].num_gpio_lines, sizeof(unsigned char));
        if (gpio_group[i].rw_values == NULL) {
            syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
            mraa_gpio_close(dev);
            return NULL;
        }

        gpio_group[i].gpio_group_to_pins_table = calloc(gpio_group[i].num_gpio_lines, sizeof(int));
        if (gpio_group[i].gpio_group_to_pins_table == NULL) {
            syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
            mraa_gpio_close(dev);
            return NULL;
        }

        /* Set event handle arrays for all lines contained on a chip to NULL. */
        gpio_group[i].event_handles = NULL;
    }

    /* Finally map the inverse relation between a gpio group and its original pin numbers
     * provided by user. */
    int* counters = calloc(dev->num_chips, sizeof(int));
    if (counters == NULL) {
        syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for local variable");
        mraa_gpio_close(dev);
        return NULL;
    }

    for (int i = 0; i < num_pins; ++i) {
        int chip = dev->pin_to_gpio_table[i];
        gpio_group[chip].gpio_group_to_pins_table[counters[chip]] = i;
        counters[chip]++;
    }
    free(counters);

    dev->gpio_group = gpio_group;

    /* Save the provided array from the user to our internal structure. */
    dev->provided_pins = malloc(dev->num_pins * sizeof(int));
    if (dev->provided_pins == NULL) {
        syslog(LOG_CRIT, "[GPIOD_INTERFACE]: Failed to allocate memory for internal member");
        mraa_gpio_close(dev);
        return NULL;
    }

    memcpy(dev->provided_pins, pins, dev->num_pins * sizeof(int));

    /* Initialize events array. */
    dev->events = NULL;

    return dev;
}

mraa_gpio_context
mraa_gpio_init_multi(int pins[], int num_pins)
{
    mraa_board_t* board = plat;

    if (board == NULL) {
        syslog(LOG_ERR, "[GPIOD_INTERFACE]: init: platform not initialised");
        return NULL;
    }

    if (board->chardev_capable)
        return mraa_gpio_chardev_init(pins, num_pins);

    /* Fallback to legacy interface. */
    mraa_gpio_context head = NULL, current, tmp;

    for (int i = 0; i < num_pins; ++i) {
        tmp = mraa_gpio_init(pins[i]);
        if (!tmp) {
            syslog(LOG_ERR, "mraa_gpio_init_multi() error initializing pin %i", pins[i]);
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

    if (head != NULL) {
        head->num_pins = num_pins;
    }

    return head;
}

mraa_gpio_context
mraa_gpio_init_raw(int pin)
{
    return mraa_gpio_init_internal(plat == NULL ? NULL : plat->adv_func, pin);
}

mraa_timestamp_t
_mraa_gpio_get_timestamp_sysfs()
{
    struct timeval time;
    gettimeofday(&time, NULL);

    return (time.tv_sec * 1e6 + time.tv_usec);
}

static mraa_result_t
mraa_gpio_wait_interrupt(int fds[],
                         int num_fds
#ifndef HAVE_PTHREAD_CANCEL
                         ,
                         int control_fd
#endif
                        ,
                         mraa_gpio_events_t events
)
{
    unsigned char c;
#ifdef HAVE_PTHREAD_CANCEL
    struct pollfd pfd[num_fds];
#else
    struct pollfd pfd[num_fds + 1];

    if (control_fd < 0) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }
#endif

    if (!fds) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    for (int i = 0; i < num_fds; ++i) {
        pfd[i].fd = fds[i];
        // setup poll on POLLPRI
        pfd[i].events = POLLPRI;

        // do an initial read to clear interrupt
        lseek(fds[i], 0, SEEK_SET);
        read(fds[i], &c, 1);
    }

#ifdef HAVE_PTHREAD_CANCEL
    // Wait for it forever or until pthread_cancel
    // poll is a cancelable point like sleep()
    poll(pfd, num_fds, -1);
#else
    // setup poll on the controling fd
    pfd[num_fds].fd = control_fd;
    pfd[num_fds].events = 0; //  POLLHUP, POLLERR, and POLLNVAL

    // Wait for it forever or until control fd is closed
    poll(pfd, num_fds + 1, -1);
#endif

    for (int i = 0; i < num_fds; ++i) {
        if (pfd[i].revents & POLLPRI) {
            read(fds[i], &c, 1);
            events[i].id = i;
            events[i].timestamp = _mraa_gpio_get_timestamp_sysfs();
        } else
            events[i].id = -1;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_gpio_chardev_wait_interrupt(int fds[], int num_fds, mraa_gpio_events_t events)
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
    }

    poll(pfd, num_fds, -1);

    for (int i = 0; i < num_fds; ++i) {
        if (pfd[i].revents & POLLIN) {
            read(fds[i], &event_data, sizeof(event_data));
            events[i].id = i;
            events[i].timestamp = event_data.timestamp;
        } else
            events[i].id = -1;
    }

    return MRAA_SUCCESS;
}

mraa_gpio_events_t
mraa_gpio_get_events(mraa_gpio_context dev)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: mraa_gpio_get_events(): context is invalid");
        return NULL;
    }

    unsigned int event_idx = 0;

    if (plat->chardev_capable) {
        unsigned int pin_idx;
        mraa_gpiod_group_t gpio_iter;

        for_each_gpio_group(gpio_iter, dev) {
            for (int i = 0; i < gpio_iter->num_gpio_lines; ++i) {
                if (dev->events[event_idx].id != -1) {
                    pin_idx = gpio_iter->gpio_group_to_pins_table[i];
                    dev->events[event_idx].id = dev->provided_pins[pin_idx];
                }
                event_idx++;
            }
        }
    } else {
        mraa_gpio_context it = dev;

        while (it) {
            if (dev->events[event_idx].id != -1) {
                dev->events[event_idx].id = it->phy_pin;
            }

            event_idx++;
            it = it->next;
        }
    }

    return dev->events;
}

static void*
mraa_gpio_interrupt_handler(void* arg)
{
    if (arg == NULL) {
        syslog(LOG_ERR, "gpio: interrupt_handler: context is invalid");
        return NULL;
    }

    mraa_result_t ret;
    mraa_gpio_context dev = (mraa_gpio_context) arg;
    int idx = 0;

    if (IS_FUNC_DEFINED(dev, gpio_interrupt_handler_init_replace)) {
        if (dev->advance_func->gpio_interrupt_handler_init_replace(dev) != MRAA_SUCCESS)
            return NULL;
    }

    int *fps = malloc(dev->num_pins * sizeof(int));
    if (!fps) {
        syslog(LOG_ERR, "mraa_gpio_interrupt_handler_multiple() malloc error");
        return NULL;
    }

    /* Is this pin on a subplatform? Do nothing... */
    if (mraa_is_sub_platform_id(dev->pin)) {}
    /* Is the platform chardev_capable? */
    else if (plat->chardev_capable) {
        mraa_gpiod_group_t gpio_group;

        for_each_gpio_group(gpio_group, dev) {
            for (int i = 0; i < gpio_group->num_gpio_lines; ++i) {
                fps[idx++] = gpio_group->event_handles[i];
            }
        }
    }
    /* Else, attempt fs access */
    else {
        mraa_gpio_context it = dev;

        while (it) {
            // open gpio value with open(3)
            char bu[MAX_SIZE];
            snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", it->pin);
            fps[idx] = open(bu, O_RDONLY);
            if (fps[idx] < 0) {
                syslog(LOG_ERR, "gpio%i: interrupt_handler: failed to open 'value' : %s", it->pin,
                       strerror(errno));
                mraa_gpio_close_event_handles_sysfs(fps, idx);
                return NULL;
            }

            idx++;
            it = it->next;
        }
    }

#ifndef HAVE_PTHREAD_CANCEL
    if (pipe(dev->isr_control_pipe)) {
        syslog(LOG_ERR, "gpio%i: interrupt_handler: failed to create isr control pipe: %s",
               dev->pin, strerror(errno));
        mraa_gpio_close_event_handles_sysfs(fps, dev->num_pins);
        return NULL;
    }
#endif

    if (lang_func->java_attach_thread != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            if (lang_func->java_attach_thread() != MRAA_SUCCESS) {
                mraa_gpio_close_event_handles_sysfs(fps, dev->num_pins);
                return NULL;
            }
        }
    }

    for (;;) {
        if (IS_FUNC_DEFINED(dev, gpio_wait_interrupt_replace)) {
            ret = dev->advance_func->gpio_wait_interrupt_replace(dev);
        } else {
            if (plat->chardev_capable) {
                ret = mraa_gpio_chardev_wait_interrupt(fps, idx, dev->events);
            } else {
                ret = mraa_gpio_wait_interrupt(fps, idx
#ifndef HAVE_PTHREAD_CANCEL
                                                ,
                                                dev->isr_control_pipe[0]
#endif
                                                ,
                                                dev->events
                );
            }
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
            mraa_gpio_close_event_handles_sysfs(fps, dev->num_pins);

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

mraa_result_t
mraa_gpio_chardev_edge_mode(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: edge_mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (!plat->chardev_capable) {
        syslog(LOG_ERR, "mraa_gpio_chardev_edge_mode() not supported for old sysfs interface");
        return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    int status;
    mraa_gpiod_group_t gpio_group;

    struct gpioevent_request req;

    switch (mode) {
        case MRAA_GPIO_EDGE_BOTH:
            req.eventflags = GPIOEVENT_REQUEST_BOTH_EDGES;
            break;
        case MRAA_GPIO_EDGE_RISING:
            req.eventflags = GPIOEVENT_REQUEST_RISING_EDGE;
            break;
        case MRAA_GPIO_EDGE_FALLING:
            req.eventflags = GPIOEVENT_REQUEST_FALLING_EDGE;
            break;
        /* Chardev interface doesn't handle EDGE_NONE. */
        case MRAA_GPIO_EDGE_NONE:
        default:
            return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
    }

    for_each_gpio_group(gpio_group, dev)
    {
        if (gpio_group->gpiod_handle != -1) {
            close(gpio_group->gpiod_handle);
            gpio_group->gpiod_handle = -1;
        }

        gpio_group->event_handles = malloc(gpio_group->num_gpio_lines * sizeof(int));
        if (!gpio_group->event_handles) {
            syslog(LOG_ERR, "mraa_gpio_chardev_edge_mode(): malloc error!");
            return MRAA_ERROR_NO_RESOURCES;
        }

        for (int i = 0; i < gpio_group->num_gpio_lines; ++i) {
            req.lineoffset = gpio_group->gpio_lines[i];
            req.handleflags = GPIOHANDLE_REQUEST_INPUT;

            status = _mraa_gpiod_ioctl(gpio_group->dev_fd, GPIO_GET_LINEEVENT_IOCTL, &req);
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
mraa_gpio_edge_mode(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: edge_mode: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, gpio_edge_mode_replace))
        return dev->advance_func->gpio_edge_mode_replace(dev, mode);

    /* Initialize events array. */
    if (dev->events == NULL && mode != MRAA_GPIO_EDGE_NONE) {
        dev->events = malloc(dev->num_pins * sizeof (mraa_gpio_event));
        if (dev->events == NULL) {
            syslog(LOG_ERR, "mraa_gpio_edge_mode() malloc error");
            return MRAA_ERROR_NO_RESOURCES;
        }

        for (int i = 0; i < dev->num_pins; ++i) {
            dev->events[i].id = -1;
        }
    }

    if (plat->chardev_capable)
        return mraa_gpio_chardev_edge_mode(dev, mode);

    mraa_gpio_context it = dev;

    while (it) {

        if (it->value_fp != -1) {
            close(it->value_fp);
            it->value_fp = -1;
        }

        char filepath[MAX_SIZE];
        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/edge", it->pin);

        int edge = open(filepath, O_RDWR);
        if (edge == -1) {
            syslog(LOG_ERR, "gpio%i: edge_mode: Failed to open 'edge' for writing: %s", it->pin,
                   strerror(errno));
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
            syslog(LOG_ERR, "gpio%i: edge_mode: Failed to write to 'edge': %s", it->pin, strerror(errno));
            close(edge);
            return MRAA_ERROR_UNSPECIFIED;
        }

        close(edge);

        it = it->next;
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

    mraa_result_t ret;


    ret = mraa_gpio_edge_mode(dev, mode);

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
    if (dev->thread_id == 0) {
        return ret;
    }
    // mark the beginning of the thread termination process for interested parties
    dev->isr_thread_terminating = 1;

    // stop isr being useful
    if (plat && (plat->chardev_capable))
        _mraa_close_gpio_event_handles(dev);
    else
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
        dev->isr_control_pipe[0] = dev->isr_control_pipe[1] = -1;
#endif
    }

    // assume our thread will exit either way we just lost it's handle
    dev->thread_id = 0;
    dev->isr_value_fp = -1;
    dev->isr_thread_terminating = 0;

    if (dev->events) {
      free(dev->events);
      dev->events = NULL;
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

    if (IS_FUNC_DEFINED(dev, gpio_mode_replace))
        return dev->advance_func->gpio_mode_replace(dev, mode);

    if (IS_FUNC_DEFINED(dev, gpio_mode_pre)) {
        mraa_result_t pre_ret = (dev->advance_func->gpio_mode_pre(dev, mode));
        if (pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }

    if (plat->chardev_capable) {
        unsigned flags = 0;
        int line_handle;
        mraa_gpiod_group_t gpio_iter;

        _mraa_close_gpio_desc(dev);

        /* We save flag values from the first valid line. */
        for_each_gpio_group(gpio_iter, dev) {
            mraa_gpiod_line_info* linfo = mraa_get_line_info_by_chip_number(gpio_iter->gpio_chip, gpio_iter->gpio_lines[0]);
            if (!linfo) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
                return MRAA_ERROR_UNSPECIFIED;
            }
            flags = linfo->flags;
            free(linfo);

            /* We don't need to iterate any further. */
            break;
        }

        /* Without changing the API, for now, we can request only one mode per call. */
        switch (mode) {
            case MRAA_GPIOD_ACTIVE_LOW:
                flags |= GPIOHANDLE_REQUEST_ACTIVE_LOW;
                break;
            case MRAA_GPIOD_OPEN_DRAIN:
                flags |= GPIOHANDLE_REQUEST_OPEN_DRAIN;
                break;
            case MRAA_GPIOD_OPEN_SOURCE:
                flags |= GPIOHANDLE_REQUEST_OPEN_SOURCE;
                break;
            default:
                return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
        }

        for_each_gpio_group(gpio_iter, dev) {
            line_handle = mraa_get_lines_handle(gpio_iter->dev_fd, gpio_iter->gpio_lines, gpio_iter->num_gpio_lines, flags, 0);
            if (line_handle <= 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line handle");
                return MRAA_ERROR_INVALID_RESOURCE;
            }

            gpio_iter->gpiod_handle = line_handle;
        }
    } else {

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
    }

    if (IS_FUNC_DEFINED(dev, gpio_mode_post))
        return dev->advance_func->gpio_mode_post(dev, mode);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_chardev_dir(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    int line_handle;
    unsigned flags = 0;
    mraa_gpiod_group_t gpio_iter;

    for_each_gpio_group(gpio_iter, dev) {
        mraa_gpiod_line_info* linfo = mraa_get_line_info_by_chip_number(gpio_iter->gpio_chip, gpio_iter->gpio_lines[0]);
        if (!linfo) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
            return MRAA_ERROR_UNSPECIFIED;
        }
        flags = linfo->flags;
        free(linfo);

        /* We don't need to iterate any further. */
        break;
    }

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

    for_each_gpio_group(gpio_iter, dev) {
        if (gpio_iter->gpiod_handle != -1) {
            close(gpio_iter->gpiod_handle);
            gpio_iter->gpiod_handle = -1;
        }

        line_handle = mraa_get_lines_handle(gpio_iter->dev_fd, gpio_iter->gpio_lines,
                                            gpio_iter->num_gpio_lines, flags, 0);
        if (line_handle <= 0) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line handle");
            return MRAA_ERROR_INVALID_RESOURCE;
        }

        gpio_iter->gpiod_handle = line_handle;
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

    if (IS_FUNC_DEFINED(dev, gpio_dir_replace)) {
            return dev->advance_func->gpio_dir_replace(dev, dir);
    }

    if (IS_FUNC_DEFINED(dev, gpio_dir_pre)) {
        mraa_result_t pre_ret = (dev->advance_func->gpio_dir_pre(dev, dir));
        if (pre_ret != MRAA_SUCCESS) {
            return pre_ret;
        }
    }

    if (plat->chardev_capable)
        return mraa_gpio_chardev_dir(dev, dir);

    mraa_gpio_context it = dev;

    while (it) {
        if (it->value_fp != -1) {
            close(it->value_fp);
            it->value_fp = -1;
        }
        char filepath[MAX_SIZE];
        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", it->pin);

        int direction = open(filepath, O_RDWR);

        if (direction == -1) {
            // Direction Failed to Open. If HIGH or LOW was passed will try and set
            // If not fail as usual.
            switch (dir) {
                case MRAA_GPIO_OUT_HIGH:
                    return mraa_gpio_write(it, 1);
                case MRAA_GPIO_OUT_LOW:
                    return mraa_gpio_write(it, 0);
                default:
                    syslog(LOG_ERR, "gpio%i: dir: Failed to open 'direction' for writing: %s",
                        it->pin, strerror(errno));
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
            syslog(LOG_ERR, "gpio%i: dir: Failed to write to 'direction': %s", it->pin, strerror(errno));
            return MRAA_ERROR_UNSPECIFIED;
        }

        close(direction);
        it = it->next;
    }

    if (IS_FUNC_DEFINED(dev, gpio_dir_post))
        return dev->advance_func->gpio_dir_post(dev, dir);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_read_dir(mraa_gpio_context dev, mraa_gpio_dir_t *dir)
{
    mraa_result_t result = MRAA_SUCCESS;

    /* Initialize with 'unusable'. */
    unsigned flags = GPIOLINE_FLAG_KERNEL;

    if (IS_FUNC_DEFINED(dev, gpio_read_dir_replace)) {
        return dev->advance_func->gpio_read_dir_replace(dev, dir);
    }

    if (plat->chardev_capable) {
        mraa_gpiod_group_t gpio_iter;

        for_each_gpio_group(gpio_iter, dev) {
            mraa_gpiod_line_info* linfo = mraa_get_line_info_by_chip_number(gpio_iter->gpio_chip, gpio_iter->gpio_lines[0]);
            if (!linfo) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting line info");
                return MRAA_ERROR_UNSPECIFIED;
            }
            flags = linfo->flags;
            free(linfo);

            /* We don't need to iterate further. */
            break;
        }

        if (flags & GPIOLINE_FLAG_KERNEL) {
            syslog(LOG_ERR, "[GPIOD_INTERFACE]: cannot read gpio direction. Line used by kernel.");
            return MRAA_ERROR_UNSPECIFIED;
        }

        *dir = flags & GPIOLINE_FLAG_IS_OUT ? MRAA_GPIO_OUT : MRAA_GPIO_IN;
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

        snprintf(filepath, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/direction", dev->pin);
        fd = open(filepath, O_RDONLY);
        if (fd == -1) {
            syslog(LOG_ERR, "gpio%i: read_dir: Failed to open 'direction' for reading: %s",
                   dev->pin, strerror(errno));
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

    if (IS_FUNC_DEFINED(dev, gpio_read_replace)) {
        return dev->advance_func->gpio_read_replace(dev);
    }

    if (plat->chardev_capable) {
        int output_values[1] = { 0 };

        if (mraa_gpio_read_multi(dev, output_values) != MRAA_SUCCESS)
                return -1;

        return output_values[0];
    }

    if (dev->mmap_read != NULL) {
        return dev->mmap_read(dev);
    }

    if (dev->value_fp == -1) {
        if (_mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
            return -1;
        }
    } else {
        // if value_fp is new this is pointless
        lseek(dev->value_fp, 0, SEEK_SET);
    }
    char bu[2];
    if (read(dev->value_fp, bu, 2 * sizeof(char)) != 2) {
        syslog(LOG_ERR, "gpio%i: read: Failed to read a sensible value from sysfs: %s",
                dev->pin, strerror(errno));
        return -1;
    }
    lseek(dev->value_fp, 0, SEEK_SET);

    return (int) strtol(bu, NULL, 10);
}

mraa_result_t
mraa_gpio_read_multi(mraa_gpio_context dev, int output_values[])
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: read multiple: context is invalid");
        return -1;
    }

    if (plat->chardev_capable) {
        memset(output_values, 0, dev->num_pins * sizeof(int));

        mraa_gpiod_group_t gpio_iter;

        for_each_gpio_group(gpio_iter, dev) {
            int status;
            unsigned flags = GPIOHANDLE_REQUEST_INPUT;

            if (gpio_iter->gpiod_handle <= 0) {
                gpio_iter->gpiod_handle = mraa_get_lines_handle(gpio_iter->dev_fd, gpio_iter->gpio_lines,
                                                                 gpio_iter->num_gpio_lines, flags, 0);
                if (gpio_iter->gpiod_handle <= 0) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                    return MRAA_ERROR_INVALID_HANDLE;
                }
            }

            status = mraa_get_line_values(gpio_iter->gpiod_handle, gpio_iter->num_gpio_lines,
                                          gpio_iter->rw_values);
            if (status < 0) {
                syslog(LOG_ERR, "[GPIOD_INTERFACE]: error writing gpio");
                return MRAA_ERROR_INVALID_RESOURCE;
            }

            /* Write values back to the user provided array. */
            for (int j = 0; j < gpio_iter->num_gpio_lines; ++j) {
                /* Use the internal reverse mapping table. */
                output_values[gpio_iter->gpio_group_to_pins_table[j]] = gpio_iter->rw_values[j];
            }
        }
    } else {
        mraa_gpio_context it = dev;
        int i = 0;

        while (it) {
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

    if (IS_FUNC_DEFINED(dev, gpio_write_pre)) {
        mraa_result_t pre_ret = (dev->advance_func->gpio_write_pre(dev, value));
        if (pre_ret != MRAA_SUCCESS)
            return pre_ret;
    }

    if (IS_FUNC_DEFINED(dev, gpio_write_replace)) {
        return dev->advance_func->gpio_write_replace(dev, value);
    }

    if (plat->chardev_capable) {
        int input_values[1] = { value };

        return mraa_gpio_write_multi(dev, input_values);
    }

    if (dev->mmap_write != NULL) {
        return dev->mmap_write(dev, value);
    }

    if (dev->value_fp == -1) {
        if (_mraa_gpio_get_valfp(dev) != MRAA_SUCCESS) {
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

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_gpio_write_multi(mraa_gpio_context dev, int input_values[])
{
    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: write: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (plat->chardev_capable) {
        mraa_gpiod_group_t gpio_iter;

        int* counters = calloc(dev->num_chips, sizeof(int));
        if (counters == NULL) {
            syslog(LOG_ERR, "mraa_gpio_write_multi() malloc error");
            return MRAA_ERROR_NO_RESOURCES;
        }

        for (int i = 0; i < dev->num_pins; ++i) {
            int chip_id = dev->pin_to_gpio_table[i];
            gpio_iter = &dev->gpio_group[chip_id];
            gpio_iter->rw_values[counters[chip_id]] = input_values[i];
            counters[chip_id]++;
        }
        free(counters);

        for_each_gpio_group(gpio_iter, dev) {
            int status;
            unsigned flags = GPIOHANDLE_REQUEST_OUTPUT;

            if (gpio_iter->gpiod_handle <= 0) {
                gpio_iter->gpiod_handle = mraa_get_lines_handle(gpio_iter->dev_fd, gpio_iter->gpio_lines,
                                                                gpio_iter->num_gpio_lines, flags, 0);
                if (gpio_iter->gpiod_handle <= 0) {
                    syslog(LOG_ERR, "[GPIOD_INTERFACE]: error getting gpio line handle");
                    return MRAA_ERROR_INVALID_HANDLE;
                }
            }

            status = mraa_set_line_values(gpio_iter->gpiod_handle, gpio_iter->num_gpio_lines,
                                          gpio_iter->rw_values);
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
_mraa_gpio_close_internal(mraa_gpio_context dev)
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

    mraa_gpio_unexport(dev);

    free(dev);

    return result;
}

mraa_result_t
mraa_gpio_close(mraa_gpio_context dev)
{
    mraa_result_t result = MRAA_SUCCESS;

    if (dev == NULL) {
        syslog(LOG_ERR, "gpio: close: context is invalid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (dev->events) {
        free(dev->events);
    }

    /* Free any ISRs */
    mraa_gpio_isr_exit(dev);

    if (plat && plat->chardev_capable) {
        _mraa_free_gpio_groups(dev);

        free(dev);
    } else {
        mraa_gpio_context it = dev, tmp;

        while (it) {
            tmp = it->next;
            if (_mraa_gpio_close_internal(it) != MRAA_SUCCESS) {
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
    } else {
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }
}
