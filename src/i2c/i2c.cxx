/*
 * Copyright (C) Intel Corporation.
 *
 * Author: Brendan Le Foll
 *                                                                                                                                                             
 * Copyright © 2014 Intel Corporation
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

#include "i2c.h"

using namespace maa;

I2C::I2C(unsigned int sda, unsigned int scl)
{
}

void
I2C::frequency(int hz)
{
    _hz = hz;
}

int
I2C::read(int address, char *data, int length, bool repeated)
{
    return 0;
}

int
I2C::read(int ack)
{
    return 0;
}

int
I2C::write(int address, const char *data, int length, bool repeated)
{
    return 0;
}

int
I2C::write(int data)
{
}

void
I2C::start()
{
}

void
I2C::stop()
{
}

void
I2C::aquire()
{
}
