/*
 * Copyright (c) 2016 Intel Corporation
 * Copyright (c) 2015 Jules Dourlens (jdourlens@gmail.com)
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

#include "firmata/serial.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/select.h>
#include <termios.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <linux/serial.h>

t_serial*
serial_new()
{
    t_serial* res;

    res = malloc(sizeof(t_serial));
    if (!res) {
        return (NULL);
    }
    res->port_is_open = 0;
    res->baud_rate = 38400;
    res->tx = 0;
    res->rx = 0;
    return (res);
}

int
serial_open(t_serial* serial, char* name)
{
    struct serial_struct kernel_serial_settings;
    int bits;

    serial->port_fd = open(name, O_RDWR | O_NOCTTY | O_NONBLOCK);
    if (serial->port_fd < 0) {
        if (errno == EACCES) {
            perror("Unable to access, insufficient permission");
        } else if (errno == EISDIR) {
            perror("Unable to open, Object is a directory, not a serial port");
        } else if (errno == ENODEV || errno == ENXIO) {
            perror("Unable to open, Serial port hardware not installed");
        } else if (errno == ENOENT) {
            perror("Unable to open, Device name does not exist");
        } else {
            perror("Unable to open: Unknown error.");
        }
        return (-1);
    }
    if (ioctl(serial->port_fd, TIOCMGET, &bits) < 0) {
        close(serial->port_fd);
        perror("Unable to query serial port signals");
        return (-1);
    }
    bits &= ~(TIOCM_DTR | TIOCM_RTS);
    if (ioctl(serial->port_fd, TIOCMSET, &bits) < 0) {
        close(serial->port_fd);
        perror("Unable to control serial port signals");
        return (-1);
    }
    if (tcgetattr(serial->port_fd, &(serial->settings_orig)) != 0) {
        close(serial->port_fd);
        perror("Unable to query serial port settings (perhaps not a serial port)");
        return (-1);
    }
    memset(&(serial->settings), 0, sizeof(struct termios));
    serial->settings.c_iflag = IGNBRK | IGNPAR;
    serial->settings.c_cflag = CS8 | CREAD | HUPCL | CLOCAL;
    serial_setBaud(serial, serial->baud_rate);
    if (ioctl(serial->port_fd, TIOCGSERIAL, &kernel_serial_settings) == 0) {
        kernel_serial_settings.flags |= ASYNC_LOW_LATENCY;
        ioctl(serial->port_fd, TIOCSSERIAL, &kernel_serial_settings);
    }
    tcflush(serial->port_fd, TCIFLUSH);
    serial->port_name = name;
    serial->port_is_open = 1;
    return 0;
}

int
serial_setBaud(t_serial* serial, int baud)
{
    speed_t spd;
    switch (baud) {
        case 230400:
            spd = B230400;
            break;
        case 115200:
            spd = B115200;
            break;
        case 57600:
            spd = B57600;
            break;
        case 38400:
            spd = B38400;
            break;
        case 19200:
            spd = B19200;
            break;
        case 9600:
            spd = B9600;
            break;
        case 4800:
            spd = B4800;
            break;
        case 2400:
            spd = B2400;
            break;
        case 1800:
            spd = B1800;
            break;
        case 1200:
            spd = B1200;
            break;
        case 600:
            spd = B600;
            break;
        case 300:
            spd = B300;
            break;
        case 200:
            spd = B200;
            break;
        case 150:
            spd = B150;
            break;
        case 134:
            spd = B134;
            break;
        case 110:
            spd = B110;
            break;
        case 75:
            spd = B75;
            break;
        case 50:
            spd = B50;
            break;
#ifdef B460800
        case 460800:
            spd = B460800;
            break;
#endif
#ifdef B500000
        case 500000:
            spd = B500000;
            break;
#endif
#ifdef B576000
        case 576000:
            spd = B576000;
            break;
#endif
#ifdef B921600
        case 921600:
            spd = B921600;
            break;
#endif
#ifdef B1000000
        case 1000000:
            spd = B1000000;
            break;
#endif
#ifdef B1152000
        case 1152000:
            spd = B1152000;
            break;
#endif
#ifdef B1500000
        case 1500000:
            spd = B1500000;
            break;
#endif
#ifdef B2000000
        case 2000000:
            spd = B2000000;
            break;
#endif
#ifdef B2500000
        case 2500000:
            spd = B2500000;
            break;
#endif
#ifdef B3000000
        case 3000000:
            spd = B3000000;
            break;
#endif
#ifdef B3500000
        case 3500000:
            spd = B3500000;
            break;
#endif
#ifdef B4000000
        case 4000000:
            spd = B4000000;
            break;
#endif
#ifdef B7200
        case 7200:
            spd = B7200;
            break;
#endif
#ifdef B14400
        case 14400:
            spd = B14400;
            break;
#endif
#ifdef B28800
        case 28800:
            spd = B28800;
            break;
#endif
#ifdef B76800
        case 76800:
            spd = B76800;
            break;
#endif
        default: {
            return -1;
        }
    }
    cfsetospeed(&(serial->settings), spd);
    cfsetispeed(&(serial->settings), spd);
    if (tcsetattr(serial->port_fd, TCSANOW, &(serial->settings)) < 0)
        return (-1);
    return (0);
}

int
serial_read(t_serial* serial, void* ptr, int count)
{
    int n, bits;

    if (!serial->port_is_open)
        return (-1);
    if (count <= 0)
        return (0);
    n = read(serial->port_fd, ptr, count);
    if (n < 0 && (errno == EAGAIN || errno == EINTR))
        return (0);
    if (n == 0 && ioctl(serial->port_fd, TIOCMGET, &bits) < 0)
        return (-99);
    serial->rx += n;
    return (n);
}

int
serial_write(t_serial* serial, void* ptr, int len)
{
    // printf("Write %d\n", len);
    if (!serial->port_is_open)
        return -1;
    int n, written = 0;
    fd_set wfds;
    struct timeval tv;
    while (written < len) {
        n = write(serial->port_fd, (const char*) ptr + written, len - written);
        if (n < 0 && (errno == EAGAIN || errno == EINTR))
            n = 0;
        // printf("Write, n = %d\n", n);
        if (n < 0)
            return -1;
        if (n > 0) {
            written += n;
        } else {
            tv.tv_sec = 10;
            tv.tv_usec = 0;
            FD_ZERO(&wfds);
            FD_SET(serial->port_fd, &wfds);
            n = select(serial->port_fd + 1, NULL, &wfds, NULL, &tv);
            if (n < 0 && errno == EINTR)
                n = 1;
            if (n <= 0)
                return -1;
        }
    }
    serial->tx += written;
    return (written);
}

int
serial_waitInput(t_serial* serial, int msec)
{
    if (!serial->port_is_open)
        return -1;
    fd_set rfds;
    struct timeval tv;
    tv.tv_sec = msec / 1000;
    tv.tv_usec = (msec % 1000) * 1000;
    FD_ZERO(&rfds);
    FD_SET(serial->port_fd, &rfds);
    return (select(serial->port_fd + 1, &rfds, NULL, NULL, &tv));
}

int
serial_discardInput(t_serial* serial)
{
    if (!serial->port_is_open)
        return;
    // does this really work properly (and is it thread safe) on Linux??
    tcflush(serial->port_fd, TCIFLUSH);
}

void
serial_flushOutput(t_serial* serial)
{
    if (!serial->port_is_open)
        return;
    tcdrain(serial->port_fd);
}

int
serial_setControl(t_serial* serial, int dtr, int rts)
{
    if (!serial->port_is_open)
        return -1;
    int bits;
    if (ioctl(serial->port_fd, TIOCMGET, &bits) < 0)
        return -1;
    if (dtr == 1) {
        bits |= TIOCM_DTR;
    } else if (dtr == 0) {
        bits &= ~TIOCM_DTR;
    }
    if (rts == 1) {
        bits |= TIOCM_RTS;
    } else if (rts == 0) {
        bits &= ~TIOCM_RTS;
    }
    if (ioctl(serial->port_fd, TIOCMSET, &bits) < 0)
        return -1;
    ;
    return (0);
}
