/*
 * Author: Norbert Wesp <nwesp@phytec.de>
 * Author: Stefan Müller-Klieser <S.Mueller-Klieser@phytec.de>
 * Copyright (c) 2016 Phytec Messtechnik GmbH.
 *
 * Based on src/arm/beaglebone.c
 *
 * SPDX-License-Identifier: MIT
 */

#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <mraa/types.h>

#include "common.h"
#include "arm/am335x.h"

// MMAP
static uint8_t* mmap_gpio[4] = { NULL, NULL, NULL, NULL };
static int mmap_fd = 0;
static unsigned int mmap_count = 0;

mraa_result_t
mraa_am335x_mmap_write(mraa_gpio_context dev, int value)
{
    if (value) {
        *(volatile uint32_t*) (mmap_gpio[dev->pin / 32] + AM335X_SET) = (uint32_t)(1 << (dev->pin % 32));
    } else {
        *(volatile uint32_t*) (mmap_gpio[dev->pin / 32] + AM335X_CLR) = (uint32_t)(1 << (dev->pin % 32));
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_am335x_mmap_unsetup()
{
    if (mmap_gpio[0] == NULL) {
        syslog(LOG_ERR, "am335x mmap: null register cant unsetup");

        return MRAA_ERROR_INVALID_RESOURCE;
    }
    munmap(mmap_gpio[0], AM335X_GPIO_SIZE);
    mmap_gpio[0] = NULL;
    munmap(mmap_gpio[1], AM335X_GPIO_SIZE);
    mmap_gpio[1] = NULL;
    munmap(mmap_gpio[2], AM335X_GPIO_SIZE);
    mmap_gpio[2] = NULL;
    munmap(mmap_gpio[3], AM335X_GPIO_SIZE);
    mmap_gpio[3] = NULL;
    if (close(mmap_fd) != 0) {
        return MRAA_ERROR_INVALID_RESOURCE;
    }
    return MRAA_SUCCESS;
}

int
mraa_am335x_mmap_read(mraa_gpio_context dev)
{
    uint32_t value = *(volatile uint32_t*) (mmap_gpio[dev->pin / 32] + AM335X_IN);
    if (value & (uint32_t)(1 << (dev->pin % 32))) {
        return 1;
    }

    return 0;
}

mraa_result_t
mraa_am335x_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "am335x mmap: context not valid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (en == 0) {
        if (dev->mmap_write == NULL && dev->mmap_read == NULL) {
            syslog(LOG_ERR, "am335x mmap: can't disable disabled mmap gpio");
            return MRAA_ERROR_INVALID_PARAMETER;
        }
        dev->mmap_write = NULL;
        dev->mmap_read = NULL;
        mmap_count--;
        if (mmap_count == 0) {
            return mraa_am335x_mmap_unsetup();
        }
        return MRAA_SUCCESS;
    }

    if (dev->mmap_write != NULL && dev->mmap_read != NULL) {
        syslog(LOG_ERR, "am335x mmap: can't enable enabled mmap gpio");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    // Might need to make some elements of this thread safe.
    // For example only allow one thread to enter the following block
    // to prevent mmap'ing twice.
    if (mmap_gpio[0] == NULL) {
        if ((mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
            syslog(LOG_ERR, "am335x map: unable to open resource file");
            return MRAA_ERROR_INVALID_HANDLE;
        }

        mmap_gpio[0] = (uint8_t*) mmap(NULL, AM335X_GPIO_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_FILE | MAP_SHARED, mmap_fd, AM335X_GPIO0_BASE);
        if (mmap_gpio[0] == MAP_FAILED) {
            syslog(LOG_ERR, "am335x mmap: failed to mmap");
            mmap_gpio[0] = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
        mmap_gpio[1] = (uint8_t*) mmap(NULL, AM335X_GPIO_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_FILE | MAP_SHARED, mmap_fd, AM335X_GPIO1_BASE);
        if (mmap_gpio[1] == MAP_FAILED) {
            syslog(LOG_ERR, "am335x mmap: failed to mmap");
            mmap_gpio[1] = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
        mmap_gpio[2] = (uint8_t*) mmap(NULL, AM335X_GPIO_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_FILE | MAP_SHARED, mmap_fd, AM335X_GPIO2_BASE);
        if (mmap_gpio[2] == MAP_FAILED) {
            syslog(LOG_ERR, "am335x mmap: failed to mmap");
            mmap_gpio[2] = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
        mmap_gpio[3] = (uint8_t*) mmap(NULL, AM335X_GPIO_SIZE, PROT_READ | PROT_WRITE,
                                       MAP_FILE | MAP_SHARED, mmap_fd, AM335X_GPIO3_BASE);
        if (mmap_gpio[3] == MAP_FAILED) {
            syslog(LOG_ERR, "am335x mmap: failed to mmap");
            mmap_gpio[3] = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    dev->mmap_write = &mraa_am335x_mmap_write;
    dev->mmap_read = &mraa_am335x_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}