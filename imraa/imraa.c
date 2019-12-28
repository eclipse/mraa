/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Author: Longwei Su <lsu@ics.com>
 * Copyright (c) 2015-2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
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

#include <mraa_internal.h>

#include <imraa_arduino.h>
#include <imraa_io.h>
#include <imraa_lock.h>

#define IMRAA_CONF_FILE "/etc/imraa.conf"

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
            break;
        case ARGP_KEY_END:
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
