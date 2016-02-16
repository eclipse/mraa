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

#pragma once

#include <stdint.h>
#include <termios.h>

typedef struct s_serial {
    int port_is_open;
    char* port_name;
    int baud_rate;
    char* error_msg;

    int port_fd;
    struct termios settings_orig;
    struct termios settings;
    int tx;
    int rx;
} t_serial;

t_serial* serial_new();
int serial_open(t_serial* serial, char* name);
int serial_setBaud(t_serial* serial, int baud);
int serial_read(t_serial* serial, void* ptr, int count);
int serial_write(t_serial* serial, void* ptr, int len);
int serial_waitInput(t_serial* serial, int msec);
int serial_discardInput(t_serial* serial);
void serial_flushOutput(t_serial* serial);
int serial_setControl(t_serial* serial, int dtr, int rts);
