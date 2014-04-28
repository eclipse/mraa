/*
 * Author: Brendan Le Foll
 *
 * Copyright © 2014 Intel Corporation
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#pragma once

typedef enum {
    MAA_SUCCESS                              =  0,
    MAA_ERROR_FEATURE_NOT_IMPLEMENTED        =  1,
    MAA_ERROR_FEATURE_NOT_SUPPORTED          =  2,
    MAA_ERROR_INVALID_VERBOSITY_LEVEL        =  3,
    MAA_ERROR_INVALID_PARAMETER              =  4,
    MAA_ERROR_INVALID_HANDLE                 =  5,
    MAA_ERROR_NO_RESOURCES                   =  6,
    MAA_ERROR_INVALID_RESOURCE               =  7,
    MAA_ERROR_INVALID_QUEUE_TYPE             =  8,
    MAA_ERROR_NO_DATA_AVAILABLE              =  9,

    MAA_ERROR_UNSPECIFIED                    = 99
} maa_result_t;

const char* maa_get_version();
