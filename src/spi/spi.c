/*
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

#include <stdlib.h>

#include "spi.h"

maa_spi_context*
maa_spi_init()
{
    return NULL;
}

maa_result_t
maa_spi_mode(maa_spi_context* spi, unsigned short mode)
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

maa_result_t
maa_spi_frequency(maa_spi_context* spi, int hz)
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

unsigned int
maa_spi_write(maa_spi_context* spi, unsigned int data)
{
    return 0;
}

maa_result_t
maa_spi_stop(maa_spi_context* spi)
{
    return MAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}
