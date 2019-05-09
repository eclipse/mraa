/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>

#include "common.h"
#include "mock/mock_board_aio.h"

mraa_result_t
mraa_mock_aio_init_internal_replace(mraa_aio_context dev, int pin)
{
    dev->channel = pin;
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_mock_aio_close_replace(mraa_aio_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

int
mraa_mock_aio_read_replace(mraa_aio_context dev)
{
    // return some random number between 0 and max value, based on the resolution
    int max_value = (1 << dev->value_bit) - 1;
    srand(time(NULL));
    return rand() % max_value;
}
