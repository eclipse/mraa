/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
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

#define PLATFORM_MEDIATEK_LINKIT	1
#define PLATFORM_MEDIATEK_LINKIT_AIR	2
#define MMAP_PATH			"/dev/mem"
#define MT7628_GPIOMODE_BASE		0x10000000
#define MT7628_BLOCK_SIZE		0x1000
#define MT7628_GPIO_CTRL		0x600
#define MT7628_GPIO_DATA		0x620
#define MT7628_GPIO_SET			0x630
#define MT7628_GPIO_CLEAR		0x640

#define MAX_SIZE 64

// MMAP
static uint8_t* mmap_reg = NULL;
static int mmap_fd = 0;
static int mmap_size;
static uint8_t* gpio_mmap_reg = NULL;
static int gpio_mmap_fd = 0;
static int gpio_mmap_size;
static unsigned int mmap_count = 0;
static int platform_detected = 0;

mraa_result_t
mraa_mtk_linkit_mmap_write(mraa_gpio_context dev, int value)
{
    volatile uint32_t* addr;
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
mraa_mtk_linkit_mmap_unsetup()
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

int
mraa_mtk_linkit_mmap_read(mraa_gpio_context dev)
{
    uint32_t value = *(volatile uint32_t*) (mmap_reg + MT7628_GPIO_DATA + (dev->pin / 32) * 4);
    if (value & (uint32_t)(1 << (dev->pin % 32))) {
        return 1;
    }
    return 0;
}

mraa_result_t
mraa_mtk_linkit_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
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
            return mraa_mtk_linkit_mmap_unsetup();
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
    dev->mmap_write = &mraa_mtk_linkit_mmap_write;
    dev->mmap_read = &mraa_mtk_linkit_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}

static int mmap_gpiomode(void)
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

static void set_gpiomode(unsigned int mask, unsigned int shift, unsigned int val)
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

static unsigned char gpio_mux_groups[64];
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

mraa_result_t gpio_init_pre(int pin)
{
	struct pinmux *m = &mt7688_mux[gpio_mux_groups[pin]];

	set_gpiomode(m->mask, m->shift, 1);

	return 0;
}

static void gpiomode_set(unsigned int id, char *name)
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

mraa_result_t i2c_init_pre(unsigned int bus)
{
	gpiomode_set(MUX_I2C, "i2c");
	return 0;
}

mraa_result_t
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

mraa_result_t spi_init_pre(int bus)
{
	gpiomode_set(MUX_SPI_CS1, "spi_cs1");
	return 0;
}

mraa_result_t uart_init_pre(int index)
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

mraa_result_t
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


mraa_board_t*
mraa_mtk_linkit()
{
    int i;

    if (mmap_gpiomode())
	    return NULL;

    mraa_board_t* b = (mraa_board_t*) malloc(sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    memset(b, 0, sizeof(mraa_board_t));

    b->platform_name = "LinkIt Smart 7688";
    platform_detected = PLATFORM_MEDIATEK_LINKIT;
    b->phy_pin_count = 64;

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

    b->adv_func->gpio_mmap_setup = &mraa_mtk_linkit_mmap_setup;

    for (i = 0; i < b->phy_pin_count; i++) {
        snprintf(b->pins[i].name, MRAA_PIN_NAME_SIZE, "GPIO%d", i);
        b->pins[i].capabilites = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };
    }

    strncpy(b->pins[43].name, "GPIO43", MRAA_PIN_NAME_SIZE);
    b->pins[43].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[43].gpio.pinmap = 43;
    gpio_mux_groups[43] = MUX_EPHY;

    strncpy(b->pins[20].name, "GPIO20", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilites = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 1 };
    b->pins[20].gpio.pinmap = 20;
    b->pins[20].uart.parent_id = 2;
    b->pins[20].uart.mux_total = 0;
    b->pins[20].pwm.parent_id = 0;
    b->pins[20].pwm.pinmap = 2;
    gpio_mux_groups[20] = MUX_UART2;

    strncpy(b->pins[21].name, "GPIO21", MRAA_PIN_NAME_SIZE);
    b->pins[21].capabilites = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 1 };
    b->pins[21].gpio.pinmap = 21;
    b->pins[21].uart.parent_id = 2;
    b->pins[21].uart.mux_total = 0;
    b->pins[21].pwm.parent_id = 0;
    b->pins[21].pwm.pinmap = 3;
    gpio_mux_groups[21] = MUX_UART2;

    strncpy(b->pins[2].name, "GPIO2", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[2].gpio.pinmap = 2;
    gpio_mux_groups[2] = MUX_I2S;

    strncpy(b->pins[3].name, "GPIO3", MRAA_PIN_NAME_SIZE);
    b->pins[3].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[3].gpio.pinmap = 3;
    gpio_mux_groups[3] = MUX_I2S;

    strncpy(b->pins[0].name, "GPIO0", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[0].gpio.pinmap = 0;
    gpio_mux_groups[0] = MUX_I2S;

    strncpy(b->pins[1].name, "GPIO1", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[1].gpio.pinmap = 1;
    gpio_mux_groups[1] = MUX_I2S;

    strncpy(b->pins[37].name, "GPIO37", MRAA_PIN_NAME_SIZE);
    b->pins[37].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[37].gpio.pinmap = 37;
    gpio_mux_groups[37] = MUX_GPIO;

    strncpy(b->pins[44].name, "GPIO44", MRAA_PIN_NAME_SIZE);
    b->pins[44].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[44].gpio.pinmap = 44;
    gpio_mux_groups[44] = MUX_WLED;

    strncpy(b->pins[46].name, "GPIO46", MRAA_PIN_NAME_SIZE);
    b->pins[46].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[46].gpio.pinmap = 46;
    b->pins[46].uart.parent_id = 1;
    b->pins[46].uart.mux_total = 0;
    gpio_mux_groups[46] = MUX_UART1;

    strncpy(b->pins[45].name, "GPIO45", MRAA_PIN_NAME_SIZE);
    b->pins[45].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[45].gpio.pinmap = 45;
    b->pins[45].uart.parent_id = 1;
    b->pins[45].uart.mux_total = 0;
    gpio_mux_groups[45] = MUX_UART1;

    strncpy(b->pins[13].name, "GPIO13", MRAA_PIN_NAME_SIZE);
    b->pins[13].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[13].gpio.pinmap = 13;
    b->pins[13].uart.parent_id = 1;
    b->pins[13].uart.mux_total = 0;
    gpio_mux_groups[13] = MUX_UART0;

    strncpy(b->pins[12].name, "GPIO12", MRAA_PIN_NAME_SIZE);
    b->pins[12].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[12].gpio.pinmap = 12;
    b->pins[12].uart.parent_id = 0;
    b->pins[12].uart.mux_total = 0;
    gpio_mux_groups[12] = MUX_UART0;

    strncpy(b->pins[5].name, "GPIO5", MRAA_PIN_NAME_SIZE);
    b->pins[5].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    b->pins[5].gpio.pinmap = 5;
    b->pins[5].i2c.pinmap = 0;
    b->pins[5].i2c.mux_total = 0;
    gpio_mux_groups[5] = MUX_I2C;

    strncpy(b->pins[4].name, "GPIO4", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    b->pins[4].gpio.pinmap = 4;
    b->pins[4].i2c.pinmap = 0;
    b->pins[4].i2c.mux_total = 0;
    gpio_mux_groups[4] = MUX_I2C;

    strncpy(b->pins[6].name, "GPIO6", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[6].gpio.pinmap = 6;
    b->pins[6].spi.pinmap = 0;
    b->pins[6].spi.mux_total = 0;
    gpio_mux_groups[6] = MUX_SPI_CS1;

    strncpy(b->pins[7].name, "GPIO7", MRAA_PIN_NAME_SIZE);
    b->pins[7].capabilites = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[7].spi.pinmap = 0;
    b->pins[7].spi.mux_total = 0;

    strncpy(b->pins[8].name, "GPIO8", MRAA_PIN_NAME_SIZE);
    b->pins[8].capabilites = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[8].spi.pinmap = 0;
    b->pins[8].spi.mux_total = 0;

    strncpy(b->pins[9].name, "GPIO9", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilites = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[9].spi.pinmap = 0;
    b->pins[9].spi.mux_total = 0;

    strncpy(b->pins[18].name, "GPIO18", MRAA_PIN_NAME_SIZE);
    b->pins[18].capabilites = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[18].gpio.pinmap = 18;
    b->pins[18].pwm.parent_id = 0;
    b->pins[18].pwm.pinmap = 0;
    gpio_mux_groups[18] = MUX_PWM0;

    strncpy(b->pins[19].name, "GPIO19", MRAA_PIN_NAME_SIZE);
    b->pins[19].capabilites = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[19].gpio.pinmap = 19;
    b->pins[19].pwm.parent_id = 0;
    b->pins[19].pwm.pinmap = 1;
    gpio_mux_groups[19] = MUX_PWM1;

    strncpy(b->pins[16].name, "GPIO16", MRAA_PIN_NAME_SIZE);
    b->pins[16].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[16].gpio.pinmap = 16;
    gpio_mux_groups[16] = MUX_SPI_S;

    strncpy(b->pins[17].name, "GPIO17", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[17].gpio.pinmap = 17;
    gpio_mux_groups[17] = MUX_SPI_S;

    strncpy(b->pins[14].name, "GPIO14", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[14].gpio.pinmap = 14;
    gpio_mux_groups[14] = MUX_SPI_S;

    strncpy(b->pins[15].name, "GPIO15", MRAA_PIN_NAME_SIZE);
    b->pins[15].capabilites = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[15].gpio.pinmap = 15;
    gpio_mux_groups[15] = MUX_SPI_S;

    // BUS DEFINITIONS
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 5;
    b->i2c_bus[0].scl = 4;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 32766;
    b->spi_bus[0].slave_s = 1;
    b->spi_bus[0].cs = 6;
    b->spi_bus[0].mosi = 8;
    b->spi_bus[0].miso = 9;
    b->spi_bus[0].sclk = 7;

    b->uart_dev_count = 3;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 13;
    b->uart_dev[0].tx = 12;
    b->uart_dev[0].device_path = "/dev/ttyS0";
    b->uart_dev[1].rx = 46;
    b->uart_dev[1].tx = 45;
    b->uart_dev[1].device_path = "/dev/ttyS1";
    b->uart_dev[2].rx = 21;
    b->uart_dev[2].tx = 20;
    b->uart_dev[2].device_path = "/dev/ttyS2";

    b->gpio_count = 0;
    for (i = 0; i < b->phy_pin_count; i++) {
        if (b->pins[i].capabilites.gpio) {
            b->gpio_count++;
        }
    }

    return b;
}
