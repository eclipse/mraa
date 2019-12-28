/*
 * Author: Henry Bruce <henry.bruce@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

typedef struct {
    void (*python_isr)(void (*isr)(void*), void* isr_args);
	void (*java_isr_callback)(void *args);
	mraa_result_t (*java_attach_thread)();
	void (*java_detach_thread)();
	void* (*java_create_global_ref)(void* args);
	void (*java_delete_global_ref)(void* ref);

} mraa_lang_func_t;
