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
#include "mraa/iio.h"

int
main()
{
    mraa_iio_context iio_device0 = mraa_iio_init(0);
    char* attr_name;
    if (iio_device0 == NULL) {
        fprintf(stderr, "IIO device %d not found\n", 0);
        return EXIT_FAILURE;
    }

    fprintf(stderr, "Using IIO device %s\n", mraa_iio_get_device_name(iio_device0));
    float iio_float;
    int iio_integer;
    mraa_result_t ret;


    attr_name = "in_accel_x_raw";
    fprintf(stdout, "IIO write in_accel_x_raw: ");
    ret = mraa_iio_write_float(iio_device0, "in_accel_x_raw", 0.019163);
    if (ret != MRAA_SUCCESS) {
        mraa_result_print(ret);
    }

    fprintf(stdout, "IIO read in_accel_x_raw\n");    
	ret = mraa_iio_read_float(iio_device0, "in_accel_x_raw", &iio_float);
    if (ret == MRAA_SUCCESS)
        fprintf(stdout, "in_accel_x_raw: %f\n", iio_float);
	else
        mraa_result_print(ret);

   return EXIT_SUCCESS;
}
