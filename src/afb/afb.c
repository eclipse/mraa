/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2017 Intel Corp.
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
#include <string.h>
#include <stdint.h>

#include <afb/afb-wsj1.h>
#include <afb/afb-ws-client.h>
#include <systemd/sd-event.h>
#include <json-c/json.h>

#include "common.h"
#include "afb/afb_board.h"

#define PLATFORM_NAME "MRAA AFB platform"

static void on_call(void *closure, const char *api, const char *verb, struct afb_wsj1_msg *msg)
{
	int rc;
	printf("ON-CALL %s/%s:\n%s\n", api, verb,
			json_object_to_json_string_ext(afb_wsj1_msg_object_j(msg),
						JSON_C_TO_STRING_PRETTY));
	fflush(stdout);
	rc = afb_wsj1_reply_error_s(msg, "\"unimplemented\"", NULL);
	if (rc < 0)
		fprintf(stderr, "replying failed: %m\n");
}

/* the callback interface for wsj1 */
static struct afb_wsj1_itf itf = {
    .on_hangup = 0,
    .on_call = on_call,
    .on_event = 0
};

/* global variables */
static struct afb_wsj1* wsj1;

/* makes a call to AFB binding */
static void
_afb_call(const char* api, const char* verb, const char* object)
{
    char *key;
    int rc;

    /* send the request */
    rc = afb_wsj1_call_s(wsj1, api, verb, object, NULL, key);
    if (rc < 0) {
        fprintf(stderr, "calling %s/%s(%s) failed: %m\n", api, verb, object);
    }
}

struct _afb_lock_msg {
    pthread_spinlock_t* lock;
    struct afb_wsj1_msg* msg;
};

static void
_afb_block(void* closure, struct afb_wsj1_msg* msg)
{
    struct _afb_lock_msg* lockmsg = (struct _afb_lock_msg*) closure;
    lockmsg->msg = msg;
    pthread_spin_unlock(lockmsg->lock);
}

static mraa_result_t
_afb_call_block_res(const char* api, const char* verb, const char* object)
{
    int rc;

    struct _afb_lock_msg lockmsg;
    pthread_spinlock_t pspinlock;
    lockmsg.lock = &pspinlock;
    int ret = pthread_spin_init(lockmsg.lock, PTHREAD_PROCESS_SHARED);
    if (ret != 0) {
        syslog(LOG_ERR, "afb: could not init locking");
        return MRAA_ERROR_NO_RESOURCES;
    }

    /* send the request */
    pthread_spin_lock(lockmsg.lock);
    rc = afb_wsj1_call_s(wsj1, api, verb, object, _afb_block, &lockmsg);
    if (rc < 0) {
        fprintf(stderr, "calling %s/%s(%s) failed: %m\n", api, verb, object);
	pthread_spin_unlock(lockmsg.lock);
	return MRAA_ERROR_INVALID_RESOURCE;
    }
    pthread_spin_lock(lockmsg.lock);
    pthread_spin_destroy(lockmsg.lock);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_init_bus_replace(mraa_i2c_context dev)
{
    _afb_call("mraa", "dev-init", "['mraa_i2c_init', 0]");
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_stop_replace(mraa_i2c_context dev)
{
    free(dev);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_set_frequency_replace(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    return MRAA_ERROR_INVALID_PARAMETER;
}

mraa_result_t
mraa_afb_i2c_address_replace(mraa_i2c_context dev, uint8_t addr)
{
    dev->addr = addr;

    char* command;
    command = calloc(sizeof(char), 255);
    snprintf(command, 255, "['mraa_i2c_address', 0, %d]", addr);
//    _afb_call("mraa", "command", command);
    return _afb_call_block_res("mraa", "command", command);

//    return MRAA_SUCCESS;
}

int
mraa_afb_i2c_read_replace(mraa_i2c_context dev, uint8_t* data, int length)
{
    return -1;
}

int
mraa_afb_i2c_read_byte_replace(mraa_i2c_context dev)
{
    return -1;
}

int
mraa_afb_i2c_read_byte_data_replace(mraa_i2c_context dev, uint8_t reg)
{
    return -1;
}

int
mraa_afb_i2c_read_bytes_data_replace(mraa_i2c_context dev, uint8_t reg, uint8_t* data, int length)
{
    return -1;
}

int
mraa_afb_i2c_read_word_data_replace(mraa_i2c_context dev, uint8_t reg)
{
    return -1;
}

mraa_result_t
mraa_afb_i2c_write_replace(mraa_i2c_context dev, const uint8_t* data, int length)
{
    char* command;
    command = calloc(sizeof(char), 255);
    snprintf(command, 255, "['mraa_i2c_write', 0, %s, %d]", data, length);
    _afb_call("mraa", "command", command);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_write_byte_replace(mraa_i2c_context dev, const uint8_t data)
{
    char* command;
    command = calloc(sizeof(char), 255);
    snprintf(command, 255, "['mraa_i2c_write_byte', 0, %d]", data);
    _afb_call("mraa", "command", command);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_write_byte_data_replace(mraa_i2c_context dev, const uint8_t data, const uint8_t reg)
{
    char* command;
    command = calloc(sizeof(char), 255);
    snprintf(command, 255, "['mraa_i2c_write_byte_data', 0, %d, %d]", data, reg);
    _afb_call("mraa", "command", command);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_afb_i2c_write_word_data_replace(mraa_i2c_context dev, const uint16_t data, const uint8_t reg)
{
    char* command;
    command = calloc(sizeof(char), 255);
    snprintf(command, 255, "['mraa_i2c_write_word_data', 0, %d, %d]", data, reg);
    _afb_call("mraa", "command", command);
    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_afb_board()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }
    b->platform_name = PLATFORM_NAME;
    b->phy_pin_count = 2;
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    // Replace functions
    b->adv_func->i2c_init_bus_replace = &mraa_afb_i2c_init_bus_replace;
    b->adv_func->i2c_stop_replace = &mraa_afb_i2c_stop_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_afb_i2c_set_frequency_replace;
    b->adv_func->i2c_address_replace = &mraa_afb_i2c_address_replace;
    b->adv_func->i2c_read_replace = &mraa_afb_i2c_read_replace;
    b->adv_func->i2c_read_byte_replace = &mraa_afb_i2c_read_byte_replace;
    b->adv_func->i2c_read_byte_data_replace = &mraa_afb_i2c_read_byte_data_replace;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_afb_i2c_read_bytes_data_replace;
    b->adv_func->i2c_read_word_data_replace = &mraa_afb_i2c_read_word_data_replace;
    b->adv_func->i2c_write_replace = &mraa_afb_i2c_write_replace;
    b->adv_func->i2c_write_byte_replace = &mraa_afb_i2c_write_byte_replace;
    b->adv_func->i2c_write_byte_data_replace = &mraa_afb_i2c_write_byte_data_replace;
    b->adv_func->i2c_write_word_data_replace = &mraa_afb_i2c_write_word_data_replace;

    int rc;
    char uri[500];
    sd_event *loop;

    /*get port and token from the command arg*/
    char *port = "1521";
    char *token = "x";

    /*Generate uri*/
    sprintf (uri, "127.0.0.1:%s/api?token=%s", port, token);

    rc = sd_event_default(&loop);
    if (rc < 0) {
        fprintf(stderr, "connection to default event loop failed: %s\n", strerror(-rc));
        return NULL;
    }

    /* connect the websocket wsj1 to the uri given by the first argument */
    wsj1 = afb_ws_client_connect_wsj1(loop, uri, &itf, NULL);
    if (wsj1 == NULL) {
        fprintf(stderr, "connection to %s failed: %m\n", uri);
        return NULL;
    }

    return b;
}

mraa_platform_t
mraa_afb_platform()
{
    plat = mraa_afb_board();

    return MRAA_AFB_PLATFORM;
}
