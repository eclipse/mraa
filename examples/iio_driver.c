/*
 * Author: Brendan Le Foll
 * Copyright (c) 2015 Intel Corporation.
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

#include <unistd.h>
//! [Interesting]
#include "mraa/iio.h"

int
main()
{
    mraa_iio_context iio_device0;

    iio_device0 = mraa_iio_init(0);
    if (iio_device0 == NULL) {
        return EXIT_FAILURE;
    }

    float iio_value;
    mraa_result_t ret = mraa_iio_read(iio_device0, "in_voltage0_raw", &iio_value);
    if (ret == MRAA_SUCCESS) {
        fprintf(stdout, "IIO read %f\n", iio_value);
    }

    return EXIT_SUCCESS;
}
//! [Interesting]
