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
#include "arm/phyboard.h"
#include "arm/am335x.h"

#define PLATFORM_NAME_PHYBOARD_WEGA "phyBOARD-Wega"

mraa_result_t
mraa_phyboard_uart_init_pre(int index)
{
    char devpath[MAX_SIZE];

    snprintf(devpath, MAX_SIZE, "/dev/ttyO%u", index);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_ERR, "uart: Device not initialized");
    } else {
        plat->uart_dev[index].device_path = devpath;
        return MRAA_SUCCESS;
    }

    return MRAA_ERROR_INVALID_PARAMETER;
}

mraa_result_t
mraa_phyboard_spi_init_pre(int index)
{
    mraa_result_t ret = MRAA_ERROR_INVALID_PARAMETER;
    char devpath[MAX_SIZE];
    int deviceindex = 0;

    if ((index == 0) && mraa_link_targets("/sys/class/spidev/spidev1.0", "48030000")) {
        deviceindex = 1;
    }
    if (deviceindex == 0) {
        deviceindex = 1;
    }

    snprintf(devpath, MAX_SIZE, "/dev/spidev%u.0", deviceindex);
    if (mraa_file_exist(devpath)) {
        plat->spi_bus[index].bus_id = deviceindex;
        ret = MRAA_SUCCESS;
    } else {
        syslog(LOG_NOTICE, "spi: Device not initialized");
    }
    return ret;
}

/* NOT DONE / TESTED YET */
mraa_result_t
mraa_phyboard_i2c_init_pre(unsigned int bus)
{
    mraa_result_t ret = MRAA_ERROR_INVALID_PARAMETER;
    char devpath[MAX_SIZE];

    snprintf(devpath, MAX_SIZE, "/dev/i2c-%u", plat->i2c_bus[bus].bus_id);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_ERR, "i2c: %s doesn't exist ", devpath);
        syslog(LOG_ERR, "i2c: Device not initialized");

        return ret;
    }
    return MRAA_SUCCESS;
}

/* NOT DONE / TESTED YET */
mraa_pwm_context
mraa_phyboard_pwm_init_replace(int pin)
{
    char devpath[MAX_SIZE];

    if (plat == NULL) {
        syslog(LOG_ERR, "pwm: Platform Not Initialised");
        return NULL;
    }
    if (plat->pins[pin].capabilities.pwm != 1) {
        syslog(LOG_ERR, "pwm: pin not capable of pwm");
        return NULL;
    }
    if (!mraa_file_exist(SYSFS_CLASS_PWM "pwmchip0")) {
        syslog(LOG_ERR, "pwm: pwmchip0 not found");
        return NULL;
    }

    snprintf(devpath, MAX_SIZE, SYSFS_CLASS_PWM "pwm%u", plat->pins[pin].pwm.pinmap);
    if (!mraa_file_exist(devpath)) {
        FILE* fh;
        fh = fopen(SYSFS_CLASS_PWM "export", "w");
        if (fh == NULL) {
            syslog(LOG_ERR, "pwm: Failed to open %s for writing, check access "
                            "rights for user", SYSFS_CLASS_PWM "export");
            return NULL;
        }
        if (fprintf(fh, "%d", plat->pins[pin].pwm.pinmap) < 0) {
            syslog(LOG_ERR, "pwm: Failed to write to %s", SYSFS_CLASS_PWM "export");
        }
        fclose(fh);
    }

    if (mraa_file_exist(devpath)) {
        mraa_pwm_context dev = (mraa_pwm_context) calloc(1, sizeof(struct _pwm));
        if (dev == NULL) {
            return NULL;
        }
        dev->duty_fp = -1;
        dev->chipid = -1;
        dev->pin = plat->pins[pin].pwm.pinmap;
        dev->period = -1;
        return dev;
    } else {
        syslog(LOG_ERR, "pwm: pin not initialized");
    }
    return NULL;
}

mraa_board_t*
mraa_phyboard()
{
    unsigned int uart2_enabled = 0;
    unsigned int uart3_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttyO2")) {
        uart2_enabled = 1;
    } else {
        uart2_enabled = 0;
    }

    if (mraa_file_exist("/sys/class/tty/ttyO3")) {
        uart3_enabled = 1;
    } else {
        uart3_enabled = 0;
    }

    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }
    b->platform_name = PLATFORM_NAME_PHYBOARD_WEGA;
    b->phy_pin_count = MRAA_PHYBOARD_WEGA_PINCOUNT;

    if (b->platform_name == NULL) {
        goto error;
    }

    b->aio_count = 4;
    b->adc_raw = 12;
    b->adc_supported = 12;

    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) calloc(b->phy_pin_count,sizeof(mraa_pininfo_t));
    if (b->pins == NULL) {
        goto error;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b->pins);
        goto error;
    }

    b->adv_func->uart_init_pre = &mraa_phyboard_uart_init_pre;
    b->adv_func->spi_init_pre = &mraa_phyboard_spi_init_pre;
    b->adv_func->i2c_init_pre = &mraa_phyboard_i2c_init_pre;
    b->adv_func->pwm_init_replace = &mraa_phyboard_pwm_init_replace;

    strncpy(b->pins[0].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    // X69 connector
    strncpy(b->pins[1].name, "VCC3V3", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[2].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[3].name, "VDIG1_1P8V", MRAA_PIN_NAME_SIZE);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[4].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[5].name, "X_SPIO_CS0", MRAA_PIN_NAME_SIZE);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[5].spi.mux_total = 0;

    strncpy(b->pins[6].name, "X_SPIO_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[6].spi.mux_total = 0;

    strncpy(b->pins[7].name, "X_SPIO_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[7].spi.mux_total = 0;

    strncpy(b->pins[8].name, "X_SPIO_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[8].spi.mux_total = 0;

    strncpy(b->pins[9].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[10].name, "X_UART0_RXD", MRAA_PIN_NAME_SIZE);
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[10].uart.mux_total = 0;

    strncpy(b->pins[11].name, "X_I2C0_SDA", MRAA_PIN_NAME_SIZE);
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[11].i2c.mux_total = 0;

    strncpy(b->pins[12].name, "X_UART0_TXD", MRAA_PIN_NAME_SIZE);
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[12].uart.mux_total = 0;

    strncpy(b->pins[13].name, "X_I2C0_SCL", MRAA_PIN_NAME_SIZE);
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[13].i2c.mux_total = 0;

    strncpy(b->pins[14].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[15].name, "X_JTAG_TMS", MRAA_PIN_NAME_SIZE);
    b->pins[15].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "X_nJTAG_TRST" to "X_JTAG_TRST" for fitting in b->pins[16].name*/
    strncpy(b->pins[16].name, "X_JTAG_TRST", MRAA_PIN_NAME_SIZE);
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[17].name, "X_JTAG_TDI", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[18].name, "X_JTAG_TDO", MRAA_PIN_NAME_SIZE);
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[19].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[20].name, "X_JTAG_TCK", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "X_USB_DP_EXP" to "X_USB_DP" for fitting in b->pins[21].name*/
    strncpy(b->pins[21].name, "X_USB_DP", MRAA_PIN_NAME_SIZE);
    b->pins[21].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "X_USB_DM_EXP" to "X_USB_DM" for fitting in b->pins[22].name*/
    strncpy(b->pins[22].name, "X_USB_DM", MRAA_PIN_NAME_SIZE);
    b->pins[22].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[23].name, "nRESET_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[24].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[24].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[25].name, "X_MMC2_CMD", MRAA_PIN_NAME_SIZE);
    b->pins[25].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[26].name, "X_MMC2_DATO", MRAA_PIN_NAME_SIZE);
    b->pins[26].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[27].name, "X_MMC2_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[28].name, "X_MMC2_DAT1", MRAA_PIN_NAME_SIZE);
    b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[29].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[30].name, "X_MMC2_DAT2", MRAA_PIN_NAME_SIZE);
    b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* PIN-Number "31" with name "X_UART2_RX_GPIO3_9" */
    if (uart2_enabled == 1) {
        strncpy(b->pins[31].name, "X_UART2_RX", MRAA_PIN_NAME_SIZE);
        b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[31].name, "X_GPIO3_9", MRAA_PIN_NAME_SIZE);
        b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[31].gpio.pinmap = (3*32 + 9);
    b->pins[31].gpio.parent_id = 0;
    b->pins[31].gpio.mux_total = 0;
    b->pins[31].uart.mux_total = 0;

    strncpy(b->pins[32].name, "X_MMC2_DAT3", MRAA_PIN_NAME_SIZE);
    b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* PIN-Number "33" with name "X_UART2_TX_GPIO3_10" */
    if (uart2_enabled == 1) {
        strncpy(b->pins[33].name, "X_UART2_TX", MRAA_PIN_NAME_SIZE);
        b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[33].name, "X_GPIO3_10", MRAA_PIN_NAME_SIZE);
        b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[33].gpio.pinmap = (3*32 + 10);
    b->pins[33].gpio.parent_id = 0;
    b->pins[33].gpio.mux_total = 0;
    b->pins[33].uart.mux_total = 0;

    strncpy(b->pins[34].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* PIN-Number "35" with name "X_UART3_RX_GPIO2_18" */
    if (uart3_enabled == 1) {
        strncpy(b->pins[35].name, "X_UART3_RX", MRAA_PIN_NAME_SIZE);
        b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[35].name, "X_GPIO2_18", MRAA_PIN_NAME_SIZE);
        b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[35].gpio.pinmap = (2*32 + 18);
    b->pins[35].gpio.parent_id = 0;
    b->pins[35].gpio.mux_total = 0;
    b->pins[35].uart.mux_total = 0;

    /*  PIN-Number "36" with name "X_UART3_TX_GPIO2_19" */
    if (uart3_enabled == 1) {
        strncpy(b->pins[36].name, "X_UART3_TX", MRAA_PIN_NAME_SIZE);
        b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    } else {
        strncpy(b->pins[36].name, "X_GPIO2_19", MRAA_PIN_NAME_SIZE);
        b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    }
    b->pins[36].gpio.pinmap = (2*32 + 19);
    b->pins[36].gpio.parent_id = 0;
    b->pins[36].gpio.mux_total = 0;
    b->pins[36].uart.mux_total = 0;

    /* PIN-Number "37" with name "X_INTR1_GPIO0_20" */
    strncpy(b->pins[37].name, "X_GPIO0_20", MRAA_PIN_NAME_SIZE);
    b->pins[37].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[37].gpio.pinmap = (0*32 + 20);
    b->pins[37].gpio.parent_id = 0;
    b->pins[37].gpio.mux_total = 0;

    strncpy(b->pins[38].name, "X_GPIO0_7", MRAA_PIN_NAME_SIZE);
    b->pins[38].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[38].gpio.pinmap = (0*32 + 7);
    b->pins[38].gpio.parent_id = 0;
    b->pins[38].gpio.mux_total = 0;

    /* changed name "X_AM335_EXT_WAKEUP" to "X_AM335_WUP" for fitting in b->pins[39].name*/
    strncpy(b->pins[39].name, "X_AM335_WUP", MRAA_PIN_NAME_SIZE);
    b->pins[39].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[40].name, "X_INT_RTCn", MRAA_PIN_NAME_SIZE);
    b->pins[40].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[41].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[41].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* PIN-Number "42" with name "X_GPIO3_7_nPMOD_PW RFAIL" */
    strncpy(b->pins[42].name, "X_GPIO3_7", MRAA_PIN_NAME_SIZE);
    b->pins[42].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[42].gpio.pinmap = (3*32 + 7);
    b->pins[42].gpio.parent_id = 0;
    b->pins[42].gpio.mux_total = 0;

    strncpy(b->pins[43].name, "nRESET_IN", MRAA_PIN_NAME_SIZE);
    b->pins[43].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[44].name, "X_GPIO1_31", MRAA_PIN_NAME_SIZE);
    b->pins[44].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[44].gpio.pinmap = (1*32 + 31);
    b->pins[44].gpio.parent_id = 0;
    b->pins[44].gpio.mux_total = 0;

    /* changed name "X_AM335_NMIn" to "X_AM335_NMI" for fitting in b->pins[45].name*/
    strncpy(b->pins[45].name, "X_AM335_NMI", MRAA_PIN_NAME_SIZE);
    b->pins[45].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[46].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[46].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // NOT DONE / TESTED YET
    strncpy(b->pins[47].name, "X_AIN4", MRAA_PIN_NAME_SIZE);
    b->pins[47].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };

    // NOT DONE / TESTED YET
    strncpy(b->pins[48].name, "X_AIN5", MRAA_PIN_NAME_SIZE);
    b->pins[48].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };

    // NOT DONE / TESTED YET
    strncpy(b->pins[49].name, "X_AIN6", MRAA_PIN_NAME_SIZE);
    b->pins[49].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };

    // NOT DONE / TESTED YET
    strncpy(b->pins[50].name, "X_AIN7", MRAA_PIN_NAME_SIZE);
    b->pins[50].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 1, 0 };

    strncpy(b->pins[51].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[51].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* to CHECK X_GPIO_CKSYNC */
    /* changed name "X_GPIO_CKSYNC" to "X_GPIO_CSYN" for fitting in b->pins[52].name*/
    strncpy(b->pins[52].name, "X_GPIO_CSYN", MRAA_PIN_NAME_SIZE);
    b->pins[52].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "X_USB_ID_EXP" to "X_USB_ID" for fitting in b->pins[53].name*/
    strncpy(b->pins[53].name, "X_USB_ID", MRAA_PIN_NAME_SIZE);
    b->pins[53].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "USB_VBUS_EXP" to "USB_VBUS" for fitting in b->pins[54].name*/
    strncpy(b->pins[54].name, "USB_VBUS", MRAA_PIN_NAME_SIZE);
    b->pins[54].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[55].name, "X_USB1_CE", MRAA_PIN_NAME_SIZE);
    b->pins[55].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[56].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[56].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[57].name, "VCC_BL", MRAA_PIN_NAME_SIZE);
    b->pins[57].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[58].name, "X_BP_POWER", MRAA_PIN_NAME_SIZE);
    b->pins[58].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[59].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[59].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60].name, "VCC5V_IN", MRAA_PIN_NAME_SIZE);
    b->pins[60].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // X70 connector
    strncpy(b->pins[60+1].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+2].name, "X_LCD_D21", MRAA_PIN_NAME_SIZE);
    b->pins[60+2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+3].name, "X_LCD_D18", MRAA_PIN_NAME_SIZE);
    b->pins[60+3].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+4].name, "X_LCD_D16", MRAA_PIN_NAME_SIZE);
    b->pins[60+4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+5].name, "X_LCD_D0", MRAA_PIN_NAME_SIZE);
    b->pins[60+5].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+6].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+7].name, "X_LCD_D1", MRAA_PIN_NAME_SIZE);
    b->pins[60+7].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+8].name, "X_LCD_D2", MRAA_PIN_NAME_SIZE);
    b->pins[60+8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+9].name, "X_LCD_D3", MRAA_PIN_NAME_SIZE);
    b->pins[60+9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+10].name, "X_LCD_D4", MRAA_PIN_NAME_SIZE);
    b->pins[60+10].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+11].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+11].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+12].name, "X_LCD_D22", MRAA_PIN_NAME_SIZE);
    b->pins[60+12].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+13].name, "X_LCD_D19", MRAA_PIN_NAME_SIZE);
    b->pins[60+13].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+14].name, "X_LCD_D5", MRAA_PIN_NAME_SIZE);
    b->pins[60+14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+15].name, "X_LCD_D6", MRAA_PIN_NAME_SIZE);
    b->pins[60+15].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+16].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+16].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+17].name, "X_LCD_D7", MRAA_PIN_NAME_SIZE);
    b->pins[60+17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+18].name, "X_LCD_D8", MRAA_PIN_NAME_SIZE);
    b->pins[60+18].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+19].name, "X_LCD_D9", MRAA_PIN_NAME_SIZE);
    b->pins[60+19].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+20].name, "X_LCD_D10", MRAA_PIN_NAME_SIZE);
    b->pins[60+20].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+21].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+21].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+22].name, "X_LCD_D23", MRAA_PIN_NAME_SIZE);
    b->pins[60+22].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+23].name, "X_LCD_D20", MRAA_PIN_NAME_SIZE);
    b->pins[60+23].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+24].name, "X_LCD_D17", MRAA_PIN_NAME_SIZE);
    b->pins[60+24].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+25].name, "X_LCD_D11", MRAA_PIN_NAME_SIZE);
    b->pins[60+25].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+26].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+26].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+27].name, "X_LCD_D12", MRAA_PIN_NAME_SIZE);
    b->pins[60+27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+28].name, "X_LCD_D13", MRAA_PIN_NAME_SIZE);
    b->pins[60+28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+29].name, "X_LCD_D14", MRAA_PIN_NAME_SIZE);
    b->pins[60+29].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+30].name, "X_LCD_D15", MRAA_PIN_NAME_SIZE);
    b->pins[60+30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+31].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+31].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+32].name, "X_LCD_PCLK", MRAA_PIN_NAME_SIZE);
    b->pins[60+32].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* changed name "X_LCD_BIAS_EN" to "X_LCD_BIAS" for fitting in b->pins[60+33].name*/
    strncpy(b->pins[60+33].name, "X_LCD_BIAS", MRAA_PIN_NAME_SIZE);
    b->pins[60+33].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+34].name, "X_LCD_HSYNC", MRAA_PIN_NAME_SIZE);
    b->pins[60+34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+35].name, "X_LCD_VSYNC", MRAA_PIN_NAME_SIZE);
    b->pins[60+35].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+36].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+36].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+37].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+37].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+38].name, "X_PWM1_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[60+38].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    b->pins[60+38].pwm.pinmap = 0;
    b->pins[60+38].pwm.mux_total = 0;

    strncpy(b->pins[60+39].name, "VCC_BL", MRAA_PIN_NAME_SIZE);
    b->pins[60+39].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[60+40].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    // X71 connector
    strncpy(b->pins[60+40+1].name, "X_I2S_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+2].name, "X_I2S_FRM", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+3].name, "X_I2S_ADC", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+3].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+4].name, "X_I2S_DAC", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    /* PIN-Number "60+40+5" with name "X_AV_INT_GPIO1_30" */
    strncpy(b->pins[60+40+5].name, "X_GPIO1_30", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+5].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[60+40+5].gpio.pinmap = (1*32 + 30);
    b->pins[60+40+5].gpio.parent_id = 0;
    b->pins[60+40+5].gpio.mux_total = 0;

    /* PIN-Number "60+40+6" with name "nUSB1_OC_GPIO3_19" or "X_MCASP0_AHCLKX_GPIO3_21" */
    // TODO -> Check settings of Jumper J77
    strncpy(b->pins[60+40+6].name, "X_GPIO3_19", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+6].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[60+40+6].gpio.pinmap = (3*32 + 19);
    b->pins[60+40+6].gpio.parent_id = 0;
    b->pins[60+40+6].gpio.mux_total = 0;

    strncpy(b->pins[60+40+7].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+7].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+8].name, "nRESET_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+9].name, "TS_X+", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+10].name, "TS_X-", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+10].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+11].name, "TS_Y+", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+11].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+12].name, "TS_Y-", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+12].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+13].name, "VCC3V3", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+13].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+14].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[60+40+15].name, "X_I2C0_SCL", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+15].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[60+40+15].i2c.mux_total = 0;

    strncpy(b->pins[60+40+16].name, "X_I2C0_SDA", MRAA_PIN_NAME_SIZE);
    b->pins[60+40+16].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[60+40+16].i2c.mux_total = 0;

    // BUS DEFINITIONS
    b->i2c_bus_count = 2;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 11;
    b->i2c_bus[0].scl = 13;
    b->i2c_bus[1].bus_id = 0;
    b->i2c_bus[1].sda = (60+40+16);
    b->i2c_bus[1].scl = (60+40+15);

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 5;
    b->spi_bus[0].mosi = 6;
    b->spi_bus[0].miso = 7;
    b->spi_bus[0].sclk = 8;

    b->uart_dev_count = 4;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 10;
    b->uart_dev[0].tx = 12;
    /*  TODO UART1  */
    b->uart_dev[1].rx = 0;
    b->uart_dev[1].tx = 0;
    /*  TODO UART1  */
    b->uart_dev[2].rx = 31;
    b->uart_dev[2].tx = 33;
    b->uart_dev[3].rx = 35;
    b->uart_dev[3].tx = 36;

    b->gpio_count = 0;
    int i;
    for (i = 0; i < b->phy_pin_count; i++) {
        if (b->pins[i].capabilities.gpio) {
            b->gpio_count++;
        }
    }
    return b;
error:
    syslog(LOG_CRIT, "phyboard: failed to initialize");
    free(b);

    return NULL;
};
