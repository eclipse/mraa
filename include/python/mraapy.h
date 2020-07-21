/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

void mraa_python_isr(void (*isr)(void*), void* isr_args);
