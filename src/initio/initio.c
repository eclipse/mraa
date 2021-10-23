/*
 * Author: Mihai Stefanescu <mihai.stefanescu@rinftech.com>
 * Copyright (c) 2018 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "initio.h"
#include "initio/initio_keys.h"
#include "mraa_internal.h"

#define DESC_SEP ","
#define TOK_SEP ":"

static mraa_result_t
mraa_atoi_x(const char* intStr, char** str_end, int* value, int base)
{
    char* lendptr;
    long val = strtol(intStr, &lendptr, base);

    /* Test for no-conversion */
    if (intStr == lendptr) {
        return MRAA_ERROR_UNSPECIFIED;
    }

    /* Test for overflow/underflow for a long int */
    if (errno == ERANGE || val > INT_MAX || val < INT_MIN) {
        *value = 0;
        return MRAA_ERROR_UNSPECIFIED;
    }

    if (str_end) {
        *str_end = lendptr;
    }

    *value = (int) val;
    return MRAA_SUCCESS;
}

static char**
mraa_tokenize_string(const char* str, const char* delims, int* num_tokens)
{
    char *saveptr, *tok, *s, *p_str;
    char** output = NULL;
    size_t output_size = 0;

    p_str = strdup(str);

    for (s = p_str;; s = NULL) {
        tok = strtok_r(s, delims, &saveptr);
        if (tok == NULL) {
            break;
        }
        output = realloc(output, (++output_size) * sizeof(char*));
        output[output_size - 1] = calloc(strlen(tok) + 1, sizeof(char));
        strncpy(output[output_size - 1], tok, strlen(tok));
    }
    *num_tokens = output_size;

    free(p_str);

    return output;
}

static void
mraa_delete_tokenized_string(char** str, int num_tokens)
{
    if (str == NULL) {
        syslog(LOG_ERR, "mraa_delete_tokenized_string: NULL string");
        return;
    }

    for (int i = 0; i < num_tokens; ++i) {
        free(str[i]);
    }

    free(str);
}

static mraa_uart_ow_context
parse_uart_ow(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_uart_ow: invalid parameters");
        return NULL;
    }

    mraa_uart_ow_context dev = NULL;

    int bus = -1;
    if (proto[1] && (mraa_atoi_x(proto[1], NULL, &bus, 0) == MRAA_SUCCESS)) {
        dev = mraa_uart_ow_init(bus);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_uart_ow: could not init uart_ow bus '%d' from '%s'", bus, proto_full);
        }
    } else {
        syslog(LOG_ERR, "parse_uart_ow: invalid uart_ow bus number '%s' from '%s'", proto[1], proto_full);
    }

    return dev;
}

static mraa_uart_context
parse_uart(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_uart: invalid parameters");
        return NULL;
    }

    int idx = 1;
    mraa_uart_context dev = NULL;

    int uart = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &uart, 0) == MRAA_SUCCESS)) {
        dev = mraa_uart_init(uart);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_uart: could not init uart index '%d' from '%s'", uart, proto_full);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    } else {
        syslog(LOG_ERR, "parse_uart: invalid uart index '%s' from '%s'", proto[idx], proto_full);
        return NULL;
    }

    /* Check for baudrate. */
    unsigned int baudrate = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, (int*) &baudrate, 0) == MRAA_SUCCESS)) {
        if (mraa_uart_set_baudrate(dev, baudrate) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_uart: could not set uart baudrate '%d' from '%s'", baudrate, proto_full);
            mraa_uart_stop(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    } else {
        syslog(LOG_ERR, "parse_uart: invalid uart baudrate '%s' from '%s'", proto[idx], proto_full);
    }

    /* Check for mode - [int] bytesize, [mraa_uart_parity_t] parity, [int] stopbits. */
    char* end = NULL;

    /* Check for bytesize. */
    int bytesize = -1;
    if (proto[idx] != NULL && mraa_atoi_x(proto[idx], &end, &bytesize, 0) != MRAA_SUCCESS) {
        syslog(LOG_ERR, "parse_uart: error reading uart bytesize '%d' from '%s'", bytesize, proto_full);
        mraa_uart_stop(dev);
        return NULL;
    }

    if (end == NULL) {
        return NULL;
    }

    int parity = -1;
    if (strncmp(end, U_PARITY_NONE, strlen(U_PARITY_NONE)) == 0) {
        parity = MRAA_UART_PARITY_NONE;
        end += strlen(U_PARITY_NONE);
    } else if (strncmp(end, U_PARITY_EVEN, strlen(U_PARITY_EVEN)) == 0) {
        parity = MRAA_UART_PARITY_EVEN;
        end += strlen(U_PARITY_EVEN);
    } else if (strncmp(end, U_PARITY_ODD, strlen(U_PARITY_ODD)) == 0) {
        parity = MRAA_UART_PARITY_ODD;
        end += strlen(U_PARITY_ODD);
    } else if (strncmp(end, U_PARITY_MARK, strlen(U_PARITY_MARK)) == 0) {
        parity = MRAA_UART_PARITY_MARK;
        end += strlen(U_PARITY_MARK);
    } else if (strncmp(end, U_PARITY_SPACE, strlen(U_PARITY_SPACE)) == 0) {
        parity = MRAA_UART_PARITY_SPACE;
        end += strlen(U_PARITY_SPACE);
    }

    if (parity == -1) {
        syslog(LOG_ERR, "parse_uart: error reading uart parity '%s' from '%s'", end, proto_full);
        mraa_uart_stop(dev);
        return NULL;
    }

    int stopbits = -1;
    if (mraa_atoi_x(end, NULL, &stopbits, 0) != MRAA_SUCCESS) {
        syslog(LOG_ERR, "parse_uart: error reading uart bytesize '%d' from '%s'", bytesize, proto_full);
        mraa_uart_stop(dev);
        return NULL;
    }

    if (mraa_uart_set_mode(dev, bytesize, (mraa_uart_parity_t) parity, stopbits) != MRAA_SUCCESS) {
        syslog(LOG_ERR, "parse_uart: error setting up uart mode '%s' from '%s'", proto[idx], proto_full);
        mraa_uart_stop(dev);
        return NULL;
    }

    return dev;
}

static mraa_spi_context
parse_spi(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_spi: invalid parameters");
        return NULL;
    }

    int idx = 1;
    mraa_spi_context dev = NULL;

    int bus = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &bus, 0) == MRAA_SUCCESS)) {
        dev = mraa_spi_init(bus);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_spi: could not init spi bus '%d' from '%s'", bus, proto_full);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    } else {
        syslog(LOG_ERR, "parse_spi: invalid spi bus number '%s' from '%s'", proto[idx], proto_full);
        return NULL;
    }

    /* Check for mode. */
    int mode = -1;
    if (strncmp(proto[idx], S_MODE_0, strlen(S_MODE_0)) == 0) {
        mode = MRAA_SPI_MODE0;
    } else if (strncmp(proto[idx], S_MODE_1, strlen(S_MODE_1)) == 0) {
        mode = MRAA_SPI_MODE1;
    } else if (strncmp(proto[idx], S_MODE_2, strlen(S_MODE_2)) == 0) {
        mode = MRAA_SPI_MODE2;
    } else if (strncmp(proto[idx], S_MODE_3, strlen(S_MODE_3)) == 0) {
        mode = MRAA_SPI_MODE3;
    }

    if (mode != -1) {
        if (mraa_spi_mode(dev, (mraa_spi_mode_t) mode) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_spi: error setting up spi mode '%s' from '%s'", proto[idx], proto_full);
            mraa_spi_stop(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    int frequency;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &frequency, 0) == MRAA_SUCCESS)) {
        if (mraa_spi_frequency(dev, frequency) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_spi: error setting up spi frequency '%d' from '%s'", frequency, proto_full);
            mraa_spi_stop(dev);
            return NULL;
        }
    } else {
        syslog(LOG_ERR, "parse_spi: invalid spi frequency '%s' from '%s'", proto[idx], proto_full);
    }

    return dev;
}

static mraa_pwm_context
parse_pwm(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_pwm: invalid parameters");
        return NULL;
    }

    mraa_pwm_context dev = NULL;

    int pin = -1;
    if (proto[1] && (mraa_atoi_x(proto[1], NULL, &pin, 0) == MRAA_SUCCESS)) {
        dev = mraa_pwm_init(pin);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_pwm: could not init pwm pin %d", pin);
        }
    } else {
        syslog(LOG_ERR, "parse_pwm: invalid pwm pin number");
    }

    return dev;
}

#if !defined(PERIPHERALMAN)
static mraa_iio_context
parse_iio(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_iio: invalid parameters");
        return NULL;
    }

    mraa_iio_context dev = NULL;

    int device = -1;
    if (proto[1] && (mraa_atoi_x(proto[1], NULL, &device, 0) == MRAA_SUCCESS)) {
        dev = mraa_iio_init(device);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_iio: could not init iio device '%d' from '%s'", device, proto_full);
        }
    } else {
        syslog(LOG_ERR, "parse_iio: invalid iio device number '%s' from '%s'", proto[1], proto_full);
    }

    return dev;
}
#endif

static mraa_i2c_context
parse_i2c(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_i2c: invalid parameters");
        return NULL;
    }

    int idx = 1;
    mraa_i2c_context dev = NULL;

    int bus = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &bus, 0) == MRAA_SUCCESS)) {
        dev = mraa_i2c_init(bus);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_i2c: could not init i2c bus '%d' from '%s'", bus, proto_full);
            return NULL;
        } else {
            if (++idx == n)
                return dev;
        }
    } else {
        syslog(LOG_ERR, "parse_i2c: invalid i2c bus number '%s' from '%s'", proto[idx], proto_full);
        return NULL;
    }

    int address = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &address, 0) == MRAA_SUCCESS)) {
        if (mraa_i2c_address(dev, (uint8_t) address) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_i2c: error setting up i2c address '0x%02x' from '%s'", address, proto_full);
            mraa_i2c_stop(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    int mode = -1;
    if (proto[idx] && strncmp(proto[idx], I_MODE_STD, strlen(I_MODE_STD)) == 0) {
        mode = MRAA_I2C_STD;
    } else if (proto[idx] && strncmp(proto[idx], I_MODE_FAST, strlen(I_MODE_FAST)) == 0) {
        mode = MRAA_I2C_FAST;
    } else if (proto[idx] && strncmp(proto[idx], I_MODE_HIGH, strlen(I_MODE_HIGH)) == 0) {
        mode = MRAA_GPIO_PULLDOWN;
    }

    if (mode != -1) {
        if (mraa_i2c_frequency(dev, (mraa_i2c_mode_t) mode) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_i2c: error setting up gpio driver mode '%s' from '%s'", proto[idx], proto_full);
            mraa_i2c_stop(dev);
            return NULL;
        }
    }

    return dev;
}

static mraa_aio_context
parse_aio(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_aio: invalid parameters");
        return NULL;
    }

    int idx = 1;
    mraa_aio_context dev = NULL;

    int aio_num = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &aio_num, 0) == MRAA_SUCCESS)) {
        dev = mraa_aio_init(aio_num);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_aio: could not init aio number '%d' from '%s'", aio_num, proto_full);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    } else {
        syslog(LOG_ERR, "parse_aio: failed to parse '%s' as integer from '%s'", proto[idx], proto_full);
        return NULL;
    }

    int num_bits = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &num_bits, 0) == MRAA_SUCCESS)) {
        if (mraa_aio_set_bit(dev, num_bits) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_aio: error setting up aio bits '%d' from '%s'", num_bits, proto_full);
            mraa_aio_close(dev);
            return NULL;
        }
    } else {
        syslog(LOG_ERR, "parse_aio: invalid aio bit number '%s' from '%s'", proto[idx], proto_full);
        mraa_aio_close(dev);
        return NULL;
    }

    return dev;
}

static mraa_gpio_context
parse_gpio(char** proto, size_t n, const char* proto_full)
{
    if (!proto || (n <= 1) || (!proto_full)) {
        syslog(LOG_ERR, "parse_gpio: invalid parameters");
        return NULL;
    }

    int idx = 1;
    mraa_gpio_context dev = NULL;

    int gpio_num = -1;
    if (proto[idx] && (mraa_atoi_x(proto[idx], NULL, &gpio_num, 0) == MRAA_SUCCESS)) {
        dev = mraa_gpio_init(gpio_num);
        if (dev == NULL) {
            syslog(LOG_ERR, "parse_gpio: could not init gpio number '%d' from '%s'", gpio_num, proto_full);
            return NULL;
        }
    } else {
        syslog(LOG_ERR, "parse_gpio: invalid gpio number '%s' from '%s'", proto[idx], proto_full);
        return NULL;
    }
    if (++idx == n) {
        return dev;
    }

    /* Check for direction. */
    int dir = -1;
    if (strncmp(proto[idx], G_DIR_OUT, strlen(G_DIR_OUT)) == 0) {
        dir = MRAA_GPIO_OUT;
    } else if (strncmp(proto[idx], G_DIR_IN, strlen(G_DIR_IN)) == 0) {
        dir = MRAA_GPIO_IN;
    } else if (strncmp(proto[idx], G_DIR_OUT_HIGH, strlen(G_DIR_OUT_HIGH)) == 0) {
        dir = MRAA_GPIO_OUT_HIGH;
    } else if (strncmp(proto[idx], G_DIR_OUT_LOW, strlen(G_DIR_OUT_LOW)) == 0) {
        dir = MRAA_GPIO_OUT_LOW;
    }

    if (dir != -1) {
        if (mraa_gpio_dir(dev, (mraa_gpio_dir_t) dir) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio direction '%s' from '%s'", proto[idx], proto_full);
            mraa_gpio_close(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    } else {
        /* Set direction to default - output. */
        if (mraa_gpio_dir(dev, MRAA_GPIO_OUT) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio direction '%s' from '%s'", G_DIR_OUT, proto_full);
            mraa_gpio_close(dev);
            return NULL;
        }
    }

    /* Check the value. */
    int value = -1;
    if (mraa_atoi_x(proto[idx], NULL, &value, 0) == MRAA_SUCCESS) {
        if (mraa_gpio_write(dev, value) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: could not init gpio number '%d' with value '%d' from '%s'",
                   gpio_num, value, proto_full);
            mraa_gpio_close(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    /* Check for mode. */
    int mode = -1;
    if (strncmp(proto[idx], G_MODE_STRONG, strlen(G_MODE_STRONG)) == 0) {
        mode = MRAA_GPIO_STRONG;
    } else if (strncmp(proto[idx], G_MODE_PULLUP, strlen(G_MODE_PULLUP)) == 0) {
        mode = MRAA_GPIO_PULLUP;
    } else if (strncmp(proto[idx], G_MODE_PULLDOWN, strlen(G_MODE_PULLDOWN)) == 0) {
        mode = MRAA_GPIO_PULLDOWN;
    } else if (strncmp(proto[idx], G_MODE_HIZ, strlen(G_MODE_HIZ)) == 0) {
        mode = MRAA_GPIO_HIZ;
    } else if (strncmp(proto[idx], G_MODE_ACTIVE_LOW, strlen(G_MODE_ACTIVE_LOW)) == 0) {
        mode = MRAA_GPIOD_ACTIVE_LOW;
    } else if (strncmp(proto[idx], G_MODE_OPEN_DRAIN, strlen(G_MODE_OPEN_DRAIN)) == 0) {
        mode = MRAA_GPIOD_OPEN_DRAIN;
    } else if (strncmp(proto[idx], G_MODE_OPEN_SOURCE, strlen(G_MODE_OPEN_SOURCE)) == 0) {
        mode = MRAA_GPIOD_OPEN_SOURCE;
    }

    if (mode != -1) {
        if (mraa_gpio_mode(dev, (mraa_gpio_mode_t) mode) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio mode '%s' from '%s'", proto[idx], proto_full);
            mraa_gpio_close(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    /* Check for edge. */
    int edge = -1;
    if (strncmp(proto[idx], G_EDGE_NONE, strlen(G_EDGE_NONE)) == 0) {
        edge = MRAA_GPIO_EDGE_NONE;
    } else if (strncmp(proto[idx], G_EDGE_BOTH, strlen(G_EDGE_BOTH)) == 0) {
        edge = MRAA_GPIO_EDGE_BOTH;
    } else if (strncmp(proto[idx], G_EDGE_RISING, strlen(G_EDGE_RISING)) == 0) {
        edge = MRAA_GPIO_EDGE_RISING;
    } else if (strncmp(proto[idx], G_EDGE_FALLING, strlen(G_EDGE_FALLING)) == 0) {
        edge = MRAA_GPIO_EDGE_FALLING;
    }

    if (edge != -1) {
        if (mraa_gpio_edge_mode(dev, (mraa_gpio_edge_t) edge) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio edge '%s' from '%s'", proto[idx], proto_full);
            mraa_gpio_close(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    /* Check for input mode. */
    int input_mode = -1;
    if (strncmp(proto[idx], G_INPUT_ACTIVE_HIGH, strlen(G_INPUT_ACTIVE_HIGH)) == 0) {
        input_mode = MRAA_GPIO_ACTIVE_HIGH;
    } else if (strncmp(proto[idx], G_INPUT_ACTIVE_LOW, strlen(G_INPUT_ACTIVE_LOW)) == 0) {
        input_mode = MRAA_GPIO_ACTIVE_LOW;
    }

    if (input_mode != -1) {
        if (mraa_gpio_input_mode(dev, (mraa_gpio_input_mode_t) input_mode) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio input mode '%s' from '%s'", proto[idx], proto_full);
            mraa_gpio_close(dev);
            return NULL;
        } else {
            if (++idx == n) {
                return dev;
            }
        }
    }

    /* Check for driver output mode. */
    int driver_mode = -1;
    if (strncmp(proto[idx], G_OUTPUT_OPEN_DRAIN, strlen(G_OUTPUT_OPEN_DRAIN)) == 0) {
        driver_mode = MRAA_GPIO_OPEN_DRAIN;
    } else if (strncmp(proto[idx], G_OUTPUT_PUSH_PULL, strlen(G_OUTPUT_PUSH_PULL)) == 0) {
        driver_mode = MRAA_GPIO_PUSH_PULL;
    }

    if (driver_mode != -1) {
        if (mraa_gpio_out_driver_mode(dev, (mraa_gpio_out_driver_mode_t) driver_mode) != MRAA_SUCCESS) {
            syslog(LOG_ERR, "parse_gpio: error setting up gpio driver mode '%s' from '%s'", proto[idx], proto_full);
            mraa_gpio_close(dev);
            return NULL;
        }
    }

    return dev;
}

mraa_result_t
mraa_io_init(const char* strdesc, mraa_io_descriptor** desc)
{
    mraa_result_t status = MRAA_SUCCESS;
    size_t leftover_str_len = 0;
    mraa_io_descriptor* new_desc = calloc(1, sizeof(mraa_io_descriptor));
    if (new_desc == NULL) {
        syslog(LOG_ERR, "mraa_io_init: Failed to allocate memory for context");
        desc = NULL;
        return MRAA_ERROR_NO_RESOURCES;
    }

    int num_descs = 0;
    char** str_descs = mraa_tokenize_string(strdesc, DESC_SEP, &num_descs);
    for (int i = 0; i < num_descs; ++i) {
        int num_desc_tokens = 0;
        char** str_tokens = mraa_tokenize_string(str_descs[i], TOK_SEP, &num_desc_tokens);

        if (strncmp(str_tokens[0], AIO_KEY, strlen(AIO_KEY)) == 0 && strlen(str_tokens[0]) == strlen(AIO_KEY)) {
            mraa_aio_context dev = parse_aio(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing aio init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->aios = realloc(new_desc->aios, sizeof(mraa_aio_context) * (new_desc->n_aio + 1));
                if (!new_desc->aios) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for aio");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->aios[new_desc->n_aio++] = dev;
                }
            }
        } else if (strncmp(str_tokens[0], GPIO_KEY, strlen(GPIO_KEY)) == 0 &&
                   strlen(str_tokens[0]) == strlen(GPIO_KEY)) {
            mraa_gpio_context dev = parse_gpio(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing gpio init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->gpios =
                realloc(new_desc->gpios, sizeof(mraa_gpio_context) * (new_desc->n_gpio + 1));
                if (!new_desc->gpios) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for gpio");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->gpios[new_desc->n_gpio++] = dev;
                }
            }
        }
#if !defined(PERIPHERALMAN)
        else if (strncmp(str_tokens[0], IIO_KEY, strlen(IIO_KEY)) == 0 &&
                 strlen(str_tokens[0]) == strlen(IIO_KEY)) {
            mraa_iio_context dev = parse_iio(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing iio init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->iios = realloc(new_desc->iios, sizeof(mraa_iio_context) * (new_desc->n_iio + 1));
                if (!new_desc->iios) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for iio");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->iios[new_desc->n_iio++] = dev;
                }
            }
        }
#endif
        else if (strncmp(str_tokens[0], I2C_KEY, strlen(I2C_KEY)) == 0 &&
                 strlen(str_tokens[0]) == strlen(I2C_KEY)) {
            mraa_i2c_context dev = parse_i2c(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing i2c init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->i2cs = realloc(new_desc->i2cs, sizeof(mraa_i2c_context) * (new_desc->n_i2c + 1));
                if (!new_desc->i2cs) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for i2c");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->i2cs[new_desc->n_i2c++] = dev;
                }
            }
        } else if (strncmp(str_tokens[0], PWM_KEY, strlen(PWM_KEY)) == 0 &&
                   strlen(str_tokens[0]) == strlen(PWM_KEY)) {
            mraa_pwm_context dev = parse_pwm(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing pwm init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->pwms = realloc(new_desc->pwms, sizeof(mraa_pwm_context) * (new_desc->n_pwm + 1));
                if (!new_desc->pwms) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for pwm");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->pwms[new_desc->n_pwm++] = dev;
                }
            }
        } else if (strncmp(str_tokens[0], SPI_KEY, strlen(SPI_KEY)) == 0 &&
                   strlen(str_tokens[0]) == strlen(SPI_KEY)) {
            mraa_spi_context dev = parse_spi(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing spi init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->spis = realloc(new_desc->spis, sizeof(mraa_spi_context) * (new_desc->n_spi + 1));
                if (!new_desc->spis) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for spi");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->spis[new_desc->n_spi++] = dev;
                }
            }
        } else if (strncmp(str_tokens[0], UART_KEY, strlen(UART_KEY)) == 0 &&
                   strlen(str_tokens[0]) == strlen(UART_KEY)) {
            mraa_uart_context dev = parse_uart(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing uart init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->uarts =
                realloc(new_desc->uarts, sizeof(mraa_uart_context) * (new_desc->n_uart + 1));
                if (!new_desc->uarts) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for uart");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->uarts[new_desc->n_uart++] = dev;
                }
            }
        } else if (strncmp(str_tokens[0], UART_OW_KEY, strlen(UART_OW_KEY)) == 0 &&
                   strlen(str_tokens[0]) == strlen(UART_OW_KEY)) {
            mraa_uart_ow_context dev = parse_uart_ow(str_tokens, num_desc_tokens, str_descs[i]);
            if (!dev) {
                syslog(LOG_ERR, "mraa_io_init: error parsing uart_ow init string '%s'", str_descs[i]);
                status = MRAA_ERROR_INVALID_HANDLE;
                free(new_desc);
            }

            if (status == MRAA_SUCCESS) {
                new_desc->uart_ows =
                realloc(new_desc->uart_ows, sizeof(mraa_uart_ow_context) * (new_desc->n_uart_ow + 1));
                if (!new_desc->uart_ows) {
                    syslog(LOG_ERR, "mraa_io_init: error allocating memory for uart_ow");
                    status = MRAA_ERROR_NO_RESOURCES;
                    free(new_desc);
                } else {
                    new_desc->uart_ows[new_desc->n_uart_ow++] = dev;
                }
            }
        } else {
            /* Here we build the leftover string. */
            new_desc->leftover_str =
            realloc(new_desc->leftover_str, sizeof(char) * (leftover_str_len + strlen(str_descs[i]) + 2));
            if (!new_desc->leftover_str) {
                syslog(LOG_ERR, "mraa_io_init: error allocating memory for leftover string");
                status = MRAA_ERROR_NO_RESOURCES;
                free(new_desc);
            } else {
                if (leftover_str_len == 0) {
                    strncpy(new_desc->leftover_str, str_descs[i], strlen(str_descs[i]));
                } else {
                    strncat(new_desc->leftover_str, str_descs[i], strlen(str_descs[i]));
                }

                leftover_str_len += strlen(str_descs[i]) + 1;
                new_desc->leftover_str[leftover_str_len - 1] = ',';
                new_desc->leftover_str[leftover_str_len] = '\0';
            }
        }

        mraa_delete_tokenized_string(str_tokens, num_desc_tokens);

        if (status != MRAA_SUCCESS) {
            break;
        }
    }
    mraa_delete_tokenized_string(str_descs, num_descs);

    if ((status == MRAA_SUCCESS) && (new_desc->leftover_str)) {
        /* We don't need the last comma. */
        new_desc->leftover_str[leftover_str_len - 1] = '\0';
    }

    if (status == MRAA_SUCCESS) {
        *desc = new_desc;
    }

    return status;
}

mraa_result_t
mraa_io_close(mraa_io_descriptor* desc)
{
    if (desc == NULL) {
        syslog(LOG_ERR, "mraa_io_close: NULL mraa_io_descriptor");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    for (int i = 0; i < desc->n_aio; ++i) {
        mraa_aio_close(desc->aios[i]);
    }
    if (desc->n_aio) {
        free(desc->aios);
    }

    for (int i = 0; i < desc->n_gpio; ++i) {
        mraa_gpio_close(desc->gpios[i]);
    }
    if (desc->n_gpio) {
        free(desc->gpios);
    }

    for (int i = 0; i < desc->n_i2c; ++i) {
        mraa_i2c_stop(desc->i2cs[i]);
    }
    if (desc->n_i2c) {
        free(desc->i2cs);
    }

#if !defined(PERIPHERALMAN)
    for (int i = 0; i < desc->n_iio; ++i) {
        mraa_iio_close(desc->iios[i]);
    }
    if (desc->n_iio) {
        free(desc->iios);
    }
#endif

    for (int i = 0; i < desc->n_pwm; ++i) {
        mraa_pwm_close(desc->pwms[i]);
    }
    if (desc->n_pwm) {
        free(desc->pwms);
    }

    for (int i = 0; i < desc->n_spi; ++i) {
        mraa_spi_stop(desc->spis[i]);
    }
    if (desc->n_spi) {
        free(desc->spis);
    }

    for (int i = 0; i < desc->n_uart; ++i) {
        mraa_uart_stop(desc->uarts[i]);
    }
    if (desc->n_uart) {
        free(desc->uarts);
    }

    for (int i = 0; i < desc->n_uart_ow; ++i) {
        mraa_uart_ow_stop(desc->uart_ows[i]);
    }
    if (desc->n_uart_ow) {
        free(desc->uart_ows);
    }

    if (desc->leftover_str) {
        free(desc->leftover_str);
    }

    free(desc);

    return MRAA_SUCCESS;
}
