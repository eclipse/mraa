/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Author: Serge Vakulenko <vak@besm6.org>
 * Copyright (c) 2014 Intel Corporation.
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
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <mraa/common.h>

#include "mraa_internal.h"

#include "common.h"

#define MMAP_PATH               "/dev/mem"
#define MT7628_GPIOMODE_BASE    0x10000000
#define MT7628_BLOCK_SIZE       0x1000
#define MT7628_GPIO_CTRL        0x600
#define MT7628_GPIO_DATA        0x620
#define MT7628_GPIO_SET         0x630
#define MT7628_GPIO_CLEAR       0x640

#define MAX_SIZE                64

// MMAP
static uint8_t *mmap_reg = NULL;
static int mmap_fd = 0;
static int mmap_size;
static uint8_t *gpio_mmap_reg = NULL;
static int gpio_mmap_fd = 0;
static unsigned int mmap_count = 0;

static mraa_result_t
mtk_mmap_write(mraa_gpio_context dev, int value)
{
    if (value) {
        *(volatile uint32_t*) (mmap_reg + MT7628_GPIO_SET + (dev->pin / 32) * 4) =
            (uint32_t)(1 << (dev->pin % 32));
    } else {
        *(volatile uint32_t*) (mmap_reg + MT7628_GPIO_CLEAR + (dev->pin / 32) * 4) =
            (uint32_t)(1 << (dev->pin % 32));
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mtk_mmap_unsetup()
{
    if (mmap_reg == NULL) {
        syslog(LOG_ERR, "linkit mmap: null register cant unsetup");
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    munmap(mmap_reg, mmap_size);
    mmap_reg = NULL;
    if (close(mmap_fd) != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    return MRAA_SUCCESS;
}

static int
mtk_mmap_read(mraa_gpio_context dev)
{
    uint32_t value = *(volatile uint32_t*) (mmap_reg + MT7628_GPIO_DATA + (dev->pin / 32) * 4);
    if (value & (uint32_t)(1 << (dev->pin % 32))) {
        return 1;
    }
    return 0;
}

static mraa_result_t
mtk_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "linkit mmap: context not valid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (en == 0) {
        if (dev->mmap_write == NULL && dev->mmap_read == NULL) {
            syslog(LOG_ERR, "linkit mmap: can't disable disabled mmap gpio");
            return MRAA_ERROR_INVALID_PARAMETER;
        }
        dev->mmap_write = NULL;
        dev->mmap_read = NULL;
        mmap_count--;
        if (mmap_count == 0) {
            return mtk_mmap_unsetup();
        }
        return MRAA_SUCCESS;
    }

    if (dev->mmap_write != NULL && dev->mmap_read != NULL) {
        syslog(LOG_ERR, "linkit mmap: can't enable enabled mmap gpio");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    // Might need to make some elements of this thread safe.
    // For example only allow one thread to enter the following block
    // to prevent mmap'ing twice.
    if (mmap_reg == NULL) {
        if ((mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
            syslog(LOG_ERR, "linkit map: unable to open resource0 file");
            return MRAA_ERROR_INVALID_HANDLE;
        }

        mmap_reg = (uint8_t*) mmap(NULL, MT7628_BLOCK_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_FILE | MAP_SHARED, mmap_fd, 0x10000000);
        if (mmap_reg == MAP_FAILED) {
            perror("foo");
            syslog(LOG_ERR, "linkit mmap: failed to mmap");
            mmap_reg = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    dev->mmap_write = &mtk_mmap_write;
    dev->mmap_read = &mtk_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}

static int
mmap_gpiomode(void)
{
    if ((gpio_mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
        syslog(LOG_ERR, "linkit map: unable to open resource0 file");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    gpio_mmap_reg = (uint8_t*) mmap(NULL, MT7628_BLOCK_SIZE, PROT_READ | PROT_WRITE,
                                   MAP_FILE | MAP_SHARED, gpio_mmap_fd, MT7628_GPIOMODE_BASE);
    if (gpio_mmap_reg == MAP_FAILED) {
        syslog(LOG_ERR, "linkit gpio_mmap: failed to mmap");
        gpio_mmap_reg = NULL;
        close(gpio_mmap_fd);
        return MRAA_ERROR_NO_RESOURCES;
    }
    return 0;
}

static void
set_gpiomode(unsigned int mask, unsigned int shift, unsigned int val)
{
    unsigned int reg;
    unsigned int offset = 0x60;

    if (shift >= 32) {
        shift -= 32;
        offset += 4;
    }

    reg = *(volatile uint32_t*) (gpio_mmap_reg + offset);

    reg &= ~(mask << shift);
    reg |= (val << shift);
    *(volatile uint32_t*) (gpio_mmap_reg + offset) = reg;
}

enum {
    MUX_GPIO = 0,
    MUX_SPI_S,
    MUX_SPI_CS1,
    MUX_I2S,
    MUX_UART0,
    MUX_I2C,
    MUX_UART1,
    MUX_UART2,
    MUX_PWM0,
    MUX_PWM1,
    MUX_EPHY,
    MUX_WLED,
    __MUX_MAX,
};

static unsigned char gpio_mux_groups[MAX_SIZE];

static struct pinmux {
    char *name;
    char *func[4];
    unsigned int shift;
    unsigned int mask;
} mt7688_mux[] = {
    {
        .name = "refclk",
        .func = { "refclk", "gpio", NULL, NULL },
        .shift = 18,
        .mask = 0x1,
    }, {
        .name = "spi_s",
        .func = { "spi_s", "gpio", "utif", "pwm" },
        .shift = 2,
        .mask = 0x3,
    }, {
        .name = "spi_cs1",
        .func = { "spi_cs1", "gpio", NULL, "refclk" },
        .shift = 4,
        .mask = 0x3,
    }, {
        .name = "i2s",
        .func = { "i2s", "gpio", "pcm", NULL },
        .shift = 6,
        .mask = 0x3,
    }, {
        .name = "uart0",
        .func = { "uart", "gpio", NULL, NULL },
        .shift = 8,
        .mask = 0x3,
    }, {
        .name = "i2c",
        .func = { "i2c", "gpio", NULL, NULL },
        .shift = 20,
        .mask = 0x3,
    }, {
        .name = "uart1",
        .func = { "uart", "gpio", NULL, NULL },
        .shift = 24,
        .mask = 0x3,
    }, {
        .name = "uart2",
        .func = { "uart", "gpio", "pwm", NULL },
        .shift = 26,
        .mask = 0x3,
    }, {
        .name = "pwm0",
        .func = { "pwm", "gpio", NULL, NULL },
        .shift = 28,
        .mask = 0x3,
    }, {
        .name = "pwm1",
        .func = { "pwm", "gpio", NULL, NULL },
        .shift = 30,
        .mask = 0x3,
    }, {
        .name = "ephy",
        .func = { "ephy", "gpio", NULL, NULL },
        .shift = 34,
        .mask = 0x3,
    }, {
        .name = "wled",
        .func = { "wled", "gpio", NULL, NULL },
        .shift = 32,
        .mask = 0x3,
    },
};

static mraa_result_t
gpio_init_pre(int pin)
{
    struct pinmux *m = &mt7688_mux[gpio_mux_groups[pin]];

    set_gpiomode(m->mask, m->shift, 1);

    return 0;
}

static void
gpiomode_set(unsigned int id, char *name)
{
    int i;

    if (id >= __MUX_MAX)
        return;

    for (i = 0; i < 4; i++) {
        if (!mt7688_mux[id].func[i] || strcmp(mt7688_mux[id].func[i], name))
            continue;
        set_gpiomode(mt7688_mux[id].mask, mt7688_mux[id].shift, i);
        syslog(0, "mraa: set pinmux %s -> %s\n", mt7688_mux[id].name, name);
        return;
    }
}

static mraa_result_t
i2c_init_pre(unsigned int bus)
{
    gpiomode_set(MUX_I2C, "i2c");
    return 0;
}

static mraa_result_t
pwm_init_post(mraa_pwm_context pwm)
{
    switch(pwm->pin) {
    case 0:
        gpiomode_set(MUX_PWM0, "pwm");
        break;
    case 1:
        gpiomode_set(MUX_PWM1, "pwm");
        break;
    case 2:
    case 3:
        gpiomode_set(MUX_UART2, "pwm");
        break;
    }
    return 0;
}

static mraa_result_t
spi_init_pre(int bus)
{
    gpiomode_set(MUX_SPI_CS1, "spi_cs1");
    return 0;
}

static mraa_result_t
uart_init_pre(int index)
{
    switch(index) {
    case 0:
        gpiomode_set(MUX_UART0, "uart");
        break;
    case 1:
        gpiomode_set(MUX_UART1, "uart");
        break;
    case 2:
        gpiomode_set(MUX_UART2, "uart");
        break;
    }
    return 0;
}

static mraa_result_t
i2c_freq(mraa_i2c_context dev, mraa_i2c_mode_t mode)
{
    switch (mode) {
    case MRAA_I2C_STD:
        break;
    default:
        syslog(LOG_ERR, "Invalid i2c frequency");
        break;
    }
    return MRAA_SUCCESS;
}

/*
 * Add a pin descriptor.
 */
static void
add_gpio_pin(mraa_board_t *b, int index, const char *name, int mux,
    mraa_boolean_t valid, mraa_boolean_t gpio, mraa_boolean_t pwm,
    mraa_boolean_t fast_gpio, mraa_boolean_t spi, mraa_boolean_t i2c,
    mraa_boolean_t aio, mraa_boolean_t uart)
{
    strncpy(b->pins[index].name, name, MRAA_PIN_NAME_SIZE);
    b->pins[index].gpio.pinmap = index;
    gpio_mux_groups[index] = mux;
    b->pins[index].capabilities.valid = valid;
    b->pins[index].capabilities.gpio = gpio;
    b->pins[index].capabilities.pwm = pwm;
    b->pins[index].capabilities.fast_gpio = fast_gpio;
    b->pins[index].capabilities.spi = spi;
    b->pins[index].capabilities.i2c = i2c;
    b->pins[index].capabilities.aio = aio;
    b->pins[index].capabilities.uart = uart;
    if (gpio) {
        b->gpio_count++;
    }
}

/*
 * Add UART device.
 */
static void
add_uart_dev(mraa_board_t *b, char *path, int tx_pin, int rx_pin)
{
    b->uart_dev[b->uart_dev_count].device_path = path;
    b->uart_dev[b->uart_dev_count].tx = tx_pin;
    b->uart_dev[b->uart_dev_count].rx = rx_pin;
    ++b->uart_dev_count;
}

/*
 * Allocate an instance for generic MT7688 board.
 */
static mraa_board_t *
mtk_common(char *name)
{
    int i;

    if (mmap_gpiomode())
        return NULL;

    mraa_board_t *b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    memset(b, 0, sizeof(mraa_board_t));

    b->platform_name = name;
    b->phy_pin_count = MAX_SIZE;

    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->pwm_default_period = 500;
    b->pwm_max_period = 1000000;
    b->pwm_min_period = 1;

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        return NULL;
    }

    b->adv_func->i2c_init_pre = i2c_init_pre;
    b->adv_func->pwm_init_post = pwm_init_post;
    b->adv_func->spi_init_pre = spi_init_pre;
    b->adv_func->uart_init_pre = uart_init_pre;
    b->adv_func->gpio_init_pre = gpio_init_pre;
    b->adv_func->i2c_set_frequency_replace = &i2c_freq;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);

    memset(b->pins, 0, sizeof(mraa_pininfo_t) * b->phy_pin_count);
    memset(gpio_mux_groups, -1, sizeof(gpio_mux_groups));

    b->adv_func->gpio_mmap_setup = &mtk_mmap_setup;

    for (i = 0; i < b->phy_pin_count; i++) {
        snprintf(b->pins[i].name, MRAA_PIN_NAME_SIZE, "GPIO%d", i);
        b->pins[i].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };
    }

    //
    // I2C bus
    //
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 5;
    b->i2c_bus[0].scl = 4;

    //
    // SPI bus
    //
    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;
    b->spi_bus[0].slave_s = 1;
    b->spi_bus[0].cs = 6;
    b->spi_bus[0].mosi = 8;
    b->spi_bus[0].miso = 9;
    b->spi_bus[0].sclk = 7;

    return b;
}

/*
 * Mediatek Linkit Smart 7688 board (and Duo)
 */
mraa_board_t *
mraa_mtk_linkit()
{
    mraa_board_t *b = mtk_common("LinkIt Smart 7688");
    if (b == NULL) {
        return NULL;
    }

    //
    // GPIO pins
    //                                                gpio     spi i2c  uart
    add_gpio_pin(b, 43, "GPIO43",   MUX_EPHY,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 20, "GPIO20",   MUX_UART2,      1, 1, 1, 0, 0, 0, 0, 1);
    b->pins[20].uart.parent_id = 2;
    b->pins[20].pwm.parent_id = 0;
    b->pins[20].pwm.pinmap = 2;
    add_gpio_pin(b, 21, "GPIO21",   MUX_UART2,      1, 1, 1, 0, 0, 0, 0, 1);
    b->pins[21].uart.parent_id = 2;
    b->pins[21].pwm.parent_id = 0;
    b->pins[21].pwm.pinmap = 3;
    add_gpio_pin(b, 2,  "GPIO2",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 3,  "GPIO3",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 0,  "GPIO0",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 1,  "GPIO1",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 37, "GPIO37",   MUX_GPIO,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 44, "GPIO44",   MUX_WLED,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 46, "GPIO46",   MUX_UART1,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[46].uart.parent_id = 1;
    add_gpio_pin(b, 45, "GPIO45",   MUX_UART1,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[45].uart.parent_id = 1;
    add_gpio_pin(b, 13, "GPIO13",   MUX_UART0,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[13].uart.parent_id = 0;
    add_gpio_pin(b, 12, "GPIO12",   MUX_UART0,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[12].uart.parent_id = 0;
    add_gpio_pin(b, 5,  "GPIO5",    MUX_I2C,        1, 1, 0, 0, 0, 1, 0, 0);
    b->pins[5].i2c.pinmap = 0;
    add_gpio_pin(b, 4,  "GPIO4",    MUX_I2C,        1, 1, 0, 0, 0, 1, 0, 0);
    b->pins[4].i2c.pinmap = 0;
    add_gpio_pin(b, 6,  "GPIO6",    MUX_SPI_CS1,    1, 1, 0, 0, 1, 0, 0, 0);
    b->pins[6].spi.pinmap = 0;
    add_gpio_pin(b, 7,  "SPI_CLK",  MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 8,  "SPI_MOSI", MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 9,  "SPI_MISO", MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 18, "GPIO18",   MUX_PWM0,       1, 1, 1, 0, 0, 0, 0, 0);
    b->pins[18].pwm.parent_id = 0;
    b->pins[18].pwm.pinmap = 0;
    add_gpio_pin(b, 19, "GPIO19",   MUX_PWM1,       1, 1, 1, 0, 0, 0, 0, 0);
    b->pins[19].pwm.parent_id = 0;
    b->pins[19].pwm.pinmap = 1;
    add_gpio_pin(b, 16, "GPIO16",   MUX_SPI_S,      1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 17, "GPIO17",   MUX_SPI_S,      1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 14, "GPIO14",   MUX_SPI_S,      1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 15, "GPIO15",   MUX_SPI_S,      1, 1, 0, 0, 0, 0, 0, 0);

    //
    // UARTs
    //
    add_uart_dev(b, "/dev/ttyS0", 12, 13);
    add_uart_dev(b, "/dev/ttyS1", 45, 46);
    add_uart_dev(b, "/dev/ttyS2", 20, 21);
    b->def_uart_dev = 0;
    return b;
}

/*
 * Onion Omega2 and Omega2+ boards
 */
mraa_board_t *
mraa_mtk_omega2()
{
    mraa_board_t *b = mtk_common("Onion Omega2");
    if (b == NULL) {
        return NULL;
    }

    //
    // GPIO pins, left side
    //                                                gpio     spi i2c  uart
    add_gpio_pin(b, 11, "GPIO11",   MUX_GPIO,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 3,  "GPIO3",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 2,  "GPIO2",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 17, "GPIO17",   MUX_GPIO,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 16, "GPIO16",   MUX_GPIO,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 15, "GPIO15",   MUX_GPIO,       1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 46, "GPIO46",   MUX_UART1,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[46].uart.parent_id = 1;
    add_gpio_pin(b, 45, "GPIO45",   MUX_UART1,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[45].uart.parent_id = 1;
    add_gpio_pin(b, 9,  "SPI_MISO", MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 8,  "SPI_MOSI", MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 7,  "SPI_CLK",  MUX_GPIO,       1, 0, 0, 0, 1, 0, 0, 0);
    add_gpio_pin(b, 6,  "GPIO6",    MUX_SPI_CS1,    1, 1, 0, 0, 1, 0, 0, 0);
    b->pins[6].spi.pinmap = 0;
    add_gpio_pin(b, 1,  "GPIO1",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 0,  "GPIO0",    MUX_I2S,        1, 1, 0, 0, 0, 0, 0, 0);

    //
    // GPIO pins, right side
    //                                                gpio     spi i2c  uart
    add_gpio_pin(b, 13, "GPIO13",   MUX_UART0,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[13].uart.parent_id = 0;
    add_gpio_pin(b, 12, "GPIO12",   MUX_UART0,      1, 1, 0, 0, 0, 0, 0, 1);
    b->pins[12].uart.parent_id = 0;
    add_gpio_pin(b, 38, "FW_RST", 0,                1, 1, 0, 0, 0, 0, 0, 0);
    add_gpio_pin(b, 18, "GPIO18",   MUX_PWM0,       1, 1, 1, 0, 0, 0, 0, 0);
    b->pins[18].pwm.parent_id = 0;
    b->pins[18].pwm.pinmap = 0;
    add_gpio_pin(b, 19, "GPIO19",   MUX_PWM1,       1, 1, 1, 0, 0, 0, 0, 0);
    b->pins[19].pwm.parent_id = 0;
    b->pins[19].pwm.pinmap = 1;
    add_gpio_pin(b, 5,  "GPIO5",    MUX_I2C,        1, 1, 0, 0, 0, 1, 0, 0);
    b->pins[5].i2c.pinmap = 0;
    add_gpio_pin(b, 4,  "GPIO4",    MUX_I2C,        1, 1, 0, 0, 0, 1, 0, 0);
    b->pins[4].i2c.pinmap = 0;

    //
    // UARTs
    //
    add_uart_dev(b, "/dev/ttyS0", 12, 13);
    add_uart_dev(b, "/dev/ttyS1", 45, 46);
    b->def_uart_dev = 0;

    return b;
}
