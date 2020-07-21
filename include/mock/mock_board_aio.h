/*
 * Author: Alex Tereschenko <alext.mkrs@gmail.com>
 * Copyright (c) 2016 Alex Tereschenko.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include "mraa_internal.h"

mraa_result_t
mraa_mock_aio_init_internal_replace(mraa_aio_context dev, int pin);

mraa_result_t
mraa_mock_aio_close_replace(mraa_aio_context dev);

int
mraa_mock_aio_read_replace(mraa_aio_context dev);

#ifdef __cplusplus
}
#endif
