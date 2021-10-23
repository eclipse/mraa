/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <glob.h>
#include <mraa/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "common.h"
#include "arm/beaglebone.h"
#include "arm/am335x.h"

#define NUM2STR(x) #x
#define P8 (8)
#define P9 (9)
#define PINS_PER_BUS (46)

#define BUILD_PIN(BUS, PIN) (((BUS - 8) * PINS_PER_BUS) + PIN)

// 76 => P9 (raw value 9)
#define GET_PIN_BUS(PIN) (P8 + (PIN / PINS_PER_BUS))

// 76 => 30
#define GET_RAW_PIN(PIN) (PIN % PINS_PER_BUS)

#define PLATFORM_NAME_BEAGLEBONE_BLACK_REV_B "Beaglebone Black Rev. B"
#define PLATFORM_NAME_BEAGLEBONE_BLACK_REV_C "Beaglebone Black Rev. C"

#define SYSFS_DEVICES_CAPEMGR_SLOTS "/sys/devices/platform/bone_capemgr/slots"
#define PLATFORM_OCP_PATH "/sys/devices/platform/ocp/"

// Copied from https://github.com/jadonk/bonescript/blob/master/src/bone.js
// See am335x technical manual, p. 183, for more info:
// http://www.ti.com/lit/ug/spruh73n/spruh73n.pdf

typedef struct {
    unsigned int index;
    unsigned int pid;
    const char* chip;
    const char* address;
} pwm_t;
pwm_t pwm_table[] = {
    { 1, BUILD_PIN(P8, 13), "48304000", "48304200" }, // "P8_13"
    { 0, BUILD_PIN(P8, 19), "48304000", "48304200" }, // "P8_19"
    { 1, BUILD_PIN(P8, 34), "48302000", "48302200" }, // "P8_34"
    { 0, BUILD_PIN(P8, 36), "48302000", "48302200" }, // "P8_36"
    { 0, BUILD_PIN(P8, 45), "48304000", "48304200" }, // "P8_45"
    { 1, BUILD_PIN(P8, 46), "48304000", "48304200" }, // "P8_46"
    { 0, BUILD_PIN(P9, 14), "48302000", "48302200" }, // "P9_14"
    { 1, BUILD_PIN(P9, 16), "48302000", "48302200" }, // "P9_16"
    { 1, BUILD_PIN(P9, 21), "48300000", "48300200" }, // "P9_21"
    { 0, BUILD_PIN(P9, 22), "48300000", "48300200" }, // "P9_22"
    { 0, BUILD_PIN(P9, 28), "48304000", "48304100" }, // "P9_28"
    { 1, BUILD_PIN(P9, 29), "48300000", "48300200" }, // "P9_29"
    { 0, BUILD_PIN(P9, 31), "48300000", "48300200" }, // "P9_31"
    { 0, BUILD_PIN(P9, 42), "48300000", "48300100" }, // "P9_42"
};

static const pwm_t*
pwm_from_pin(int pin)
{
    size_t num_chips = sizeof(pwm_table) / sizeof(pwm_table[0]);
    int i = 0;

    for (i = 0; i < num_chips; i++) {
        if (pwm_table[i].pid == pin) {
            return &pwm_table[i];
        }
    }

    return NULL;
}

// Used for PWM
// Given a partial path and a prefix, glob for the first result maching the prefix
// eg. build_path("/home/me/", "file*", res_buffer, sizeof(res_buffer))
// If there's a file called file1 in /home/me, res_buffer will be /home/me/file1
static mraa_result_t
build_path(const char* partial_path, const char* prefix, char* full_path, size_t full_path_len)
{
    glob_t results;
    size_t len = strlen(partial_path) + strlen(prefix) + 5;
    char* pattern = malloc(len);
    snprintf(pattern, len, "%s/%s*", partial_path, prefix);

    int err = glob(pattern, 0, NULL, &results);
    free(pattern);
    if (err != 0) {
        globfree(&results);
        if (err == GLOB_NOSPACE)
            return MRAA_ERROR_UNSPECIFIED;
        else
            return MRAA_ERROR_UNSPECIFIED;
    }

    // We will return the first match
    strncpy(full_path, results.gl_pathv[0], full_path_len);

    // Free memory
    globfree(&results);

    return MRAA_SUCCESS;
}

// Given a pin, find the chip id (eg. pwmchipX)
static mraa_result_t
mraa_get_pwm_chip_id(int pin, int* chip_id)
{
    // Get device path
    mraa_result_t ret = MRAA_ERROR_UNSPECIFIED;

    const pwm_t* pwm_chip = pwm_from_pin(pin);

    if (pwm_chip) {
        // /sys/devices/platform/ocp/48300000.*
        char ocp_path[128];
        ret = build_path(PLATFORM_OCP_PATH, pwm_chip->chip, ocp_path, sizeof(ocp_path));

        if (ret != MRAA_SUCCESS)
            return ret;

        // ocp path should now be something like
        // /sys/devices/platform/ocp/48300000.epwmss/
        char dev_path[128];
        ret = build_path(ocp_path, pwm_chip->address, dev_path, sizeof(dev_path));

        if (ret != MRAA_SUCCESS)
            return ret;

        // dev path should now be something like
        // /sys/devices/platform/ocp/48300000.epwmss/48300200.pwm/
        char addr_path[128];
        ret = build_path(dev_path, "pwm/pwmchip", addr_path, sizeof(addr_path));

        if (ret != MRAA_SUCCESS)
            return ret;

        // Grab the integer chip id
        char* chip_id_pos = strstr(addr_path, "pwmchip");

        if (chip_id_pos) {
            chip_id_pos += strlen("pwmchip");
            *chip_id = strtol(chip_id_pos, NULL, 10);
            ret = MRAA_SUCCESS;
        }
    }

    return ret;
}

static mraa_result_t
set_pin_mode(int pin, const char* mode)
{
    // char ocp_dir[] defined in common.h
    char path[MAX_SIZE];       // "/sys/devices/platform/ocp/ocp:P#_##_pinmux/state"
    char pinmux_dir[MAX_SIZE]; // "ocp:P#_##_pinmux"
    char pin_str[MAX_SIZE];    //"P#_##"
    FILE* f = NULL;

    int pin_bus = GET_PIN_BUS(pin);
    int pin_num = GET_RAW_PIN(pin);

    snprintf(pin_str, sizeof(pin_str), "P%1d_%02d", pin_bus, pin_num);
    snprintf(pinmux_dir, sizeof(pinmux_dir), "ocp:%s_pinmux", pin_str);
    snprintf(path, sizeof(path), "%s/%s/state", PLATFORM_OCP_PATH, pinmux_dir);

    f = fopen(path, "w");
    if (NULL == f) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    fprintf(f, "%s", mode);
    fclose(f);

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_beaglebone_uart_init_pre(int index)
{
    mraa_result_t ret = MRAA_ERROR_NO_RESOURCES;
    char devpath[MAX_SIZE];
    sprintf(devpath, "/dev/ttyO%u", index + 1);
    if (!mraa_file_exist(devpath)) {
        ret = MRAA_ERROR_INVALID_HANDLE;
    }
    if (mraa_file_exist(devpath)) {
       if (set_pin_mode(plat->uart_dev[index].rx, "uart") == MRAA_SUCCESS &&
            set_pin_mode(plat->uart_dev[index].tx, "uart") == MRAA_SUCCESS) {
            return MRAA_SUCCESS;
        }
    } else {
        syslog(LOG_ERR, "uart: Device not initialized");
    }
    return ret;
}

mraa_result_t
mraa_beaglebone_spi_init_pre(int index)
{
    mraa_result_t ret = MRAA_ERROR_NO_RESOURCES;
    char devpath[MAX_SIZE];
    int deviceindex = 0;

    // The first initialized SPI devices always gets the bus id 1
    // So we need to track down correct mapping and adjust the bus_id field
    if ((index == 0) && mraa_link_targets("/sys/class/spidev/spidev1.0", "48030000"))
        deviceindex = 1;
    if ((index == 0) && mraa_link_targets("/sys/class/spidev/spidev2.0", "48030000"))
        deviceindex = 2;
    if ((index == 1) && mraa_link_targets("/sys/class/spidev/spidev1.0", "481a0000"))
        deviceindex = 1;
    if ((index == 1) && mraa_link_targets("/sys/class/spidev/spidev2.0", "481a0000"))
        deviceindex = 2;
    if ((deviceindex == 0) && mraa_file_exist("/sys/class/spidev/spidev1.0"))
        deviceindex = 2;
    if (deviceindex == 0)
        deviceindex = 1;

    sprintf(devpath, "/dev/spidev%u.0", deviceindex);
    if (!mraa_file_exist(devpath)) {
        ret = MRAA_ERROR_INVALID_HANDLE;
    }
    if (mraa_file_exist(devpath)) {
        plat->spi_bus[index].bus_id = deviceindex;
        if (set_pin_mode(plat->spi_bus[index].cs, "spi") == MRAA_SUCCESS &&
            set_pin_mode(plat->spi_bus[index].mosi, "spi") == MRAA_SUCCESS &&
            set_pin_mode(plat->spi_bus[index].miso, "spi") == MRAA_SUCCESS &&
            set_pin_mode(plat->spi_bus[index].sclk, "spi") == MRAA_SUCCESS) {
            ret = MRAA_SUCCESS;
        }
        ret = MRAA_SUCCESS;
    } else {
        syslog(LOG_ERR, "spi: Check http://elinux.org/BeagleBone_Black_Enable_SPIDEV for details");
    }
    return ret;
}

mraa_result_t
mraa_beaglebone_i2c_init_pre(unsigned int bus)
{
    mraa_result_t ret = MRAA_ERROR_NO_RESOURCES;
    char devpath[MAX_SIZE];

    snprintf(devpath, MAX_SIZE, "/dev/i2c-%u", bus);

    if (mraa_file_exist(devpath)) {
        // Bus 1 doesn't seem to be configurable
        if (bus == 0) {
            if (set_pin_mode(plat->i2c_bus[0].scl, "i2c") != MRAA_SUCCESS ||
                set_pin_mode(plat->i2c_bus[0].sda, "i2c") != MRAA_SUCCESS) {
                return MRAA_ERROR_UNSPECIFIED;
            }
        }

        ret = MRAA_SUCCESS;
    } else {
        syslog(LOG_ERR, "i2c: Device %s not initialized", devpath);
        ret = MRAA_ERROR_INVALID_HANDLE;
    }
    return ret;
}

mraa_pwm_context
mraa_beaglebone_pwm_init_replace(int pin)
{
    char devpath[MAX_SIZE];
    char chippath[MAX_SIZE];
    char pinmode[5];

    if (plat == NULL) {
        syslog(LOG_ERR, "pwm: Platform Not Initialised");
        return NULL;
    }
    if (plat->pins[pin].capabilities.pwm != 1) {
        syslog(LOG_ERR, "pwm: pin %d not capable of pwm", pin);
        return NULL;
    }

    // Special case gleaned from adafruit BBIO
    if (pin == (BUILD_PIN(P9, 28))) {
        sprintf(pinmode, "pwm2");
    } else {
        sprintf(pinmode, "pwm");
    }

    if (set_pin_mode(pin, pinmode) != MRAA_SUCCESS) {
        syslog(LOG_ERR, "pwm: pin %d not capable of pwm", pin);
        return NULL;
    }

    int chip_id = 0;
    if (mraa_get_pwm_chip_id(pin, &chip_id) != MRAA_SUCCESS) {
        syslog(LOG_ERR, "pwm: Unable to find chip ID for pin %d", pin);
        return NULL;
    }

    sprintf(chippath, SYSFS_CLASS_PWM "pwmchip%u", chip_id);

    if (!mraa_file_exist(chippath)) {
        syslog(LOG_ERR, "pwm: Chip path %s does not exist for pin %d", chippath, pin);
        return NULL;
    }

    const pwm_t* pwm_chip = pwm_from_pin(pin);

    if (!pwm_chip) {
        syslog(LOG_ERR, "pwm: No pwm definition for pin %d", pin);
        return NULL;
    }

    sprintf(devpath, SYSFS_CLASS_PWM "pwmchip%u/pwm%u", chip_id, pwm_chip->index);

    if (!mraa_file_exist(devpath)) {
        FILE* fh;
        char exportpath[MAX_SIZE];
        sprintf(exportpath, "%s/export", chippath);
        fh = fopen(exportpath, "w");
        if (fh == NULL) {
            syslog(LOG_ERR, "pwm: Failed to open %s for writing, check access "
                            "rights for user",
                   exportpath);
            return NULL;
        }
        if (fprintf(fh, "%d", pwm_chip->index) < 0) {
            syslog(LOG_ERR, "pwm: Failed to write to CapeManager for pin %d", pin);
        }
        fclose(fh);
    }

    if (mraa_file_exist(devpath)) {
        mraa_pwm_context dev = (mraa_pwm_context) calloc(1, sizeof(struct _pwm));
        if (dev == NULL)
            return NULL;
        dev->duty_fp = -1;
        dev->chipid = chip_id;
        dev->pin = pwm_chip->index;
        dev->period = -1;
        return dev;
    } else
        syslog(LOG_ERR, "pwm: pin %d not initialized", pin);
    return NULL;
}

mraa_board_t*
mraa_beaglebone()
{
    unsigned int emmc_enabled = 1;
    unsigned int hdmi_enabled = 1;
    unsigned int pcm0_enabled = 1;
    unsigned int i2c0_enabled = 1;
    unsigned int i2c1_enabled = 1;
    unsigned int spi0_enabled = 0;
    unsigned int spi1_enabled = 0;
    unsigned int uart1_enabled = 0;
    unsigned int uart2_enabled = 0;
    unsigned int uart3_enabled = 0;
    unsigned int uart4_enabled = 0;
    unsigned int uart5_enabled = 0;
    unsigned int ehrpwm0a_enabled = 0;
    unsigned int ehrpwm0b_enabled = 0;
    unsigned int ehrpwm1a_enabled = 0;
    unsigned int ehrpwm1b_enabled = 0;
    unsigned int ehrpwm2a_enabled = 0;
    unsigned int ehrpwm2b_enabled = 0;
    unsigned int is_rev_c = 0;
    size_t len = 0;
    char* line = NULL;

    FILE* fh;
    fh = fopen(SYSFS_CLASS_MMC "mmc1/mmc1:0001/name", "r");
    if (fh != NULL) {
        emmc_enabled = 1;
        if (getline(&line, &len, fh) != -1) {
            if (strstr(line, "MMC04G")) {
                is_rev_c = 1;
            }
        }
        fclose(fh);
        free(line);
    } else
        emmc_enabled = 0;


    if (mraa_file_exist("/sys/devices/ocp.*/hdmi.*"))
        hdmi_enabled = 1;
    else
        hdmi_enabled = 0;

    if (mraa_file_exist("/sys/class/sound/pcmC0D0p"))
        pcm0_enabled = 1;
    else
        pcm0_enabled = 0;

    if (mraa_file_exist("/sys/class/i2c-dev/i2c-0"))
        i2c0_enabled = 1;
    else
        i2c0_enabled = 0;

    if (mraa_file_exist("/sys/class/i2c-dev/i2c-2"))
        i2c1_enabled = 1;
    else
        i2c1_enabled = 0;

    if (mraa_file_exist("/sys/class/spidev/spidev1.0"))
        spi0_enabled = 1;
    else
        spi0_enabled = 0;

    if (mraa_file_exist("/sys/class/spidev/spidev2.0"))
        spi1_enabled = 1;
    else
        spi1_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO1"))
        uart1_enabled = 1;
    else
        uart1_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO2"))
        uart2_enabled = 1;
    else
        uart2_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO3"))
        uart3_enabled = 1;
    else
        uart3_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO4"))
        uart4_enabled = 1;
    else
        uart4_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO5"))
        uart5_enabled = 1;
    else
        uart5_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip0"))
        ehrpwm0a_enabled = 1;
    else
        ehrpwm0a_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip1"))
        ehrpwm0b_enabled = 1;
    else
        ehrpwm0b_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip3"))
        ehrpwm1a_enabled = 1;
    else
        ehrpwm1a_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip4"))
        ehrpwm1b_enabled = 1;
    else
        ehrpwm1b_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip5"))
        ehrpwm2a_enabled = 1;
    else
        ehrpwm2a_enabled = 0;

    if (mraa_file_exist("/sys/class/pwm/pwmchip6"))
        ehrpwm2b_enabled = 1;
    else
        ehrpwm2b_enabled = 0;

    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL)
        return NULL;
    // TODO: Detect Beaglebone Black Revisions, for now always TYPE B
    if (is_rev_c == 0) {
        b->platform_name = PLATFORM_NAME_BEAGLEBONE_BLACK_REV_B;
        b->phy_pin_count = MRAA_BEAGLEBONE_BLACK_PINCOUNT;
    }
    if (is_rev_c == 1) {
        b->platform_name = PLATFORM_NAME_BEAGLEBONE_BLACK_REV_C;
        b->phy_pin_count = MRAA_BEAGLEBONE_BLACK_PINCOUNT;
    }

    if (b->platform_name == NULL) {
        goto error;
    }

    b->aio_count = 7;
    b->adc_raw = 12;
    b->adc_supported = 12;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count, sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    b->adv_func->uart_init_pre = &mraa_beaglebone_uart_init_pre;
    b->adv_func->spi_init_pre = &mraa_beaglebone_spi_init_pre;
    b->adv_func->i2c_init_pre = &mraa_beaglebone_i2c_init_pre;
    b->adv_func->pwm_init_replace = &mraa_beaglebone_pwm_init_replace;

    strncpy(b->pins[BUILD_PIN(P8, 0)].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 0)].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P8, 1)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 1)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P8, 2)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 2)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 3)].name, "MMC1_D6", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 3)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 3)].name, "GPIO38", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 3)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 3)].gpio.pinmap = 38;
    b->pins[BUILD_PIN(P8, 3)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 3)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 4)].name, "MMC1_D7", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 4)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 4)].name, "GPIO39", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 4)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 4)].gpio.pinmap = 39;
    b->pins[BUILD_PIN(P8, 4)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 4)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 5)].name, "MMC1_D2", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 5)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 5)].name, "GPIO34", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 5)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 5)].gpio.pinmap = 34;
    b->pins[BUILD_PIN(P8, 5)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 5)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 6)].name, "MMC1_D3", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 6)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 6)].name, "GPIO35", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 6)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 6)].gpio.pinmap = 35;
    b->pins[BUILD_PIN(P8, 6)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 6)].gpio.mux_total = 0;

    // TODO TIMER4
    strncpy(b->pins[BUILD_PIN(P8, 7)].name, "GPIO66", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 7)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 7)].gpio.pinmap = 66;
    b->pins[BUILD_PIN(P8, 7)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 7)].gpio.mux_total = 0;

    // TODO TIMER7
    strncpy(b->pins[BUILD_PIN(P8, 8)].name, "GPIO67", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 8)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 8)].gpio.pinmap = 67;
    b->pins[BUILD_PIN(P8, 8)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 8)].gpio.mux_total = 0;

    // TODO TIMER5
    strncpy(b->pins[BUILD_PIN(P8, 9)].name, "GPIO69", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 9)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 9)].gpio.pinmap = 69;
    b->pins[BUILD_PIN(P8, 9)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 9)].gpio.mux_total = 0;

    // TODO TIMER6
    strncpy(b->pins[BUILD_PIN(P8, 10)].name, "GPIO68", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 10)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 10)].gpio.pinmap = 68;
    b->pins[BUILD_PIN(P8, 10)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 10)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 11)].name, "GPIO45", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 11)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 11)].gpio.pinmap = 45;
    b->pins[BUILD_PIN(P8, 11)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 11)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 12)].name, "GPIO44", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 12)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 12)].gpio.pinmap = 44;
    b->pins[BUILD_PIN(P8, 12)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 12)].gpio.mux_total = 0;

    if (ehrpwm2b_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 13)].name, "EHRPWM2B", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 13)].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 13)].name, "GPIO23", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 13)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 13)].gpio.pinmap = 23;
    b->pins[BUILD_PIN(P8, 13)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 13)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 13)].pwm.pinmap = 6;
    b->pins[BUILD_PIN(P8, 13)].pwm.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 14)].name, "GPIO26", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 14)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 14)].gpio.pinmap = 26;
    b->pins[BUILD_PIN(P8, 14)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 14)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 15)].name, "GPIO47", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 15)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 15)].gpio.pinmap = 47;
    b->pins[BUILD_PIN(P8, 15)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 15)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 16)].name, "GPIO46", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 16)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 16)].gpio.pinmap = 46;
    b->pins[BUILD_PIN(P8, 16)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 16)].gpio.mux_total = 0;

    // TODO PWM0_SYNCO
    strncpy(b->pins[BUILD_PIN(P8, 17)].name, "GPIO27", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 17)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 17)].gpio.pinmap = 27;
    b->pins[BUILD_PIN(P8, 17)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 17)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 18)].name, "GPIO65", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 18)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 18)].gpio.pinmap = 65;
    b->pins[BUILD_PIN(P8, 18)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 18)].gpio.mux_total = 0;

    if (ehrpwm2a_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 19)].name, "EHRPWM2A", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 19)].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 19)].name, "GPIO22", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 19)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 19)].gpio.pinmap = 22;
    b->pins[BUILD_PIN(P8, 19)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 19)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 19)].pwm.pinmap = 5;
    b->pins[BUILD_PIN(P8, 19)].pwm.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 20)].name, "MMC1_CMD", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 20)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 20)].name, "GPIO63", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 20)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 20)].gpio.pinmap = 63;
    b->pins[BUILD_PIN(P8, 20)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 20)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 21)].name, "MMC1_CLK", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 21)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 21)].name, "GPIO62", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 21)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 21)].gpio.pinmap = 62;
    b->pins[BUILD_PIN(P8, 21)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 21)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 22)].name, "MMC1_D5", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 22)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 22)].name, "GPIO37", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 22)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 22)].gpio.pinmap = 37;
    b->pins[BUILD_PIN(P8, 22)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 22)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 23)].name, "MMC_D4", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 23)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 23)].name, "GPIO36", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 23)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 23)].gpio.pinmap = 36;
    b->pins[BUILD_PIN(P8, 23)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 23)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 24)].name, "MMC_D1", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 24)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 24)].name, "GPIO33", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 24)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 24)].gpio.pinmap = 33;
    b->pins[BUILD_PIN(P8, 24)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 24)].gpio.mux_total = 0;

    if (emmc_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 25)].name, "MMC1_D0", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 25)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 25)].name, "GPIO32", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 25)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 25)].gpio.pinmap = 32;
    b->pins[BUILD_PIN(P8, 25)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 25)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P8, 26)].name, "GPIO61", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P8, 26)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P8, 26)].gpio.pinmap = 61;
    b->pins[BUILD_PIN(P8, 26)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 26)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 27)].name, "LCD_VSYNC", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 27)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 27)].name, "GPIO86", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 27)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 27)].gpio.pinmap = 86;
    b->pins[BUILD_PIN(P8, 27)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 27)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 28)].name, "LCD_PCLK", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 28)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 28)].name, "GPIO88", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 28)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 28)].gpio.pinmap = 88;
    b->pins[BUILD_PIN(P8, 28)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 28)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 29)].name, "LCD_HSYNC", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 29)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 29)].name, "GPIO87", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 29)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 29)].gpio.pinmap = 87;
    b->pins[BUILD_PIN(P8, 29)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 29)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 30)].name, "LCD_AC_BIAS", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 30)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 30)].name, "GPIO89", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 30)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 30)].gpio.pinmap = 89;
    b->pins[BUILD_PIN(P8, 30)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 30)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 31)].name, "LCD_D14", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 31)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        // TODO UART5_CTS this is ignored when using ADAFRUIT
        strncpy(b->pins[BUILD_PIN(P8, 31)].name, "GPIO10", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 31)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 31)].gpio.pinmap = 10;
    b->pins[BUILD_PIN(P8, 31)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 31)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 31)].uart.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 32)].name, "LCD_D15", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 32)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        // TODO UART5_RTS this is ignored when using ADAFRUIT
        strncpy(b->pins[BUILD_PIN(P8, 32)].name, "GPIO11", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 32)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 32)].gpio.pinmap = 11;
    b->pins[BUILD_PIN(P8, 32)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 32)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 32)].uart.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 33)].name, "LCD_D13", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 33)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        // TODO UART4_RTS this is ignored when using ADAFRUIT
        strncpy(b->pins[BUILD_PIN(P8, 33)].name, "GPIO9", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 33)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 33)].gpio.pinmap = 9;
    b->pins[BUILD_PIN(P8, 33)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 33)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 33)].uart.mux_total = 0;

    // TODO PWM_1B
    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 34)].name, "LCD_D11", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 34)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 34)].name, "GPIO81", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 34)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 34)].gpio.pinmap = 81;
    b->pins[BUILD_PIN(P8, 34)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 34)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 35)].name, "LCD_D12", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 35)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        // TODO UART4_CTS this is ignored when using ADAFRUIT
        strncpy(b->pins[BUILD_PIN(P8, 35)].name, "GPIO8", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 35)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 35)].gpio.pinmap = 8;
    b->pins[BUILD_PIN(P8, 35)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 35)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 35)].uart.mux_total = 0;

    // TODO PWM_1A
    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 36)].name, "LCD_D10", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 36)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 36)].name, "GPIO80", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 36)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 36)].gpio.pinmap = 80;
    b->pins[BUILD_PIN(P8, 36)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 36)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 37)].name, "LCD_D8", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 37)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        if (uart5_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P8, 37)].name, "UART5TX", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P8, 37)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
        } else {
            strncpy(b->pins[BUILD_PIN(P8, 37)].name, "GPIO78", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P8, 37)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
        }
    }
    b->pins[BUILD_PIN(P8, 37)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[BUILD_PIN(P8, 37)].gpio.pinmap = 78;
    b->pins[BUILD_PIN(P8, 37)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 37)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 37)].uart.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 38)].name, "LCD_D9", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 38)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        // TODO UART5_RX
        if (uart5_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P8, 38)].name, "UART5RX", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P8, 38)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
        } else {
            strncpy(b->pins[BUILD_PIN(P8, 38)].name, "GPIO79", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P8, 38)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
        }
    }
    b->pins[BUILD_PIN(P8, 38)].gpio.pinmap = 79;
    b->pins[BUILD_PIN(P8, 38)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 38)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P8, 38)].uart.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 39)].name, "LCD_D6", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 39)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 39)].name, "GPIO76", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 39)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 39)].gpio.pinmap = 76;
    b->pins[BUILD_PIN(P8, 39)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 39)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 40)].name, "LCD_D7", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 40)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 40)].name, "GPIO77", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 40)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 40)].gpio.pinmap = 77;
    b->pins[BUILD_PIN(P8, 40)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 40)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 41)].name, "LCD_D4", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 41)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 41)].name, "GPIO74", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 41)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 41)].gpio.pinmap = 74;
    b->pins[BUILD_PIN(P8, 41)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 41)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 42)].name, "LCD_D5", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 42)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 42)].name, "GPIO75", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 42)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 42)].gpio.pinmap = 75;
    b->pins[BUILD_PIN(P8, 42)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 42)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 43)].name, "LCD_D2", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 43)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 43)].name, "GPIO72", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 43)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 43)].gpio.pinmap = 72;
    b->pins[BUILD_PIN(P8, 43)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 43)].gpio.mux_total = 0;

    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 44)].name, "LCD_D3", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 44)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 44)].name, "GPIO73", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 44)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 44)].gpio.pinmap = 73;
    b->pins[BUILD_PIN(P8, 44)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 44)].gpio.mux_total = 0;

    // TODO PWM_2A
    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 45)].name, "LCD_D0", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 45)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 45)].name, "GPIO70", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 45)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 45)].gpio.pinmap = 70;
    b->pins[BUILD_PIN(P8, 45)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 45)].gpio.mux_total = 0;

    // TODO PWM_2B
    if (hdmi_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P8, 46)].name, "LCD_D1", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 46)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P8, 46)].name, "GPIO71", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P8, 46)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P8, 46)].gpio.pinmap = 71;
    b->pins[BUILD_PIN(P8, 46)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P8, 46)].gpio.mux_total = 0;
    strncpy(b->pins[BUILD_PIN(P9, 1)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 1)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 2)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 2)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 3)].name, "3.3V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 3)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 4)].name, "3.3V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 4)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 5)].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 5)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 6)].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 6)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 7)].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 7)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 8)].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 8)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 9)].name, "PWR", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 9)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[BUILD_PIN(P9, 10)].name, "RESET", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 10)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (uart4_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 11)].name, "UART4_RX", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 11)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 11)].name, "GPIO30", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 11)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 11)].gpio.pinmap = 30;
    b->pins[BUILD_PIN(P9, 11)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 11)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 11)].uart.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P9, 12)].name, "GPIO60", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 12)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 12)].gpio.pinmap = 60;
    b->pins[BUILD_PIN(P9, 12)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 12)].gpio.mux_total = 0;

    if (uart4_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 13)].name, "UART4_TX", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 13)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 13)].name, "GPIO31", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 13)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 13)].gpio.pinmap = 31;
    b->pins[BUILD_PIN(P9, 13)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 13)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 13)].uart.mux_total = 0;

    if (ehrpwm1a_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 14)].name, "EHRPWM1A", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 14)].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 14)].name, "GPIO50", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 14)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 14)].gpio.pinmap = 50;
    b->pins[BUILD_PIN(P9, 14)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 14)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 14)].pwm.pinmap = 3;
    b->pins[BUILD_PIN(P9, 14)].pwm.mux_total = 0;

    // TODO PWM_TRIP2_IN (not a PWM output, but used for sync cf ref. manual)
    strncpy(b->pins[BUILD_PIN(P9, 15)].name, "GPIO48", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 15)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 15)].gpio.pinmap = 48;
    b->pins[BUILD_PIN(P9, 15)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 15)].gpio.mux_total = 0;

    if (ehrpwm1b_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 16)].name, "EHRPWM1B", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 16)].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 16)].name, "GPIO51", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 16)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 16)].gpio.pinmap = 51;
    b->pins[BUILD_PIN(P9, 16)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 16)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 16)].pwm.pinmap = 4;
    b->pins[BUILD_PIN(P9, 16)].pwm.mux_total = 0;

    if ((i2c0_enabled == 1) || (spi0_enabled == 1)) {
        if (i2c0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 17)].name, "I2C1SCL", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 17)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 };
        }
        if (spi0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 17)].name, "SPI0CS0", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 17)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 17)].name, "GPIO4", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 17)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 17)].gpio.pinmap = 4;
    b->pins[BUILD_PIN(P9, 17)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 17)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 17)].i2c.mux_total = 0;
    b->pins[BUILD_PIN(P9, 17)].spi.mux_total = 0;

    if ((i2c0_enabled == 1) || (spi0_enabled == 1)) {
        if (i2c0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 18)].name, "I2C1SDA", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 18)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 };
        }
        if (spi0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 18)].name, "SPI0D1", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 18)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 18)].name, "GPIO5", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 18)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 1, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 18)].gpio.pinmap = 5;
    b->pins[BUILD_PIN(P9, 18)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 18)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 18)].i2c.mux_total = 0;
    b->pins[BUILD_PIN(P9, 18)].spi.mux_total = 0;

    if (i2c1_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 19)].name, "I2C2SCL", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 19)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
        b->pins[BUILD_PIN(P9, 19)].i2c.mux_total = 0;
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 19)].name, "GPIO13", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 19)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 19)].gpio.pinmap = 13;
    b->pins[BUILD_PIN(P9, 19)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 19)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 19)].i2c.mux_total = 0;

    if (i2c1_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 20)].name, "I2C2SDA", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 20)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
        b->pins[BUILD_PIN(P9, 20)].i2c.mux_total = 0;
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 20)].name, "GPIO12", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 20)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 20)].gpio.pinmap = 12;
    b->pins[BUILD_PIN(P9, 20)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 20)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 20)].i2c.mux_total = 0;

    if ((spi0_enabled == 1) || uart2_enabled == 1 || ehrpwm0b_enabled == 1) {
        if (uart2_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 21)].name, "UART2_TX", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 21)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
        }
        if (spi0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 21)].name, "SPI0D0", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 21)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        }
        if (ehrpwm0b_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 21)].name, "EHRPWM0B", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 21)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 21)].name, "GPIO3", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 21)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 1, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 21)].gpio.pinmap = 3;
    b->pins[BUILD_PIN(P9, 21)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 21)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 21)].spi.mux_total = 0;
    b->pins[BUILD_PIN(P9, 21)].uart.mux_total = 0;
    b->pins[BUILD_PIN(P9, 21)].pwm.pinmap = 1;
    b->pins[BUILD_PIN(P9, 21)].pwm.mux_total = 0;

    if ((spi0_enabled == 1) || uart2_enabled == 1 || ehrpwm0a_enabled == 1) {
        if (uart2_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 22)].name, "UART2_RX", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 22)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 1 };
        }
        if (spi0_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 22)].name, "SPI0CLK", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 22)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        }
        if (ehrpwm0a_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 22)].name, "EHRPWM0A", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 22)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 22)].name, "GPIO2", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 22)].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 1, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 22)].gpio.pinmap = 2;
    b->pins[BUILD_PIN(P9, 22)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 22)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 22)].spi.mux_total = 0;
    b->pins[BUILD_PIN(P9, 22)].uart.mux_total = 0;
    b->pins[BUILD_PIN(P9, 22)].pwm.pinmap = 0;
    b->pins[BUILD_PIN(P9, 22)].pwm.mux_total = 0;

    // TODO PWM0_SYNCO ?? PWM
    strncpy(b->pins[BUILD_PIN(P9, 23)].name, "GPIO49", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 23)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 23)].gpio.pinmap = 49;
    b->pins[BUILD_PIN(P9, 23)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 23)].gpio.mux_total = 0;

    if (uart1_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 24)].name, "UART1_RX", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 24)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 24)].name, "GPIO15", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 24)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 24)].gpio.pinmap = 15;
    b->pins[BUILD_PIN(P9, 24)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 24)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 24)].uart.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P9, 25)].name, "GPIO117", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 25)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 25)].gpio.pinmap = 117;
    b->pins[BUILD_PIN(P9, 25)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 25)].gpio.mux_total = 0;

    if (uart1_enabled == 1) {
        strncpy(b->pins[BUILD_PIN(P9, 26)].name, "UART1_RX", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 26)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 26)].name, "GPIO14", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 26)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[BUILD_PIN(P9, 26)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[BUILD_PIN(P9, 26)].gpio.pinmap = 14;
    b->pins[BUILD_PIN(P9, 26)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 26)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 26)].uart.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P9, 27)].name, "GPIO115", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 27)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 27)].gpio.pinmap = 115;
    b->pins[BUILD_PIN(P9, 27)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 27)].gpio.mux_total = 0;

    if (pcm0_enabled != 1) {
        if (spi1_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 28)].name, "SPI1CS0", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 28)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        } else {
            strncpy(b->pins[BUILD_PIN(P9, 28)].name, "GPIO113", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 28)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 28)].name, "MCASP0XX", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 28)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 28)].gpio.pinmap = 113;
    b->pins[BUILD_PIN(P9, 28)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 28)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 28)].spi.mux_total = 0;

    if (pcm0_enabled != 1) {
        if (spi1_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 29)].name, "SPI1D0", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 29)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        } else {
            strncpy(b->pins[BUILD_PIN(P9, 29)].name, "GPIO111", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 29)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 29)].name, "MMC1_SD", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 29)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 29)].gpio.pinmap = 111;
    b->pins[BUILD_PIN(P9, 29)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 29)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 29)].spi.mux_total = 0;

    if (pcm0_enabled != 1) {
        if (spi1_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 30)].name, "SPI1D1", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 30)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        } else {
            strncpy(b->pins[BUILD_PIN(P9, 30)].name, "GPIO112", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 30)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 30)].name, "MMC2_SD", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 30)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 30)].gpio.pinmap = 112;
    b->pins[BUILD_PIN(P9, 30)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 30)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 30)].spi.mux_total = 0;

    if (pcm0_enabled != 1) {
        if (spi1_enabled == 1) {
            strncpy(b->pins[BUILD_PIN(P9, 31)].name, "SPI1CLK", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 31)].capabilities =
            (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
        } else {
            strncpy(b->pins[BUILD_PIN(P9, 31)].name, "GPIO110", MRAA_PIN_NAME_SIZE);
            b->pins[BUILD_PIN(P9, 31)].capabilities =
            (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        }
    } else {
        strncpy(b->pins[BUILD_PIN(P9, 31)].name, "MMC0_SD", MRAA_PIN_NAME_SIZE);
        b->pins[BUILD_PIN(P9, 31)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    }
    b->pins[BUILD_PIN(P9, 31)].gpio.pinmap = 110;
    b->pins[BUILD_PIN(P9, 31)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 31)].gpio.mux_total = 0;
    b->pins[BUILD_PIN(P9, 31)].spi.mux_total = 0;


    strncpy(b->pins[BUILD_PIN(P9, 32)].name, "VDD_ADC", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 32)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // TODO AIN4
    strncpy(b->pins[BUILD_PIN(P9, 33)].name, "AIN4", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 33)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 33)].aio.pinmap = 4;

    strncpy(b->pins[BUILD_PIN(P9, 34)].name, "GND_ADC", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 34)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    // TODO AIN6
    strncpy(b->pins[BUILD_PIN(P9, 35)].name, "AIN6", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 35)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 35)].aio.pinmap = 6;
    // TODO AIN5
    strncpy(b->pins[BUILD_PIN(P9, 36)].name, "AIN5", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 36)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 36)].aio.pinmap = 5;
    // TODO AIN2
    strncpy(b->pins[BUILD_PIN(P9, 37)].name, "AIN2", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 37)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 37)].aio.pinmap = 2;
    // TODO AIN3
    strncpy(b->pins[BUILD_PIN(P9, 38)].name, "AIN3", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 38)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 38)].aio.pinmap = 3;
    // TODO AIN0
    strncpy(b->pins[BUILD_PIN(P9, 39)].name, "AIN0", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 39)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 39)].aio.pinmap = 0;
    // TODO AIN1
    strncpy(b->pins[BUILD_PIN(P9, 40)].name, "AIN1", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 40)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };
    b->pins[BUILD_PIN(P9, 40)].aio.pinmap = 1;

    strncpy(b->pins[BUILD_PIN(P9, 41)].name, "GPIO20", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 41)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 41)].gpio.pinmap = 20;
    b->pins[BUILD_PIN(P9, 41)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 41)].gpio.mux_total = 0;

    strncpy(b->pins[BUILD_PIN(P9, 42)].name, "GPIO7", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 42)].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[BUILD_PIN(P9, 42)].gpio.pinmap = 7;
    b->pins[BUILD_PIN(P9, 42)].gpio.parent_id = 0;
    b->pins[BUILD_PIN(P9, 42)].gpio.mux_total = 0;

    // GND
    strncpy(b->pins[BUILD_PIN(P9, 43)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 43)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // GND
    strncpy(b->pins[BUILD_PIN(P9, 44)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 44)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // GND
    strncpy(b->pins[BUILD_PIN(P9, 45)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 45)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // GND
    strncpy(b->pins[BUILD_PIN(P9, 46)].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[BUILD_PIN(P9, 46)].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // BUS DEFINITIONS
    b->i2c_bus_count = 2;
    b->def_i2c_bus = 0;

    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = BUILD_PIN(P9, 18);
    b->i2c_bus[0].scl = BUILD_PIN(P9, 17);

    b->i2c_bus[1].bus_id = 2;
    b->i2c_bus[1].sda = BUILD_PIN(P9, 20);
    b->i2c_bus[1].scl = BUILD_PIN(P9, 19);

    if (emmc_enabled == 1)
        b->spi_bus_count = 1;
    else
        b->spi_bus_count = 2;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 1;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = BUILD_PIN(P9, 17);
    b->spi_bus[0].mosi = BUILD_PIN(P9, 18);
    b->spi_bus[0].miso = BUILD_PIN(P9, 21);
    b->spi_bus[0].sclk = BUILD_PIN(P9, 22);

    b->spi_bus[1].bus_id = 2;
    b->spi_bus[1].slave_s = 0;
    b->spi_bus[1].cs = BUILD_PIN(P9, 28);
    b->spi_bus[1].mosi = BUILD_PIN(P9, 29);
    b->spi_bus[1].miso = BUILD_PIN(P9, 30);
    b->spi_bus[1].sclk = BUILD_PIN(P9, 31);

    b->uart_dev_count = 5;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = BUILD_PIN(P9, 26);
    b->uart_dev[0].tx = BUILD_PIN(P9, 24);
    b->uart_dev[0].device_path = "/dev/ttyO1";

    b->uart_dev[1].rx = BUILD_PIN(P9, 22);
    b->uart_dev[1].tx = BUILD_PIN(P9, 21);
    b->uart_dev[1].device_path = "/dev/ttyO2";

    // TODO
    b->uart_dev[2].rx = 0;
    b->uart_dev[2].tx = 42;
    b->uart_dev[2].device_path = "/dev/ttyO3";

    b->uart_dev[3].rx = BUILD_PIN(P9, 11);
    b->uart_dev[3].tx = BUILD_PIN(P9, 13);
    b->uart_dev[3].device_path = "/dev/ttyO4";

    b->uart_dev[4].rx = BUILD_PIN(P8, 38);
    b->uart_dev[4].tx = BUILD_PIN(P8, 37);
    b->uart_dev[4].device_path = "/dev/ttyO5";

    b->aio_non_seq = 1;
    b->aio_dev[0].pin = BUILD_PIN(P9, 39);
    b->aio_dev[1].pin = BUILD_PIN(P9, 40);
    b->aio_dev[2].pin = BUILD_PIN(P9, 37);
    b->aio_dev[3].pin = BUILD_PIN(P9, 38);
    b->aio_dev[4].pin = BUILD_PIN(P9, 33);
    b->aio_dev[5].pin = BUILD_PIN(P9, 36);
    b->aio_dev[6].pin = BUILD_PIN(P9, 35);
    b->gpio_count = 81;

    return b;

error:
    syslog(LOG_CRIT, "Beaglebone: failed to initialize");
    free(b);
    return NULL;
};
