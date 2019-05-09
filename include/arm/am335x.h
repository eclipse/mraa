/*
 * Author: Norbert Wesp <nwesp@phytec.de>
 * Author: Stefan Müller-Klieser <S.Mueller-Klieser@phytec.de>
 * Copyright (c) 2016 Phytec Messtechnik GmbH.
 *
 * Based on src/arm/beaglebone.c
 *
 * SPDX-License-Identifier: MIT
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <mraa/types.h>

#include "common.h"

#define SYSFS_CLASS_PWM "/sys/class/pwm/"
#define SYSFS_CLASS_MMC "/sys/class/mmc_host/"

#define MMAP_PATH "/dev/mem"
#define MAX_SIZE 64

#define AM335X_GPIO0_BASE 0x44e07000
#define AM335X_GPIO1_BASE 0x4804c000
#define AM335X_GPIO2_BASE 0x481AC000
#define AM335X_GPIO3_BASE 0x481AE000
#define AM335X_GPIO_SIZE (4 * 1024)
#define AM335X_IN 0x138
#define AM335X_CLR 0x190
#define AM335X_SET 0x194

/**
 * Writes 'value' to gpio_context 'dev'
 *
 * @return mraa_result_t indicating success of actions.
 */
mraa_result_t mraa_am335x_mmap_write(mraa_gpio_context dev, int value);

/**
 * Unsetup register of mmap_gpio[]
 *
 * @return mraa_result_t indicating success of actions.
 */
mraa_result_t mraa_am335x_mmap_unsetup();

/**
 * Read from gpio_context 'dev'
 *
 * @return int result indicating success of actions.
 */
int mraa_am335x_mmap_read(mraa_gpio_context dev);

/**
 * Disables gpio_context 'dev' in case of 'en' or mmap'ing gpio_context 'dev'
 *
 * @return mraa_result_t indicating success of actions.
 */
mraa_result_t mraa_am335x_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en);

#ifdef __cplusplus
}
#endif
