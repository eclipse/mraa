/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Longwei Su <lsu@ics.com>
 * Copyright (c) 2015-2016 Intel Corporation.
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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <error.h>
#include <sys/types.h>

#include <json-c/json.h>

#include <argp.h>

#include <pwd.h>

#include <mraa/uart.h>
#include <mraa/gpio.h>
#include <mraa/i2c.h>
#include <mraa/pwm.h>

#include <mraa_internal.h>

#include <imraa_arduino.h>

#define IMRAA_CONF_FILE "/etc/imraa.conf"

#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64

typedef struct mraa_io_objects_t {
    const char* type;
    int index;
    bool raw;
    const char* label;
    const char* user;
} mraa_io_objects_t;

void
imraa_handle_IO(struct json_object* jobj)
{
    struct mraa_io_objects_t* mraa_io_obj;
    struct json_object* ioarray;
    int ionum = 0;
    int i;
    if (json_object_object_get_ex(jobj, "IO", &ioarray) == true) {
        ionum = json_object_array_length(ioarray);
        printf("Length of IO array is %d\n", ionum);
        mraa_io_obj = (mraa_io_objects_t*) calloc(ionum, sizeof(mraa_io_objects_t));
        if (!mraa_io_obj) {
            printf("imraa_handle_IO malloc failed\n");
            exit(1);
        }
        int index2 = -1;
        // optional index for io configuration;
        if (json_object_is_type(ioarray, json_type_array)) {
            for (i = 0; i < ionum; i++) {
                struct json_object* ioobj = json_object_array_get_idx(ioarray, i);
                struct json_object* x;
                if (json_object_object_get_ex(ioobj, "type", &x) == true) {
                    mraa_io_obj[i].type = json_object_get_string(x);
                }
                if (json_object_object_get_ex(ioobj, "index", &x) == true) {
                    mraa_io_obj[i].index = json_object_get_int(x);
                }
                if (json_object_object_get_ex(ioobj, "raw", &x) == true) {
                    mraa_io_obj[i].raw = json_object_get_boolean(x);
                }
                if (json_object_object_get_ex(ioobj, "label", &x) == true) {
                    mraa_io_obj[i].label = json_object_get_string(x);
                }
                if (json_object_object_get_ex(ioobj, "index2", &x) == true) {
                    index2 = json_object_get_int(x);
                }
                if (json_object_object_get_ex(ioobj, "user", &x) == true) {
                    mraa_io_obj[i].user = json_object_get_string(x);
                }
                // starting io configuration
                if (strcmp(mraa_io_obj[i].type, "gpio") == 0) {
                    mraa_gpio_context gpio = NULL;
                    if (mraa_io_obj[i].raw) {
                        gpio = mraa_gpio_init_raw(mraa_io_obj[i].index);
                    } else {
                        gpio = mraa_gpio_init(mraa_io_obj[i].index);
                    }
                    if (gpio != NULL) {
                        if (mraa_io_obj[i].user != NULL) {
                            int sysfs_gpio_pin = mraa_gpio_get_pin_raw(gpio);
                            if (sysfs_gpio_pin >= 0) {
                                char bu[MAX_SIZE];
                                snprintf(bu, MAX_SIZE, SYSFS_CLASS_GPIO "/gpio%d/value", sysfs_gpio_pin);
                                struct passwd* user = getpwnam(mraa_io_obj[i].user);
                                if (user != NULL) {
                                    if (chown(bu, user->pw_uid, user->pw_gid) == -1) {
                                        fprintf(stderr, "Couldn't change ownership of file %s to %d:%d\n", bu, user->pw_uid, user->pw_gid);
                                    } else {
                                        fprintf(stderr, "Invalid user %s\n", mraa_io_obj[i].user);
                                    }
                                }
                            }
                            mraa_result_t r = mraa_gpio_owner(gpio, 0);
                            if (r != MRAA_SUCCESS) {
                                mraa_result_print(r);
                            }
                            mraa_gpio_close(gpio);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "i2c") == 0) {
                        mraa_i2c_context i2c = NULL;
                        if (mraa_io_obj[i].raw) {
                            i2c = mraa_i2c_init_raw(mraa_io_obj[i].index);
                        } else {
                            i2c = mraa_i2c_init(mraa_io_obj[i].index);
                        }
                        if (i2c != NULL) {
                            mraa_i2c_stop(i2c);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "pwm") == 0) {
                        mraa_pwm_context pwm = NULL;
                        if (mraa_io_obj[i].raw) {
                            pwm = mraa_pwm_init_raw(index2, mraa_io_obj[i].index);
                        } else {
                            pwm = mraa_pwm_init(mraa_io_obj[i].index);
                        }
                        if (pwm != NULL) {
                            mraa_result_t r = mraa_pwm_owner(pwm, 0);
                            if (r != MRAA_SUCCESS) {
                                mraa_result_print(r);
                            }
                            mraa_pwm_close(pwm);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "spi") == 0) {
                        mraa_spi_context spi = NULL;
                        if (mraa_io_obj[i].raw) {
                            spi = mraa_spi_init_raw(mraa_io_obj[i].index, index2);
                        } else {
                            spi = mraa_spi_init(mraa_io_obj[i].index);
                        }
                        if (spi != NULL) {
                            mraa_spi_stop(spi);
                        }
                    } else if (strcmp(mraa_io_obj[i].type, "uart") == 0) {
                        mraa_uart_context uart = NULL;
                        if (mraa_io_obj[i].raw) {
                            uart = mraa_uart_init_raw(mraa_io_obj[i].label);
                        } else {
                            uart = mraa_uart_init(mraa_io_obj[i].index);
                        }
                        if (uart != NULL) {
                            mraa_uart_stop(uart);
                        }
                    }
                }
            }
        } else {
            fprintf(stderr, "IO array incorrectly parsed\n");
        }
        free(mraa_io_obj);
    }
}

int
imraa_check_version(struct json_object* jobj)
{
    struct json_object* imraa_version;
    if (json_object_object_get_ex(jobj, "version", &imraa_version) == true) {
        if (json_object_is_type(imraa_version, json_type_string)) {
            printf("imraa version is %s good\n", json_object_get_string(imraa_version));
            // TODO check version?
        } else {
            fprintf(stderr, "version string incorrectly parsed\n");
            return -1;
        }
    } else {
        fprintf(stderr, "no version specified\n");
    }
    return 0;
}

const char *argp_program_version = "imraa version 2.0";
const char *argp_program_bug_address = "http://github.com/intel-iot-devkit/mraa/issues";

static char doc[] = "somestuff";

static char args_doc[] = "-i imraa.conf";

static struct argp_option options[] = {
    {"verbose",  'v', 0,      0,  "Produce verbose output" },
    {"quiet",    'q', 0,      0,  "Don't produce any output" },
    {"silent",   's', 0,      OPTION_ALIAS },
    {"force",    'f', 0,      0,  "Force update" },
    {"arduino",  'a', 0,      0,  "Arduino detection" },
    {"input",    'i', "FILE", 0,
     "configuration file" },
    { 0 }
};

struct arguments
{
    char *args[2];
    int silent, verbose, force, arduino;
    char *input_file;
};

static error_t
parse_opt(int key, char *arg, struct argp_state *state)
{

    /* Get the input argument from argp_parse, which we
     know is a pointer to our arguments structure. */
    struct arguments *arguments = state->input;

    switch(key) {
        case 'q': case 's':
            arguments->silent = 1;
            break;
        case 'v':
            arguments->verbose = 1;
            break;
        case 'a':
            arguments->arduino = 1;
            break;
        case 'f':
            arguments->force = 1;
            break;
        case 'i':
            arguments->input_file = arg;
            break;
        case ARGP_KEY_ARG:
            if (state->arg_num >= 2) {
                argp_usage(state);
            }
            arguments->args[state->arg_num] = arg;
            break;
        case ARGP_KEY_END:
            if (state->arg_num < 0) {
                /* Not enough arguments. */
                argp_usage (state);
            }
            break;
        default:
            return ARGP_ERR_UNKNOWN;
    }
    return 0;
}

static struct argp argp = { options, parse_opt, args_doc, doc};

int
main(int argc, char** argv)
{
    char* buffer = NULL;
    long fsize;
    struct arguments arguments;

    arguments.silent = 0;
    arguments.verbose = 0;
    arguments.force = 0;
    arguments.input_file = IMRAA_CONF_FILE;

    argp_parse(&argp, argc, argv, 0, 0, &arguments);

    FILE* fh = fopen(arguments.input_file, "r");
    if (fh == NULL) {
        fprintf(stderr, "Failed to open configuration file\n");
        return EXIT_FAILURE;
    }

    fseek(fh, 0, SEEK_END);
    fsize = ftell(fh) + 1;
    fseek(fh, 0, SEEK_SET);
    buffer = calloc(fsize, sizeof(char));
    if (buffer != NULL) {
        int result = fread(buffer, sizeof(char), fsize, fh);
        if (result != (fsize - 1)) {
            printf("imraa conf reading error\n");
        }
    } else {
        printf("imraa read_conf buffer can't allocated\n");
        exit(1);
    }

    // call reduced imraa_init (not that mraa_init) will already have been called
    imraa_init();

    json_object* jobj = json_tokener_parse(buffer);
    if (imraa_check_version(jobj) != 0) {
        printf("version of configuration file is not compatible, please check again\n");
    } else {
        mraa_platform_t type = mraa_get_platform_type();

        if (arguments.arduino) {
            imraa_handle_subplatform(jobj, arguments.force);
        }

        if (type == MRAA_NULL_PLATFORM || type == MRAA_UNKNOWN_PLATFORM) {
            printf("imraa: attempting to do IO pinmuxing on null/unkown platform\n");
        }
        imraa_handle_IO(jobj);
    }

    fclose(fh);
    json_object_put(jobj);
    free(buffer);
    return EXIT_SUCCESS;
}
