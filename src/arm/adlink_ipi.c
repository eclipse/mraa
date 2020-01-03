/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Santhanakumar A <santhanakumar.a@adlinktech.com>
 * Copyright (c) 2014 Intel Corporation.
 * Copyright (c) 2019 ADLINK Technology Inc.
 * SPDX-License-Identifier: MIT

*/


#include <dirent.h>
#include <mraa/common.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>

#include "arm/adlink_ipi.h"
#include "common.h"

#define PLATFORM_NAME_ADLINK_IPI "Adlink IPI - PX30"
#define MRAA_ADLINK_IPI_PINCOUNT 41

static int platform_detected = 0;

static const char* serialdev[] = { "/dev/ttyS0", "/dev/ttyS1" };
static const char* seriallink[] = { "/sys/class/tty/ttyS0", "/sys/class/tty/ttyS1" };

static const char* spilink[] = { "/sys/class/spidev/spidev0.0",
                          "/sys/class/spidev/spidev1.0" };

static const char* i2clink[] = {
    "/sys/class/i2c-dev/i2c-0", "/sys/class/i2c-dev/i2c-1" };

static const char* pwmlink[] = {
    "/sys/class/pwm/pwmchip1", "/sys/class/pwm/pwmchip2" };

static unsigned char regIon[16]   = {0x2A, 0x2D, 0x30, 0x33, 0x36, 0x3B, 0x40, 0x45, 0x4A, 0x4D, 0x50, 0x53, 0x56, 0x5B, 0x60, 0x65};

static unsigned int  IonValue[16]   = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};

static int base2, _fd;

#define MAX_SIZE 64
#define SYSFS_PWM "/sys/class/pwm"

static int sx150x_pwm_init(int);

static int
mraa_pwm_setup_duty_fp(mraa_pwm_context dev)
{
    char bu[MAX_SIZE];
    snprintf(bu, MAX_SIZE, "/sys/class/pwm/pwmchip%d/pwm%d/duty_cycle", dev->chipid, dev->pin);

    dev->duty_fp = open(bu, O_RDWR);
    if (dev->duty_fp == -1) {
        return 1;
    }
    return 0;
}


int _tperiod;

static mraa_result_t pwm_period_replace(mraa_pwm_context dev, int period)
{
	_tperiod = period;
	return MRAA_SUCCESS;
}

static float pwm_read_replace(mraa_pwm_context dev)
{
        unsigned char rx_tx_buf[3] = {0};

        if(dev->pin < 9)
        {
                if(_fd == -1)
                {
                        return 0;
                }

                rx_tx_buf[0] = regIon[dev->pin];
                if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
                {
                        if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
                        {
                                return (rx_tx_buf[1] / 2.55);
                        }
                }
        }
        return 0;
}

static mraa_result_t pwm_write_replace(mraa_pwm_context dev, float duty)
{
        unsigned char rx_tx_buf[3] = {0};

	IonValue[dev->pin] = ((duty /_tperiod) * 255);
	
        if(dev->pin < 9)
        {
                if(_fd == -1)
                {
                        return MRAA_ERROR_INVALID_RESOURCE;
                }

                rx_tx_buf[0] = regIon[dev->pin];
                rx_tx_buf[1] = IonValue[dev->pin];

                if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
                {
                        return MRAA_ERROR_NO_RESOURCES;
                }
                return MRAA_SUCCESS;
        }
        return MRAA_ERROR_NO_RESOURCES;
}


static mraa_result_t pwm_enable_replace(mraa_pwm_context dev, int enable)
{
        int pin = dev->pin;

        if(9 > pin && 0 <= pin)
        {
                if(_fd != -1)
                {
                        unsigned char rx_tx_buf[3] = {0};

                        if(_fd == -1)
                        {
                                return MRAA_ERROR_NO_RESOURCES;
                        }

                        rx_tx_buf[0] = regIon[pin];
                        rx_tx_buf[1] = IonValue[pin];
                        if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
                        {
                                return MRAA_ERROR_NO_RESOURCES;
                        }
                }
                else
                {
                        return MRAA_ERROR_NO_RESOURCES;
                }

                return MRAA_SUCCESS;
        }
        return MRAA_ERROR_NO_RESOURCES;
}

static mraa_result_t pwm_init_raw_replace(mraa_pwm_context dev, int pin)
{
	char buffer[100] = {0};
	int i, fd;

	if(dev->chipid == 3)
	{
		if(pin < 9)
		{
			if(sx150x_pwm_init(pin))
			{
				return MRAA_ERROR_NO_RESOURCES;
			}
			if((fd = open("/sys/class/gpio/unexport", O_WRONLY)) != -1)
			{
				i = sprintf(buffer,"%d",base2 + pin);
				write(fd, buffer, i);
				close(fd);
			}
			if((fd = open("/sys/class/gpio/export", O_WRONLY)) != -1)
			{
				i = sprintf(buffer,"%d",base2 + pin);
				write(fd, buffer, i);
				close(fd);
				sprintf(buffer,"/sys/class/gpio/gpio%d/direction",base2 + pin);
				if((fd = open(buffer, O_WRONLY)) != -1)
				{
					write(fd, "out", 3);
					close(fd);
					sprintf(buffer,"/sys/class/gpio/gpio%d/value",base2 + pin);
					if((fd = open(buffer, O_WRONLY)) != -1)
					{
						write(fd, "0", 1);
						close(fd);
					}
				}
			}
			else
			{
				return MRAA_ERROR_NO_RESOURCES;
			}
			return MRAA_SUCCESS;
		}
		else
		{
			return MRAA_ERROR_NO_RESOURCES;
		}
	}
	return MRAA_ERROR_NO_RESOURCES;
}

//configuring extra registers as pwm for extended gpio
static int sx150x_pwm_init(int pin)
{
        int i;
        int add = (pin < 8) ? 1 : 0;

        unsigned char rx_tx_buf[] = {0x0 + add, 0, 0x6 + add, 0, 0xE + add, 0, 0x20 + add, 0, 0x10 + add, 0};

        if(_fd == -1)
        {
                return -1;
        }

        for(i = 0; i < 9; i += 2)
        {
                if(write(_fd, &(rx_tx_buf[i]), 1) != 1)
                {
                        return -1;
                }
                if(read(_fd, &(rx_tx_buf[i + 1]), 1) != 1)
                {
                        return -1;
                }

                if((i == 2) || (i == 6))
                        rx_tx_buf[i+1] |= (1 << (pin % 8));
                else
                        rx_tx_buf[i+1]  &= ~(1 << (pin % 8));

                if(write(_fd, &(rx_tx_buf[i]), 2) != 2)
                {
                        return -1;
                }
        }

        return 0;
}

static mraa_result_t gpio_init_pre(int pin)
{
        unsigned char rx_tx_buf[3] = {0};
        int fd, i;
        char buffer[50] = {0};

        if((496 + 16) > pin && (496 - 1) < pin)
        {
                pin = pin - base2;

                int add = (pin < 8) ? 1 : 0;

                if(_fd == -1)
                {
                        return MRAA_ERROR_INVALID_RESOURCE;
                }

                rx_tx_buf[0] = regIon[pin];
                rx_tx_buf[1] = -1;

                if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
                {
                        return MRAA_ERROR_NO_RESOURCES;
                }

                rx_tx_buf[0] = 0x0 + add;
                if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
                {
                        if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
                        {
                                rx_tx_buf[1] &= ~(1 < (pin % 8));
                                write(_fd, &rx_tx_buf[0], 2);
                        }
                }

                rx_tx_buf[0] = 0x20 + add;
                if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
                {
                        if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
                        {
                                rx_tx_buf[1] &= ~(1 < (pin % 8));
                                write(_fd, &rx_tx_buf[0], 2);
                                if((fd = open("/sys/class/gpio/unexport", O_WRONLY)) != -1)
                                {
                                        i = sprintf(buffer,"%d",base2 + pin);
                                        write(fd, buffer, i);
                                        close(fd);
                                }
                                return MRAA_SUCCESS;
                        }
                }
                return MRAA_ERROR_INVALID_RESOURCE;
        }
        return MRAA_SUCCESS;
}


static int sx150x_init()
{
        char rx_tx_buf[100] = {0};
	int i, bus_num, fd;

        for(i = 0; i < 999; i++)
	{
		sprintf(rx_tx_buf,"/sys/class/gpio/gpiochip%d/device/name",i);
		if((fd = open(rx_tx_buf, O_RDONLY)) != -1)
		{
			int count = read(fd, rx_tx_buf, 7);
			if(count != 0)
			{
				if(strncmp(rx_tx_buf, "sx1509q", count) == 0)
				{
					base2 = i;
					break;
				}
			}
			close(fd);
		}
	}

	for(i = 0; i < 999;i++)
	{
		sprintf(rx_tx_buf,"/sys/bus/i2c/devices/%x-003e/name",i);
		if((fd = open(rx_tx_buf, O_RDONLY)) != -1)
		{
			int count = read(fd, rx_tx_buf, 7);
			if(count != 0)
			{
				if(strncmp(rx_tx_buf, "sx1509q", count) == 0)
				{
					bus_num = i;
					break;
				}
			}
			close(fd);
		}
	}

        sprintf(rx_tx_buf, "/dev/i2c-%d",bus_num);
        if((_fd = open(rx_tx_buf, O_RDWR)) < 0)
        {
                return -1;
        }

        if(ioctl(_fd, I2C_SLAVE_FORCE, 0x3E) < 0)
        {
                return -1;
        }
// configuring clock and misc register for PWM feature
        rx_tx_buf[0] = 0x1E;
        if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
        {
                if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
                {
                        rx_tx_buf[1] |= (1 << 6);
                        rx_tx_buf[1] |= ~(1 << 5);
                        if(write(_fd, &(rx_tx_buf[0]), 2) != 2)
                        {
                                return -1;
                        }
                }
        }
        else
        {
                return -1;
        }

        rx_tx_buf[0] = 0x1F;
        if(write(_fd, &(rx_tx_buf[0]), 1) == 1)
        {
                if(read(_fd, &(rx_tx_buf[1]), 1) == 1)
                {
                        rx_tx_buf[1] &= ~(1 << 7);
                        rx_tx_buf[1] &= ~(1 << 3);
                        rx_tx_buf[1] &= ~((0x7) << 4);
                        rx_tx_buf[1] |= ((1 & 0x7) << 4);
                        if(write(_fd, &(rx_tx_buf[0]), 2) == 2)
                        {
                                return 0;
                        }
                }
        }

        return -1;
}

mraa_board_t*
mraa_adlink_ipi()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    platform_detected = 0;
    int i2c0 = -1;
    int i2c1 = -1;
    int spi0 = -1;
    int uart0 = -1;
    int pwm0 = -1;
    int pwm1 = -1;

    //TODO: Handle different Adlink arm variants eg. IMX6, IMX8M
    b->platform_name = PLATFORM_NAME_ADLINK_IPI;
    //platform_detected = PLATFORM_PRO;
    b->phy_pin_count = MRAA_ADLINK_IPI_PINCOUNT;
/*
    if (platform_detected == 0) {
        free(b);
        syslog(LOG_ERR, "mraa: Could not detect platform");
        return NULL;
    }
*/
    int devnum;
    for (devnum = 0; devnum < 2; devnum++) {
        if (mraa_link_targets(seriallink[devnum], "ff030000")) {
            uart0 = devnum;
        }
    }

    for (devnum = 0; devnum < 2; devnum++) {
        if (mraa_link_targets(spilink[devnum], "ff1d0000")) {
            spi0 = devnum;
        }
    }

    for (devnum = 0; devnum < 2; devnum++) {
        if (mraa_link_targets(i2clink[devnum], "ff180000")) {
            i2c0 = devnum;
        }
        if (mraa_link_targets(i2clink[devnum], "ff190000")) {
            i2c1 = devnum;
        }
    }

    for (devnum = 0; devnum < 2; devnum++) {
        if (mraa_link_targets(pwmlink[devnum], "ff208020")) {
            pwm0 = devnum;
        }
        if (mraa_link_targets(pwmlink[devnum], "ff208030")) {
            pwm1 = devnum;
        }
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pwm_dev_count = 0;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->adv_func->gpio_init_pre = gpio_init_pre;

    // initializations of pwm functions
    b->adv_func->pwm_init_raw_replace = pwm_init_raw_replace;
    b->adv_func->pwm_period_replace = pwm_period_replace;
    b->adv_func->pwm_read_replace = pwm_read_replace;
    b->adv_func->pwm_write_replace = pwm_write_replace;
    b->adv_func->pwm_enable_replace = pwm_enable_replace;


    /*
    b->adv_func->spi_init_pre = &mraa_adlink_spi_init_pre;
    b->adv_func->i2c_init_pre = &mraa_adlink_i2c_init_pre;
    b->adv_func->gpio_mmap_setup = &mraa_adlink_mmap_setup;
*/
    strncpy(b->pins[0].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[1].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[2].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[3].name, "I2C1_SDA", MRAA_PIN_NAME_SIZE); // GPIO0_C3 
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[3].gpio.pinmap = 19;

    strncpy(b->pins[4].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[5].name, "I2C1_SCL", MRAA_PIN_NAME_SIZE); // GPIO0_C2
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[5].gpio.pinmap = 18;

    strncpy(b->pins[6].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[7].name, "GPIO3_C4", MRAA_PIN_NAME_SIZE); // GPIO3_C4
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.pinmap = 116;

    strncpy(b->pins[8].name, "UART0_TX", MRAA_PIN_NAME_SIZE); // GPIO0_B2
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[8].gpio.pinmap = 10;

    strncpy(b->pins[9].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[10].name, "UART0_RX", MRAA_PIN_NAME_SIZE); // GPIO0_B3
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[10].gpio.pinmap = 11;

    strncpy(b->pins[11].name, "GPIO3_C6", MRAA_PIN_NAME_SIZE); // GPIO3_C6
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[11].gpio.pinmap = 118;

    strncpy(b->pins[12].name, "GPIO3_C5", MRAA_PIN_NAME_SIZE); // GPIO3_C5
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[12].gpio.pinmap = 117;

    strncpy(b->pins[13].name, "GPIO3_B3", MRAA_PIN_NAME_SIZE); // GPIO3_B3
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.pinmap = 107;

    strncpy(b->pins[14].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[15].name, "GPIO3_B4", MRAA_PIN_NAME_SIZE); // GPIO3_B4
    b->pins[15].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[15].gpio.pinmap = 104;

    strncpy(b->pins[16].name, "GPIO3_B5", MRAA_PIN_NAME_SIZE); // GPIO3_B5
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[16].gpio.pinmap = 109;

    strncpy(b->pins[17].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[18].name, "GPIO3_D1", MRAA_PIN_NAME_SIZE); // GPIO3_D1
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[18].gpio.pinmap = 121;

    strncpy(b->pins[19].name, "SPI0_MOSI", MRAA_PIN_NAME_SIZE); // GPIO1_B4
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[19].gpio.pinmap = 44;

    strncpy(b->pins[20].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[21].name, "SPI0_MISO", MRAA_PIN_NAME_SIZE); // GPIO1_B5
    b->pins[21].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[21].gpio.pinmap = 45;

    strncpy(b->pins[22].name, "GPIO3_D2", MRAA_PIN_NAME_SIZE); // GPIO3_D2
    b->pins[22].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[22].gpio.pinmap = 122;

    strncpy(b->pins[23].name, "SPI0_CLK", MRAA_PIN_NAME_SIZE); // GPIO1_B7
    b->pins[23].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[23].gpio.pinmap = 47;

    strncpy(b->pins[24].name, "SPI0_CSN", MRAA_PIN_NAME_SIZE); // GPIO1_B6
    b->pins[24].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[24].gpio.pinmap = 46;

    strncpy(b->pins[25].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[25].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[26].name, "SPI0_CS1", MRAA_PIN_NAME_SIZE); // NC 
    b->pins[26].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[27].name, "I2C0_SDA", MRAA_PIN_NAME_SIZE); // GPIO0_B1
    b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[27].gpio.pinmap = 9;

    strncpy(b->pins[28].name, "I2C0_SCL", MRAA_PIN_NAME_SIZE); // GPIO0_B0
    b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[28].gpio.pinmap = 8;

    strncpy(b->pins[29].name, "EGPIO1_0", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[29].gpio.pinmap = 496;

    b->pins[29].pwm.parent_id = 3;
    b->pins[29].pwm.pinmap = 0;
    b->pwm_dev_count++;

    strncpy(b->pins[30].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[31].name, "EGPIO1_1", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[31].gpio.pinmap = 497;

    b->pins[31].pwm.parent_id = 3;
    b->pins[31].pwm.pinmap = 1;
    b->pwm_dev_count++;

    strncpy(b->pins[32].name, "EGPIO1_2", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[32].gpio.pinmap = 498;

    b->pins[32].pwm.parent_id = 3;
    b->pins[32].pwm.pinmap = 2;
    b->pwm_dev_count++;

    strncpy(b->pins[33].name, "EGPIO1_3", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[33].gpio.pinmap = 499;

    b->pins[33].pwm.parent_id = 3;
    b->pins[33].pwm.pinmap = 3;
    b->pwm_dev_count++;

    strncpy(b->pins[34].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[35].name, "EGPIO1_4", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[35].gpio.pinmap = 500;

    b->pins[35].pwm.parent_id = 3;
    b->pins[35].pwm.pinmap = 4;
    b->pwm_dev_count++;

    strncpy(b->pins[36].name, "EGPIO1_5", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[36].gpio.pinmap = 501;

    b->pins[36].pwm.parent_id = 3;
    b->pins[36].pwm.pinmap = 5;
    b->pwm_dev_count++;

    strncpy(b->pins[37].name, "EGPIO1_6", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[37].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[37].gpio.pinmap = 502;

    b->pins[37].pwm.parent_id = 3;
    b->pins[37].pwm.pinmap = 6;
    b->pwm_dev_count++;

    strncpy(b->pins[38].name, "EGPIO1_7", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[38].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[38].gpio.pinmap = 503;

    b->pins[38].pwm.parent_id = 3;
    b->pins[38].pwm.pinmap = 7;
    b->pwm_dev_count++;

    strncpy(b->pins[39].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[39].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[40].name, "EGPIO2_8", MRAA_PIN_NAME_SIZE); // Expander GPIO
    b->pins[40].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[40].gpio.pinmap = 504;

    b->pins[40].pwm.parent_id = 3;
    b->pins[40].pwm.pinmap = 8;
    b->pwm_dev_count++;

    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->gpio_count = 0;
    int i;
    for (i = 0; i < b->phy_pin_count; i++) {
        if (b->pins[i].capabilities.gpio) {
            b->gpio_count++;
        }
    }

    // BUS DEFINITIONS
    b->i2c_bus_count = 0;
    b->def_i2c_bus = 0;
    if (i2c0 >= 0) {
	b->def_i2c_bus = b->i2c_bus_count;
        b->i2c_bus[b->i2c_bus_count].bus_id = i2c0;
        b->i2c_bus[b->i2c_bus_count].sda = 27;
        b->i2c_bus[b->i2c_bus_count].scl = 28;
        b->i2c_bus_count++;
    }

    if (i2c1 >= 0) {
	b->def_i2c_bus = b->i2c_bus_count;
        b->i2c_bus[b->i2c_bus_count].bus_id = i2c1;
        b->i2c_bus[b->i2c_bus_count].sda = 3;
        b->i2c_bus[b->i2c_bus_count].scl = 5;
        b->i2c_bus_count++;
    }

    b->spi_bus_count = 0;
    b->def_spi_bus = 0;
    if (spi0 >= 0) {
        b->spi_bus[b->spi_bus_count].bus_id = spi0;
        b->spi_bus[b->spi_bus_count].slave_s = 0;
        b->spi_bus[b->spi_bus_count].cs = 24;
        b->spi_bus[b->spi_bus_count].mosi = 19;
        b->spi_bus[b->spi_bus_count].miso = 21;
        b->spi_bus[b->spi_bus_count].sclk = 23;
        b->spi_bus_count++;
    }

    b->uart_dev_count = 0;
    b->def_uart_dev = 0;

    if (uart0 >= 0) {
        b->uart_dev[b->uart_dev_count].device_path = (char *)serialdev[uart0];
        b->uart_dev[b->uart_dev_count].rx = 10;
        b->uart_dev[b->uart_dev_count].tx = 8;
        b->uart_dev_count++;
    }

    if(sx150x_init() < 0)
    {
	    _fd = -1;
    }

    return b;
}
