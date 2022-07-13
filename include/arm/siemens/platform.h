/*
 * Author: Le Jin <le.jin@siemens.com>
 * Copyright (c) Siemens AG, 2019
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
#ifndef _PLATFORM_H_
#define _PLATFORM_H_
#ifdef __cplusplus
extern "C" {
#endif
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
typedef struct {
    bool (*init)(void);
    void (*deinit)(void);
    void (*select_func)(uint8_t group, uint16_t pinIndex, uint8_t func);
    void (*select_input)(uint8_t group, uint16_t pinIndex);
    void (*select_output)(uint8_t group, uint16_t pinIndex);
    void (*select_inout)(uint8_t group, uint16_t pinIndex);
    void (*select_hiz)(uint8_t group, uint16_t pinIndex);
    void (*select_pull_up)(uint8_t group, uint16_t pinIndex);
    void (*select_pull_down)(uint8_t group, uint16_t pinIndex);
    void (*select_pull_disable)(uint8_t group, uint16_t pinIndex);
    uint32_t (*get_raw_reg_value)(uint8_t group, uint16_t pinIndex);
    void (*set_raw_reg_value)(uint8_t group, uint16_t pinIndex, uint32_t value);
    void (*dump_info)(uint8_t group, uint16_t pinIndex);
} pin_mux_ops_t;

typedef struct{
    bool            initialized;
    pin_mux_ops_t     ops;
} pin_mux_interface_t;

#define VOID_INTERFACE_CALL(instance, function) \
do {\
        if((instance) && ((pin_mux_interface_t *)instance)->ops.function) \
            ((pin_mux_interface_t *)instance)->ops.function(); \
}while(0)

#define VOID_INTERFACE_CALL_WITH_ARGS(instance, function, ...) \
do {\
        if((instance) && ((pin_mux_interface_t *)instance)->ops.function) \
            ((pin_mux_interface_t *)instance)->ops.function(__VA_ARGS__); \
}while(0)

#define INTERFACE_CALL_WITH_ARGS(instance, retType, defaultRetValue, function, ...) \
( \
    { \
        retType ret = defaultRetValue; \
        if((instance) && ((pin_mux_interface_t *)instance)->ops.function) \
            ret = ((pin_mux_interface_t *)instance)->ops.function(__VA_ARGS__); \
        ret; \
    } \
)

#ifdef _DEBUG
#define DEBUG_PRINT(...) fprintf(stderr, __VA_ARGS__)
#else
#define DEBUG_PRINT(...) do{}while(0)
#endif

void *platfrom_pinmux_get_instance(char *platform);
#define platform_pinmux_release_instance(instance) VOID_INTERFACE_CALL(instance, deinit)
#define platform_pinmux_select_func(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_func, __VA_ARGS__)
#define platform_pinmux_select_input(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_input, __VA_ARGS__)
#define platform_pinmux_select_output(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_output, __VA_ARGS__)
#define platform_pinmux_select_inout(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_inout, __VA_ARGS__)
#define platform_pinmux_select_hiz(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_hiz, __VA_ARGS__)
#define platform_pinmux_select_pull_up(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_pull_up, __VA_ARGS__)
#define platform_pinmux_select_pull_down(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_pull_down, __VA_ARGS__)
#define platform_pinmux_select_pull_disable(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, select_pull_disable, __VA_ARGS__)
#define platform_pinmux_get_raw_reg_value(instance, ...) INTERFACE_CALL_WITH_ARGS(instance, uint32_t, -1, get_raw_reg_value, __VA_ARGS__)
#define platform_pinmux_set_raw_reg_value(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, set_raw_reg_value, __VA_ARGS__)
#define platform_pinmux_dump_info(instance, ...) VOID_INTERFACE_CALL_WITH_ARGS(instance, dump_info, __VA_ARGS__)
#ifdef __cplusplus
}
#endif
#endif
