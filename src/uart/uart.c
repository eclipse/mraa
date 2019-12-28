/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Contributions: Jon Trulson <jtrulson@ics.com>
 *                Brendan le Foll <brendan.le.foll@intel.com>
 *                Nicola Lunghi <nicola.lunghi@emutex.com>
 * Copyright (c) 2014 - 2015 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <termios.h>
#include <sys/select.h>
#include <errno.h>
#include <string.h>

#include "uart.h"
#include "mraa_internal.h"

#ifndef CMSPAR
#define CMSPAR   010000000000
#endif

// This function takes an unsigned int and converts it to a B* speed_t
// that can be used with linux/posix termios
static speed_t
uint2speed(unsigned int speed)
{
    switch (speed) {
        case 0:
            return B0; // hangup, not too useful otherwise
        case 50:
            return B50;
        case 75:
            return B75;
        case 110:
            return B110;
        case 150:
            return B150;
        case 200:
            return B200;
        case 300:
            return B300;
        case 600:
            return B600;
        case 1200:
            return B1200;
        case 1800:
            return B1800;
        case 2400:
            return B2400;
        case 4800:
            return B4800;
        case 9600:
            return B9600;
        case 19200:
            return B19200;
        case 38400:
            return B38400;
        case 57600:
            return B57600;
        case 115200:
            return B115200;
        case 230400:
            return B230400;
        case 460800:
            return B460800;
        case 500000:
            return B500000;
        case 576000:
            return B576000;
        case 921600:
            return B921600;
        case 1000000:
            return B1000000;
        case 1152000:
            return B1152000;
        case 1500000:
            return B1500000;
        case 2000000:
            return B2000000;
        case 2500000:
            return B2500000;
        case 3000000:
            return B3000000;
#if !defined(MSYS)
        case 3500000:
            return B3500000;
        case 4000000:
            return B4000000;
#endif
        default:
            // if we are here, then an unsupported baudrate was selected.
            return B0;
    }
}

static unsigned int speed_to_uint(speed_t speedt) {
    struct baud_table {
        speed_t speedt;
        unsigned int baudrate;
    };
    static const struct baud_table bauds[] = {
        { B50, 50 },
        { B75, 75 },
        { B110, 110 },
        { B150, 150 },
        { B200, 200 },
        { B300, 300 },
        { B600, 600 },
        { B1200, 1200 },
        { B1800, 1800 },
        { B2400, 2400 },
        { B9600, 9600 },
        { B19200, 19200 },
        { B38400, 38400 },
        { B57600, 57600 },
        { B115200, 115200 },
        { B230400, 230400 },
        { B460800, 460800 },
        { B500000, 500000 },
        { B576000, 576000 },
        { B921600, 921600 },
        { B1000000, 1000000 },
        { B1152000, 1152000 },
        { B1500000, 1500000 },
        { B2000000, 2000000 },
        { B2500000, 2500000 },
        { B3000000, 3000000 },
#if !defined(MSYS)
        { B3500000, 3500000 },
        { B4000000, 4000000 },
#endif
        { B0, 0} /* Must be last in this table */
    };
    int i = 0;

    while (bauds[i].baudrate > 0) {
        if (speedt == bauds[i].speedt) {
            return bauds[i].baudrate;
        }
        i++;
    }
    return 0;
}

static mraa_uart_context
mraa_uart_init_internal(mraa_adv_func_t* func_table)
{
    mraa_uart_context dev = (mraa_uart_context) calloc(1, sizeof(struct _uart));
    if (dev == NULL) {
        syslog(LOG_CRIT, "uart: Failed to allocate memory for context");
        return NULL;
    }
    dev->index = -1;
    dev->fd = -1;
    dev->advance_func = func_table;

    return dev;
}

mraa_uart_context
mraa_uart_init(int index)
{
    if (plat == NULL) {
        syslog(LOG_ERR, "uart%i: init: platform not initialised", index);
        return NULL;
    }

    if (mraa_is_sub_platform_id(index)) {
        syslog(LOG_NOTICE, "uart%i: init: Using sub platform is not supported", index);
        return NULL;
    }

    if (plat->adv_func != NULL && plat->adv_func->uart_init_pre != NULL) {
        if (plat->adv_func->uart_init_pre(index) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "uart%i: init: failure in pre-init platform hook", index);
            return NULL;
        }
    }

    if (plat->uart_dev_count == 0) {
        syslog(LOG_ERR, "uart%i: init: platform has no UARTs defined", index);
        return NULL;
    }

    if (plat->uart_dev_count <= index) {
        syslog(LOG_ERR, "uart%i: init: platform has only %i uarts", index, plat->uart_dev_count);
        return NULL;
    }

    if (!plat->no_bus_mux) {
        int pos = plat->uart_dev[index].rx;
        if (pos >= 0) {
            if (plat->pins[pos].uart.mux_total > 0) {
                if (mraa_setup_mux_mapped(plat->pins[pos].uart) != MRAA_SUCCESS) {
                    syslog(LOG_ERR, "uart%i: init: failed to setup muxes for RX pin", index);
                    return NULL;
                }
            }
        }

        pos = plat->uart_dev[index].tx;
        if (pos >= 0) {
            if (plat->pins[pos].uart.mux_total > 0) {
                if (mraa_setup_mux_mapped(plat->pins[pos].uart) != MRAA_SUCCESS) {
                    syslog(LOG_ERR, "uart%i: init: failed to setup muxes for TX pin", index);
                    return NULL;
                }
            }
        }
    }

    mraa_uart_context dev = mraa_uart_init_raw((char*)plat->uart_dev[index].device_path);
    if (dev == NULL) {
        return NULL;
    }
    dev->index = index; //Set the board Index.

    if (IS_FUNC_DEFINED(dev, uart_init_post)) {
        mraa_result_t ret = dev->advance_func->uart_init_post(dev);
        if (ret != MRAA_SUCCESS) {
            free(dev);
            return NULL;
        }
    }

    return dev;
}

mraa_uart_context
mraa_uart_init_raw(const char* path)
{
    mraa_result_t status = MRAA_SUCCESS;
    mraa_uart_context dev = NULL;

    if (!path) {
        syslog(LOG_ERR, "uart: device path undefined");
        status = MRAA_ERROR_INVALID_PARAMETER;
        goto init_raw_cleanup;
    }

    dev = mraa_uart_init_internal(plat == NULL ? NULL : plat->adv_func);
    if (dev == NULL) {
        syslog(LOG_ERR, "uart: Failed to allocate memory for context");
        status = MRAA_ERROR_NO_RESOURCES;
        goto init_raw_cleanup;
    }
    dev->path = (char*) calloc(strlen(path)+1, sizeof(char));
    if (dev->path == NULL) {
        syslog(LOG_ERR, "uart: Failed to allocate memory for path");
        status =  MRAA_ERROR_NO_RESOURCES;
        goto init_raw_cleanup;
    }
    strncpy((char *) dev->path, path, strlen(path));

    if (IS_FUNC_DEFINED(dev, uart_init_raw_replace)) {
        status = dev->advance_func->uart_init_raw_replace(dev, path);
        if (status == MRAA_SUCCESS) {
            return dev;
        } else {
            goto init_raw_cleanup;
        }
    }

    // now open the device
    if ((dev->fd = open(dev->path, O_RDWR)) == -1) {
        syslog(LOG_ERR, "uart: open(%s) failed: %s", path, strerror(errno));
        status = MRAA_ERROR_INVALID_RESOURCE;
        goto init_raw_cleanup;
    }

    // now setup the tty and the selected baud rate
    struct termios termio;

    // get current modes
    if (tcgetattr(dev->fd, &termio)) {
        syslog(LOG_ERR, "uart: tcgetattr(%s) failed: %s", path, strerror(errno));
        status = MRAA_ERROR_INVALID_RESOURCE;
        goto init_raw_cleanup;
    }

    // setup for a 'raw' mode.  8N1, no echo or special character
    // handling, such as flow control or line editing semantics.
    // cfmakeraw is not POSIX!
    cfmakeraw(&termio);
    if (tcsetattr(dev->fd, TCSAFLUSH, &termio) < 0) {
        syslog(LOG_ERR, "uart: tcsetattr(%s) failed after cfmakeraw(): %s", path, strerror(errno));
        status = MRAA_ERROR_INVALID_RESOURCE;
        goto init_raw_cleanup;
    }

    if (mraa_uart_set_baudrate(dev, 9600) != MRAA_SUCCESS) {
        status = MRAA_ERROR_INVALID_RESOURCE;
        goto init_raw_cleanup;
    }

init_raw_cleanup:
    if (status != MRAA_SUCCESS) {
        if (dev != NULL) {
            if (dev->fd >= 0) {
                close(dev->fd);
            }
            if (dev->path != NULL) {
                free((void *) dev->path);
            }
            free(dev);
        }
        return NULL;
    }

    return dev;
}

mraa_result_t
mraa_uart_stop(mraa_uart_context dev)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: stop: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    // just close the device and reset our fd.
    if (dev->fd >= 0) {
        close(dev->fd);
    }
    if (dev->path != NULL) {
        free((void *) dev->path);
    }

    free(dev);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_settings(int index, const char **devpath, const char **name, int* baudrate, int* databits, int* stopbits, mraa_uart_parity_t* parity, unsigned int* ctsrts, unsigned int* xonxoff) {
    struct termios term;
    int fd;

    if (plat == NULL) {
        return MRAA_ERROR_PLATFORM_NOT_INITIALISED;
    }

    /* Access through UART index? */
    if (index >= 0 && index < plat->uart_dev_count) {
        if (devpath != NULL) {
            *devpath = plat->uart_dev[index].device_path;
        }
        if (name != NULL) {
            *name = plat->uart_dev[index].name;
        }
    }

    /* is any information that requires opening the device requested?  */
    if (baudrate != NULL || stopbits != NULL || parity != NULL || databits != NULL || ctsrts != NULL || xonxoff != NULL) {
       const char *dev;

       /* Access UART by index or devpath? */
       if (index >=0 && index < plat->uart_dev_count) {
           dev = plat->uart_dev[index].device_path;
       } else
       if (devpath != NULL) {
           dev = *devpath;
       } else {
           return MRAA_ERROR_INVALID_RESOURCE;
       }

       fd = open(dev, O_RDONLY | O_NOCTTY);

       if (fd < 0) {
           return MRAA_ERROR_INVALID_RESOURCE;
       }

       if (tcgetattr(fd, &term)) {
           close(fd);
           return MRAA_ERROR_INVALID_RESOURCE;
       }

       if (databits != NULL) {
           switch (term.c_cflag & CSIZE) {
           case CS8:
               *databits = 8;
               break;
           case CS7:
               *databits = 7;
               break;
           case CS6:
               *databits = 6;
               break;
           case CS5:
               *databits = 5;
           default: /* Cannot happen? Linux kernel CSIZE mask is exactly two bits wide */
               break;
           }
       }

       if (stopbits != NULL) {
           *stopbits = term.c_cflag & CSTOPB ? 2 : 1;
       }

       if (parity != NULL) {
           if (term.c_cflag & PARODD) *parity = MRAA_UART_PARITY_ODD;
           else
           if (term.c_cflag & PARENB) *parity = MRAA_UART_PARITY_EVEN;
           else
           *parity = MRAA_UART_PARITY_NONE;
       }

       if (baudrate != NULL) {
           *baudrate = speed_to_uint(cfgetospeed(&term));
       }

       if (ctsrts != NULL) {
           *ctsrts = term.c_cflag & CRTSCTS;
       }

       if (xonxoff != NULL) {
           *xonxoff = term.c_iflag & (IXON|IXOFF);
       }

       close(fd);
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_flush(mraa_uart_context dev)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: flush: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_flush_replace)) {
        return dev->advance_func->uart_flush_replace(dev);
    }

#if !defined(PERIPHERALMAN)
    if (tcdrain(dev->fd) == -1) {
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }
#endif

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_sendbreak(mraa_uart_context dev, int duration)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: sendbreak: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_sendbreak_replace)) {
        return dev->advance_func->uart_sendbreak_replace(dev, duration);
    }

#if !defined(PERIPHERALMAN)
    if (tcsendbreak(dev->fd, duration) == -1) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }
#endif

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_set_baudrate(mraa_uart_context dev, unsigned int baud)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: set_baudrate: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_set_baudrate_replace)) {
        return dev->advance_func->uart_set_baudrate_replace(dev, baud);
    }

    struct termios termio;
    if (tcgetattr(dev->fd, &termio)) {
        syslog(LOG_ERR, "uart%i: set_baudrate: tcgetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    // set our baud rates
    speed_t speed = uint2speed(baud);
    if (speed == B0)
    {
        syslog(LOG_ERR, "uart%i: set_baudrate: invalid baudrate: %i", dev->index, baud);
        return MRAA_ERROR_INVALID_PARAMETER;
    }
    cfsetispeed(&termio, speed);
    cfsetospeed(&termio, speed);

    // make it so
    if (tcsetattr(dev->fd, TCSAFLUSH, &termio) < 0) {
        syslog(LOG_ERR, "uart%i: set_baudrate: tcsetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_set_mode(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: set_mode: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_set_mode_replace)) {
        return dev->advance_func->uart_set_mode_replace(dev, bytesize, parity, stopbits);
    }

    struct termios termio;
    if (tcgetattr(dev->fd, &termio)) {
        syslog(LOG_ERR, "uart%i: set_mode: tcgetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    termio.c_cflag &= ~CSIZE;
    switch (bytesize) {
        case 8:
            termio.c_cflag |= CS8;
            break;
        case 7:
            termio.c_cflag |= CS7;
            break;
        case 6:
            termio.c_cflag |= CS6;
            break;
        case 5:
            termio.c_cflag |= CS5;
            break;
        default:
            termio.c_cflag |= CS8;
            break;
    }

    // POSIX & linux doesn't support 1.5 and I've got bigger fish to fry
    switch (stopbits) {
        case 1:
            termio.c_cflag &= ~CSTOPB;
            break;
        case 2:
            termio.c_cflag |= CSTOPB;
        default:
            break;
    }

    switch (parity) {
        case MRAA_UART_PARITY_NONE:
            termio.c_cflag &= ~(PARENB | PARODD);
            break;
        case MRAA_UART_PARITY_EVEN:
            termio.c_cflag |= PARENB;
            termio.c_cflag &= ~PARODD;
            break;
        case MRAA_UART_PARITY_ODD:
            termio.c_cflag |= PARENB | PARODD;
            break;
        case MRAA_UART_PARITY_MARK: // not POSIX
            termio.c_cflag |= PARENB | CMSPAR | PARODD;
            break;
        case MRAA_UART_PARITY_SPACE: // not POSIX
            termio.c_cflag |= PARENB | CMSPAR;
            termio.c_cflag &= ~PARODD;
            break;
    }

    if (tcsetattr(dev->fd, TCSAFLUSH, &termio) < 0) {
        syslog(LOG_ERR, "uart%i: set_mode: tcsetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_set_flowcontrol(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: set_flowcontrol: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_set_flowcontrol_replace)) {
        return dev->advance_func->uart_set_flowcontrol_replace(dev, xonxoff, rtscts);
    }

    if (rtscts) {
        // assign the CTS and RTS pin to UART when enabling flow control
        if (!plat->no_bus_mux) {
            int pos_cts = plat->uart_dev[dev->index].cts;
            int pos_rts = plat->uart_dev[dev->index].rts;

            if ((pos_cts >= 0) && (pos_rts >= 0)) {
                if (plat->pins[pos_cts].uart.mux_total > 0) {
                    if (mraa_setup_mux_mapped(plat->pins[pos_cts].uart) != MRAA_SUCCESS) {
                        syslog(LOG_ERR, "uart%i: init: failed to setup muxes for CTS pin", dev->index);
                        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
                    }
                }
                if (plat->pins[pos_rts].uart.mux_total > 0) {
                    if (mraa_setup_mux_mapped(plat->pins[pos_rts].uart) != MRAA_SUCCESS) {
                        syslog(LOG_ERR, "uart%i: init: failed to setup muxes for RTS pin", dev->index);
                        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
                    }
                }
            }
        }
    }

    // hardware flow control
    int action = TCIOFF;
    if (xonxoff) {
        action = TCION;
    }
    if (tcflow(dev->fd, action)) {
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }

    // rtscts
    struct termios termio;

    // get current modes
    if (tcgetattr(dev->fd, &termio)) {
        syslog(LOG_ERR, "uart%i: set_flowcontrol: tcgetattr() failed: %s", dev->index, strerror(errno));
         return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (rtscts) {
        termio.c_cflag |= CRTSCTS;
    } else {
        termio.c_cflag &= ~CRTSCTS;
    }

    if (tcsetattr(dev->fd, TCSAFLUSH, &termio) < 0) {
        syslog(LOG_ERR, "uart%i: set_flowcontrol: tcsetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_set_timeout(mraa_uart_context dev, int read, int write, int interchar)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: set_timeout: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_set_timeout_replace)) {
        return dev->advance_func->uart_set_timeout_replace(dev, read, write, interchar);
    }

    struct termios termio;
    // get current modes
    if (tcgetattr(dev->fd, &termio)) {
        syslog(LOG_ERR, "uart%i: set_timeout: tcgetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    if (read > 0) {
        read = read / 100;
        if (read == 0)
            read = 1;
    }
    termio.c_lflag &= ~ICANON; /* Set non-canonical mode */
    termio.c_cc[VTIME] = read; /* Set timeout in tenth seconds */
    if (tcsetattr(dev->fd, TCSANOW, &termio) < 0) {
        syslog(LOG_ERR, "uart%i: set_timeout: tcsetattr() failed: %s", dev->index, strerror(errno));
        return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_uart_set_non_blocking(mraa_uart_context dev, mraa_boolean_t nonblock)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: non_blocking: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_set_non_blocking_replace)) {
        return dev->advance_func->uart_set_non_blocking_replace(dev, nonblock);
    }

    // get current flags
    int flags = fcntl(dev->fd, F_GETFL);

    // update flags with new blocking state according to nonblock bool
    if (nonblock) {
        flags |= O_NONBLOCK;
    } else {
        flags &= ~O_NONBLOCK;
    }

    // set new flags
    if (fcntl(dev->fd, F_SETFL, flags) < 0) {
        syslog(LOG_ERR, "uart%i: non_blocking: failed changing fd blocking state: %s", dev->index, strerror(errno));
        return MRAA_ERROR_UNSPECIFIED;
    }

    return MRAA_SUCCESS;
}

const char*
mraa_uart_get_dev_path(mraa_uart_context dev)
{
    if (!dev) {
        return NULL;
    }
    if (dev->path == NULL) {
        return NULL;
    }

    return dev->path;
}

int
mraa_uart_read(mraa_uart_context dev, char* buf, size_t len)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: read: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_read_replace)) {
        return dev->advance_func->uart_read_replace(dev, buf, len);
    }

    if (dev->fd < 0) {
        syslog(LOG_ERR, "uart%i: read: port is not open", dev->index);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return read(dev->fd, buf, len);
}

int
mraa_uart_write(mraa_uart_context dev, const char* buf, size_t len)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: write: context is NULL");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (IS_FUNC_DEFINED(dev, uart_write_replace)) {
        return dev->advance_func->uart_write_replace(dev, buf, len);
    }

    if (dev->fd < 0) {
        syslog(LOG_ERR, "uart%i: write: port is not open", dev->index);
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return write(dev->fd, buf, len);
}

mraa_boolean_t
mraa_uart_data_available(mraa_uart_context dev, unsigned int millis)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: data_available: context is NULL");
        return 0;
    }

    if (IS_FUNC_DEFINED(dev, uart_data_available_replace)) {
        return dev->advance_func->uart_data_available_replace(dev, millis);
    }

    if (dev->fd < 0) {
        syslog(LOG_ERR, "uart%i: data_available: port is not open", dev->index);
        return 0;
    }

    struct timeval timeout;

    if (millis == 0) {
        // no waiting
        timeout.tv_sec = 0;
        timeout.tv_usec = 0;
    } else {
        timeout.tv_sec = millis / 1000;
        timeout.tv_usec = (millis % 1000) * 1000;
    }

    fd_set readfds;

#if !defined(PERIPHERALMAN)
    FD_ZERO(&readfds);
    FD_SET(dev->fd, &readfds);
#endif

    if (select(dev->fd + 1, &readfds, NULL, NULL, &timeout) > 0) {
        return 1; // data is ready
    } else {
        return 0;
    }
}


