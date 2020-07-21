/*
 * Author: Brendan Le Foll <brendan.le.foll@intel.com>
 * Copyright (c) 2016 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <syslog.h>
#include <poll.h>
#include <errno.h>

#include "mraa_internal.h"
#include "peripheralmanager/peripheralman.h"

#define UART_PATH_SIZE 11

APeripheralManagerClient *client = NULL;
char **gpios = NULL;
int gpios_count = 0;
char **i2c_busses = NULL;
int i2c_busses_count = 0;
char **spi_busses = NULL;
int spi_busses_count = 0;
char **uart_devices = NULL;
int uart_dev_count = 0;
char **pwm_devices = NULL;
int pwm_dev_count = 0;
const char uart_dev_path_prefix[UART_PATH_SIZE] = "/dev/ttyS";

static mraa_result_t
mraa_pman_pwm_init_raw_replace(mraa_pwm_context dev, int pin)
{
    if (APeripheralManagerClient_openPwm(client, pwm_devices[pin], &dev->bpwm) != 0) {
        APwm_delete(dev->bpwm);
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_pwm_period_replace(mraa_pwm_context dev, int period)
{
    if (!dev) {
        syslog(LOG_ERR, "pwm: stop: context is NULL");
        return 0;
    }

    if (APwm_setFrequencyHz(dev->bpwm, period) != 0) {
        return 0;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_pwm_write_replace(mraa_pwm_context dev, float duty)
{
    if (!dev) {
        syslog(LOG_ERR, "pwm: stop: context is NULL");
        return 0;
    }

    // PIO can only take values between 0 <= duty <= 100
    int d = (int) (duty > 100 ) ? 100 : duty;
    if (APwm_setDutyCycle(dev->bpwm, d) != 0) {
        return 0;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_pwm_enable_replace(mraa_pwm_context dev, int enable)
{
    if (!dev) {
        syslog(LOG_ERR, "pwm: stop: context is NULL");
        return 0;
    }

    if (APwm_setEnabled(dev->bpwm, enable) != 0) {
        return 0;
    }

    return MRAA_SUCCESS;
}

static float
mraa_pman_pwm_read_replace(mraa_pwm_context dev)
{
    return -1;
}

static mraa_result_t
mraa_pman_uart_init_raw_replace(mraa_uart_context dev, const char* path)
{
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_uart_init_post(mraa_uart_context dev)
{
    if (APeripheralManagerClient_openUartDevice(client, uart_devices[dev->index], &dev->buart) != 0) {
        AUartDevice_delete(dev->buart);
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_uart_set_baudrate_replace(mraa_uart_context dev, unsigned int baud)
{
    if (!dev) {
        syslog(LOG_ERR, "uart: stop: context is NULL");
        return 0;
    }

    if (AUartDevice_setBaudrate(dev->buart, baud) != 0) {
        return 0;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_uart_flush_replace(mraa_uart_context dev)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_pman_uart_sendbreak_replace(mraa_uart_context dev)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static int
mraa_pman_uart_read_replace(mraa_uart_context dev, char* buf, size_t len)
{
    int rc;
    uint32_t bytes_read;

    if (dev->buart == NULL) {
        return -1;
    }

    rc = AUartDevice_read(dev->buart, buf, len, &bytes_read);
    if (rc != 0) {
        return rc;
    }

    return bytes_read;
}

static int
mraa_pman_uart_write_replace(mraa_uart_context dev, const char* buf, size_t len)
{
    int rc;
    uint32_t bytes_written;

    if (dev->buart == NULL) {
        return -1;
    }

    rc = AUartDevice_write(dev->buart, buf, len, &bytes_written);
    if (rc != 0) {
        return rc;
    }

    return bytes_written;
}

static mraa_result_t
mraa_pman_uart_set_mode_replace(mraa_uart_context dev, int bytesize, mraa_uart_parity_t parity, int stopbits)
{
    int rc;

    if (dev->buart == NULL) {
        return -1;
    }

    rc = AUartDevice_setStopBits(dev->buart, stopbits);
    if (rc != 0) {
        return -1;
    }

    rc = AUartDevice_setDataSize(dev->buart, bytesize);
    if (rc != 0) {
        return -1;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_uart_set_flowcontrol_replace(mraa_uart_context dev, mraa_boolean_t xonxoff, mraa_boolean_t rtscts)
{
    int rc;

    if (dev->buart == NULL) {
        return -1;
    }

    // There are only two modes to choose from.
    // AUART_HARDWARE_FLOW_CONTROL_NONE = 0,       < No hardware flow control
    // AUART_HARDWARE_FLOW_CONTROL_AUTO_RTSCTS = 1 < Auto RTS/CTS
    rc = AUartDevice_setHardwareFlowControl(dev->buart, rtscts ? 1 : 0);
    if (rc != 0) {
        return -1;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_uart_set_non_blocking_replace(mraa_uart_context dev, mraa_boolean_t nonblock)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_pman_uart_set_timeout_replace(mraa_uart_context dev, int read, int write, int interchar)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_boolean_t
mraa_pman_uart_data_available_replace(mraa_uart_context dev, unsigned int millis)
{
    int fd = -1;

    if (AUartDevice_getPollingFd(dev->buart, &fd)) {
        syslog(LOG_ERR, "peripheralman: failed to get the fd");
        return 0;
    }

    struct pollfd poller = {
        .fd = fd,
        .events = POLLIN | POLLERR,
        .revents = 0,
    };

    if (poll(&poller, 1, millis) > 0) {
        syslog(LOG_INFO, "peripheralman: received an event");
        AUartDevice_ackInputEvent(fd);
        return 1;
    } else {
        return 0;
    }
}

static mraa_result_t
mraa_pman_spi_init_raw_replace(mraa_spi_context dev, unsigned int bus, unsigned int cs)
{
    int rc;
    rc = APeripheralManagerClient_openSpiDevice(client, spi_busses[bus], &dev->bspi);
    if (rc != 0) {
        free(dev);
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_mode_replace(mraa_spi_context dev, mraa_spi_mode_t mode)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    switch (mode) {
        case MRAA_SPI_MODE0:
            rc = ASpiDevice_setMode(dev->bspi, ASPI_MODE0);
            break;
        case MRAA_SPI_MODE1:
            rc = ASpiDevice_setMode(dev->bspi, ASPI_MODE1);
            break;
        case MRAA_SPI_MODE2:
            rc = ASpiDevice_setMode(dev->bspi, ASPI_MODE2);
            break;
        case MRAA_SPI_MODE3:
            rc = ASpiDevice_setMode(dev->bspi, ASPI_MODE3);
            break;
        default:
            rc = ASpiDevice_setMode(dev->bspi, ASPI_MODE0);
            break;
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->mode = mode;

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_frequency_replace(mraa_spi_context dev, int hz)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    rc = ASpiDevice_setFrequency(dev->bspi, hz);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->clock = hz;

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_lsbmode_replace(mraa_spi_context dev, mraa_boolean_t lsb)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (lsb) {
        rc = ASpiDevice_setBitJustification(dev->bspi, ASPI_LSB_FIRST);
    } else {
        rc = ASpiDevice_setBitJustification(dev->bspi, ASPI_MSB_FIRST);
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    dev->lsb = lsb;
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_bit_per_word_replace(mraa_spi_context dev, unsigned int bits)
{
    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    if (ASpiDevice_setBitsPerWord(dev->bspi, bits) != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static int
mraa_pman_spi_write_replace(mraa_spi_context dev, uint8_t data)
{
    int rc;
    uint8_t recv = 0;

    if (dev->bspi == NULL) {
        return -1;
    }

    rc = ASpiDevice_transfer(dev->bspi, &data, &recv, 1);
    if (rc != 0) {
        return rc;
    }

    return (int) recv;
}

static int
mraa_pman_spi_write_word_replace(mraa_spi_context dev, uint16_t data)
{
    int rc;
    uint16_t recv = 0;

    if (dev->bspi == NULL) {
        return -1;
    }

    rc = ASpiDevice_transfer(dev->bspi, &data, &recv, 2);
    if (rc != 0) {
        return rc;
    }

    return (int) recv;
}

static mraa_result_t
mraa_pman_spi_transfer_buf_replace(mraa_spi_context dev, uint8_t* data, uint8_t* rxbuf, int length)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    rc = ASpiDevice_transfer(dev->bspi, data, rxbuf, length);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_transfer_buf_word_replace(mraa_spi_context dev, uint16_t* data, uint16_t* rxbuf, int length)
{
    int rc;

    if (dev->bspi == NULL) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    // IS IT CORRECT ?
    rc = ASpiDevice_transfer(dev->bspi, data, rxbuf, length * 2);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_spi_stop_replace(mraa_spi_context dev)
{
    if (dev->bspi != NULL) {
        ASpiDevice_delete(dev->bspi);
        dev->bspi = NULL;
    }

    free(dev);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_i2c_init_bus_replace(mraa_i2c_context dev)
{
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_i2c_stop_replace(mraa_i2c_context dev)
{
    if (dev->bi2c != NULL) {
        AI2cDevice_delete(dev->bi2c);
        dev->bi2c = NULL;
    }

    free(dev);

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_i2c_set_frequency_replace(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_SUPPORTED;
}

static mraa_result_t
mraa_pman_i2c_address_replace(mraa_i2c_context dev, uint8_t addr)
{
    int rc;

    if (dev == NULL || dev->busnum > (int)i2c_busses_count) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    dev->addr = (int) addr;

    if (strlen(dev->bus_name) > 0) {
        rc = APeripheralManagerClient_openI2cDevice(client,
            dev->bus_name, addr, &dev->bi2c);
    } else {
        rc = APeripheralManagerClient_openI2cDevice(client,
            i2c_busses[dev->busnum], addr, &dev->bi2c);
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static int
mraa_pman_i2c_read_replace(mraa_i2c_context dev, uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return -1;
    }

    rc = AI2cDevice_read(dev->bi2c, data, length);
    if (rc != 0) {
        return rc;
    }

    return length;
}

static int
mraa_pman_i2c_read_byte_replace(mraa_i2c_context dev)
{
    int rc;
    uint8_t val;

    if (dev->bi2c == NULL) {
        return -1;
    }

    rc = AI2cDevice_read(dev->bi2c, &val, 1);
    if (rc != 0 ) {
        return rc;
    }

    return val;
}

static int
mraa_pman_i2c_read_byte_data_replace(mraa_i2c_context dev, uint8_t command)
{
    int rc;
    uint8_t val;

    if (dev->bi2c == NULL) {
        return -1;
    }

    rc = AI2cDevice_readRegByte(dev->bi2c, command, &val);
    if (rc != 0) {
        return rc;
    }

    return val;
}

static int
mraa_pman_i2c_read_bytes_data_replace(mraa_i2c_context dev, uint8_t command, uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return -1;
    }

    //TODO Replace with I2C_RDWR Ioctl from PIO when available since i2c_read_bytes_data expects length on success
    rc = AI2cDevice_readRegBuffer(dev->bi2c, command, data, length);
    if (rc != 0) {
        return rc;
    }

    return length;
}

static int
mraa_pman_i2c_read_word_data_replace(mraa_i2c_context dev, uint8_t command)
{
    int rc;
    uint16_t val;

    if (dev->bi2c == NULL) {
        return -1;
    }

    rc = AI2cDevice_readRegWord(dev->bi2c, command, &val);
    if (rc != 0) {
        return rc;
    }

    return val;
}

static mraa_result_t
mraa_pman_i2c_write_replace(mraa_i2c_context dev, const uint8_t* data, int length)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = AI2cDevice_write(dev->bi2c, data, length);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_i2c_write_byte_replace(mraa_i2c_context dev, const uint8_t data)
{
    return mraa_i2c_write(dev, &data, 1);
}

static mraa_result_t
mraa_pman_i2c_write_byte_data_replace(mraa_i2c_context dev, const uint8_t data, const uint8_t command)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = AI2cDevice_writeRegByte(dev->bi2c, command, data);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_i2c_write_word_data_replace(mraa_i2c_context dev, const uint16_t data, const uint8_t command)
{
    int rc;

    if (dev->bi2c == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = AI2cDevice_writeRegWord(dev->bi2c, command, data);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_init_internal_replace(mraa_gpio_context dev, int pin)
{
    int rc = APeripheralManagerClient_openGpio(client, gpios[pin], &dev->bgpio);
    if (rc != 0) {
        syslog(LOG_ERR, "peripheralmanager: Failed to init gpio");
        return MRAA_ERROR_INVALID_HANDLE;
    }
    dev->pin = pin;
    dev->phy_pin = pin;

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_close_replace(mraa_gpio_context dev)
{
    if (dev->bgpio != NULL) {
        AGpio_delete(dev->bgpio);
    }

    free(dev);
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t dir)
{
    int rc;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    switch (dir) {
        case MRAA_GPIO_IN:
            rc = AGpio_setDirection(dev->bgpio, AGPIO_DIRECTION_IN);
            break;
        case MRAA_GPIO_OUT:
        case MRAA_GPIO_OUT_HIGH:
            rc = AGpio_setDirection(dev->bgpio, AGPIO_DIRECTION_OUT_INITIALLY_HIGH);
            break;
        case MRAA_GPIO_OUT_LOW:
            rc = AGpio_setDirection(dev->bgpio, AGPIO_DIRECTION_OUT_INITIALLY_LOW);
            break;
    }
    if (rc != 0) {
        syslog(LOG_ERR, "peripheralman: Failed to switch direction");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_read_dir_replace(mraa_gpio_context dev, mraa_gpio_dir_t *dir)
{
    syslog(LOG_WARNING, "peripheralman: mraa_gpio_read_dir() dunction not implemented on this backend");
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

static mraa_result_t
mraa_pman_gpio_write_replace(mraa_gpio_context dev, int val)
{
    int rc;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    rc = AGpio_setValue(dev->bgpio, val);
    if (rc != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }

    return MRAA_SUCCESS;
}

static int
mraa_pman_gpio_read_replace(mraa_gpio_context dev)
{
    int rc, val;

    if (dev->bgpio == NULL) {
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
        return -1;
    }

    rc = AGpio_getValue(dev->bgpio, &val);
    if (rc != 0) {
        syslog(LOG_ERR, "peripheralman: Unable to read internal gpio");
        return rc;
    }

    return val;
}

static mraa_result_t
mraa_pman_gpio_edge_mode_replace(mraa_gpio_context dev, mraa_gpio_edge_t mode)
{
    int rc;

    if (dev->bgpio == NULL) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    switch (mode) {
        case MRAA_GPIO_EDGE_BOTH:
            rc = AGpio_setEdgeTriggerType(dev->bgpio, AGPIO_EDGE_BOTH);
            break;
        case MRAA_GPIO_EDGE_FALLING:
            rc = AGpio_setEdgeTriggerType(dev->bgpio, AGPIO_EDGE_FALLING);
            break;
        case MRAA_GPIO_EDGE_RISING:
            rc = AGpio_setEdgeTriggerType(dev->bgpio, AGPIO_EDGE_RISING);
            break;
        case MRAA_GPIO_EDGE_NONE:
            rc = AGpio_setEdgeTriggerType(dev->bgpio, AGPIO_EDGE_NONE);
            break;
    }
    if (rc != 0) {
        return MRAA_ERROR_INVALID_HANDLE;
    }

    return MRAA_SUCCESS;
};

static mraa_result_t
mraa_pman_gpio_wait_interrupt(int fd, int control_fd)
{
    if (fd < 0) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    struct pollfd poller = {
        .fd = fd,
        .events = POLLIN | POLLPRI | POLLERR,
        .revents = 0,
    };

    poll(&poller, 1, -1);
    syslog(LOG_INFO, "peripheralman: received an event");
    AGpio_ackInterruptEvent(fd);

    return MRAA_SUCCESS;
}

static void*
mraa_pman_gpio_interrupt_handler(void* arg)
{
    if (arg == NULL) {
        syslog(LOG_ERR, "peripheralman: interrupt_handler: context is invalid");
        return NULL;
    }

    mraa_gpio_context dev = (mraa_gpio_context) arg;
    int fd = -1;

    if (AGpio_getPollingFd(dev->bgpio, &fd)) {
        syslog(LOG_ERR, "peripheralman: failed to get the fd");
        return NULL;
    }

    if (pipe(dev->isr_control_pipe)) {
        syslog(LOG_ERR, "peripheralman: interrupt_handler: failed to create isr control pipe: %s", strerror(errno));
        close(fd);
        return NULL;
    }

    dev->isr_value_fp = fd;

    if (lang_func->java_attach_thread != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            if (lang_func->java_attach_thread() != MRAA_SUCCESS) {
                close(dev->isr_value_fp);
                dev->isr_value_fp = -1;
                return NULL;
            }
        }
    }

    for (;;) {
        mraa_result_t ret = mraa_pman_gpio_wait_interrupt(dev->isr_value_fp, dev->isr_control_pipe[0]);
	if (ret == MRAA_SUCCESS) {
            dev->isr(dev->isr_args);
        } else {
           if (fd != -1) {
               close(dev->isr_value_fp);
               dev->isr_value_fp = -1;
           }

           if (lang_func->java_detach_thread != NULL && lang_func->java_delete_global_ref != NULL) {
               if (dev->isr == lang_func->java_isr_callback) {
                   lang_func->java_delete_global_ref(dev->isr_args);
                   lang_func->java_detach_thread();
               }
           }

           return NULL;
        }
    }
}

static mraa_result_t
mraa_pman_gpio_isr_replace(mraa_gpio_context dev, mraa_gpio_edge_t edge, void (*fptr)(void*), void* args)
{
    if (dev->bgpio == NULL) {
        syslog(LOG_ERR, "peripheralman: Invalid internal gpio handle");
        return -1;
    }

    mraa_pman_gpio_edge_mode_replace(dev, edge);

    // we only allow one isr per mraa_gpio_context
    if (dev->thread_id != 0) {
        return MRAA_ERROR_NO_RESOURCES;
    }

    dev->isr = fptr;

    /* Most UPM sensors use the C API, the Java global ref must be created here. */
    /* The reason for checking the callback function is internal callbacks. */
    if (lang_func->java_create_global_ref != NULL) {
        if (dev->isr == lang_func->java_isr_callback) {
            args = lang_func->java_create_global_ref(args);
        }
    }

    dev->isr_args = args;
    pthread_create(&dev->thread_id, NULL, mraa_pman_gpio_interrupt_handler, (void*) dev);

    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_pman_gpio_mode_replace(mraa_gpio_context dev, mraa_gpio_mode_t mode)
{
    return MRAA_ERROR_FEATURE_NOT_IMPLEMENTED;
}

mraa_board_t*
mraa_peripheralman_plat_init()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    char* buf1 = (char*) calloc(0, sizeof(UART_PATH_SIZE));
    char buf2[2] = {0};

    if (b == NULL) {
        return NULL;
    }

    if (client != NULL) {
        APeripheralManagerClient_delete(client);
    }

    client = APeripheralManagerClient_new();
    if (client == NULL) {
        return NULL;
    }

    // error checking?
    gpios = APeripheralManagerClient_listGpio(client, &gpios_count);
    i2c_busses = APeripheralManagerClient_listI2cBuses(client, &i2c_busses_count);
    spi_busses = APeripheralManagerClient_listSpiBuses(client, &spi_busses_count);
    uart_devices = APeripheralManagerClient_listUartDevices(client, &uart_dev_count);
    pwm_devices = APeripheralManagerClient_listPwm(client, &pwm_dev_count);

    b->platform_name = "peripheralmanager";
    // query this from peripheral manager?
    b->platform_version = "1.0";

    // disable AIO support
    b->aio_count = 0;
    b->adc_supported = 0;

    b->gpio_count = gpios_count;
    b->phy_pin_count = gpios_count;
    b->i2c_bus_count = i2c_busses_count;
    b->spi_bus_count = spi_busses_count;
    b->uart_dev_count = uart_dev_count;
    b->pwm_dev_count = pwm_dev_count;
    b->pwm_default_period = 5000;
    b->pwm_max_period = 218453;
    b->pwm_min_period = 1;
    b->def_i2c_bus = 0;
    b->def_spi_bus = 0;
    b->def_uart_dev = 0;
    b->def_pwm_dev = 0;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b);
        return NULL;
    }

    int i = 0;

    //Updating GPIO bus structure
    for (; i < gpios_count; i++) {
        b->pins[i].name = gpios[i];
        //Retrieve this information from PIO once the API is available
        b->pins[i].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 1, 1, 1, 1, 1 };
        b->pins[i].gpio.pinmap = i;
    }

    //Updating I2C bus structure
    for (i = 0; i < i2c_busses_count; i++) {
        b->i2c_bus[i].name = i2c_busses[i];
        b->i2c_bus[i].bus_id = i;
        b->i2c_bus[i].sda = -1;
        b->i2c_bus[i].scl = -1;
    }

    //Updating SPI bus structure
    for (i =0; i < spi_busses_count; i++) {
        b->spi_bus[i].name = spi_busses[i];
        b->spi_bus[i].bus_id = i;
        b->spi_bus[i].slave_s = -1;
        b->spi_bus[i].three_wire = -1;
        b->spi_bus[i].sclk = -1;
        b->spi_bus[i].mosi = -1;
        b->spi_bus[i].miso = -1;
        b->spi_bus[i].cs = -1;
    }

    //Updating PWM structure
    for (i = 0; i < pwm_dev_count; i++) {
        b->pwm_dev[i].name = pwm_devices[i];
        b->pwm_dev[i].index = i;
    }

    //Updating UART structure
    for (i = 0; i < uart_dev_count; i++) {
        b->uart_dev[i].name = uart_devices[i];
        b->uart_dev[i].index = i;
        b->uart_dev[i].rx = 0;
        b->uart_dev[i].tx = 0;
        strcpy(buf1, uart_dev_path_prefix);
        sprintf(buf2, "%d", i);
        strcat(buf1, buf2);
        b->uart_dev[i].device_path = buf1;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        free(b);
        return NULL;
    }

    b->adv_func->gpio_init_internal_replace = &mraa_pman_gpio_init_internal_replace;
    b->adv_func->gpio_close_replace = &mraa_pman_gpio_close_replace;
    b->adv_func->gpio_dir_replace = &mraa_pman_gpio_dir_replace;
    b->adv_func->gpio_read_dir_replace = &mraa_pman_gpio_read_dir_replace;
    b->adv_func->gpio_write_replace = &mraa_pman_gpio_write_replace;
    b->adv_func->gpio_read_replace = &mraa_pman_gpio_read_replace;
    b->adv_func->gpio_edge_mode_replace = &mraa_pman_gpio_edge_mode_replace;
    b->adv_func->gpio_mode_replace = &mraa_pman_gpio_mode_replace;
    b->adv_func->gpio_isr_replace = &mraa_pman_gpio_isr_replace;

    b->adv_func->i2c_init_bus_replace = &mraa_pman_i2c_init_bus_replace;
    b->adv_func->i2c_set_frequency_replace = &mraa_pman_i2c_set_frequency_replace;
    b->adv_func->i2c_address_replace = &mraa_pman_i2c_address_replace;
    b->adv_func->i2c_read_replace = &mraa_pman_i2c_read_replace;
    b->adv_func->i2c_read_byte_replace = &mraa_pman_i2c_read_byte_replace;
    b->adv_func->i2c_read_byte_data_replace = &mraa_pman_i2c_read_byte_data_replace;
    b->adv_func->i2c_read_word_data_replace = &mraa_pman_i2c_read_word_data_replace;
    b->adv_func->i2c_read_bytes_data_replace = &mraa_pman_i2c_read_bytes_data_replace;
    b->adv_func->i2c_write_replace = &mraa_pman_i2c_write_replace;
    b->adv_func->i2c_write_byte_replace = &mraa_pman_i2c_write_byte_replace;
    b->adv_func->i2c_write_byte_data_replace = &mraa_pman_i2c_write_byte_data_replace;
    b->adv_func->i2c_write_word_data_replace = &mraa_pman_i2c_write_word_data_replace;
    b->adv_func->i2c_stop_replace = &mraa_pman_i2c_stop_replace;

    b->adv_func->spi_init_raw_replace = &mraa_pman_spi_init_raw_replace;
    b->adv_func->spi_stop_replace = &mraa_pman_spi_stop_replace;
    b->adv_func->spi_bit_per_word_replace = &mraa_pman_spi_bit_per_word_replace;
    b->adv_func->spi_lsbmode_replace = &mraa_pman_spi_lsbmode_replace;
    b->adv_func->spi_mode_replace = &mraa_pman_spi_mode_replace;
    b->adv_func->spi_frequency_replace = &mraa_pman_spi_frequency_replace;
    b->adv_func->spi_write_replace = &mraa_pman_spi_write_replace;
    b->adv_func->spi_write_word_replace = &mraa_pman_spi_write_word_replace;
    b->adv_func->spi_transfer_buf_replace = &mraa_pman_spi_transfer_buf_replace;
    b->adv_func->spi_transfer_buf_word_replace = &mraa_pman_spi_transfer_buf_word_replace;

    b->adv_func->uart_init_raw_replace = &mraa_pman_uart_init_raw_replace;
    b->adv_func->uart_init_post = &mraa_pman_uart_init_post;
    b->adv_func->uart_set_baudrate_replace = &mraa_pman_uart_set_baudrate_replace;
    b->adv_func->uart_flush_replace = &mraa_pman_uart_flush_replace;
    b->adv_func->uart_sendbreak_replace = &mraa_pman_uart_sendbreak_replace;
    b->adv_func->uart_set_flowcontrol_replace = &mraa_pman_uart_set_flowcontrol_replace;
    b->adv_func->uart_set_mode_replace = &mraa_pman_uart_set_mode_replace;
    b->adv_func->uart_set_non_blocking_replace = &mraa_pman_uart_set_non_blocking_replace;
    b->adv_func->uart_set_timeout_replace = &mraa_pman_uart_set_timeout_replace;
    b->adv_func->uart_data_available_replace = &mraa_pman_uart_data_available_replace;
    b->adv_func->uart_write_replace = &mraa_pman_uart_write_replace;
    b->adv_func->uart_read_replace = &mraa_pman_uart_read_replace;

    b->adv_func->pwm_init_raw_replace = &mraa_pman_pwm_init_raw_replace;
    b->adv_func->pwm_period_replace = &mraa_pman_pwm_period_replace;
    b->adv_func->pwm_enable_replace = &mraa_pman_pwm_enable_replace;
    b->adv_func->pwm_read_replace = &mraa_pman_pwm_read_replace;
    b->adv_func->pwm_write_replace = &mraa_pman_pwm_write_replace;
    return b;
}

mraa_platform_t
mraa_peripheralman_platform()
{
    plat = mraa_peripheralman_plat_init();

    return MRAA_ANDROID_PERIPHERALMANAGER;
}

static void
free_resources(char ***resources, int count)
{
    int i;

    if (*resources != NULL) {
        for(i = 0; i < count; i++) {
            free((*resources)[i]);
        }
        free(*resources);
    }

    *resources = NULL;
}

void
pman_mraa_deinit()
{
    free_resources(&pwm_devices, pwm_dev_count);
    free_resources(&uart_devices, uart_dev_count);
    free_resources(&spi_busses, spi_busses_count);
    free_resources(&i2c_busses, i2c_busses_count);
    free_resources(&gpios, gpios_count);

    if (client != NULL) {
        APeripheralManagerClient_delete(client);
        client = NULL;
    }
}
