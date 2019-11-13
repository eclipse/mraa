#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <mraa/types.h>

#include "common.h"
#include "arm/phyboard.h"
#include "arm/phyboard_pico.h"
#include "arm/am335x.h"

#define PLATFORM_NAME_PHYBOARD_PICO "phyBOARD-pico"

mraa_result_t
mraa_phyboard_segin_uart_init_pre(int index)
{
    char devpath[MAX_SIZE];
    snprintf(devpath, MAX_SIZE, "/dev/ttymxc%u", index);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_ERR, "uart: Device not initialized");
    } else {
        plat->uart_dev[index].device_path = devpath;
        return MRAA_SUCCESS;
    }

    return MRAA_ERROR_INVALID_PARAMETER;
}

/* NOT DONE / TESTED YET */
mraa_result_t
mraa_phyboard_segin_spi_init_pre(int index)
{
    mraa_result_t ret = MRAA_ERROR_NO_RESOURCES;
    char devpath[MAX_SIZE];
    int deviceindex = 0;

    if ((index == 0) && mraa_link_targets("/sys/class/spidev/spidev1.0", "48030000"))
        deviceindex = 1;
    if (deviceindex == 0)
        deviceindex = 1;

    sprintf(devpath, "/dev/spidev%u.0", deviceindex);
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
mraa_phyboard_segin_i2c_init_pre(unsigned int bus)
{
    mraa_result_t ret = MRAA_ERROR_NO_RESOURCES;
    char devpath[MAX_SIZE];

    sprintf(devpath, "/dev/i2c-%u", plat->i2c_bus[bus].bus_id);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_INFO, "i2c: %s doesn't exist ", devpath);
        syslog(LOG_ERR, "i2c: Device not initialized");

        return ret;
    }
    return MRAA_SUCCESS;
}

/* NOT DONE / TESTED YET */
mraa_pwm_context
mraa_phyboard_segin_pwm_init_replace(int pin)
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

    sprintf(devpath, SYSFS_CLASS_PWM "pwm%u", plat->pins[pin].pwm.pinmap);
    if (!mraa_file_exist(devpath)) {
        FILE* fh;
        fh = fopen(SYSFS_CLASS_PWM "export", "w");
        if (fh == NULL) {
            syslog(LOG_ERR, "pwm: Failed to open /sys/class/pwm/export for writing, check access "
                            "rights for user");
            return NULL;
        }
        if (fprintf(fh, "%d", plat->pins[pin].pwm.pinmap) < 0) {
            syslog(LOG_ERR, "pwm: Failed to write to sysfs-pwm-export");
        }
        fclose(fh);
    }

    if (mraa_file_exist(devpath)) {
        mraa_pwm_context dev = (mraa_pwm_context) calloc(1, sizeof(struct _pwm));
        if (dev == NULL)
            return NULL;
        dev->duty_fp = -1;
        dev->chipid = -1;
        dev->pin = plat->pins[pin].pwm.pinmap;
        dev->period = -1;
        return dev;
    } else
        syslog(LOG_ERR, "pwm: pin not initialized");
    return NULL;
}


mraa_board_t*
mraa_phyboard_pico()
{
    unsigned int uart2_enabled = 0;
    unsigned int uart3_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttymxc1"))
        uart2_enabled = 1;
    else
        uart2_enabled = 0;

    if (mraa_file_exist("/sys/class/tty/ttymxc2"))
        uart3_enabled = 1;
    else
        uart3_enabled = 0;

    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL)
        return NULL;
    b->platform_name = PLATFORM_NAME_PHYBOARD_PICO;
    b->phy_pin_count = MRAA_PHYBOARD_PICO_PINCOUNT;

    if (b->platform_name == NULL) {
        goto error;
    }

    b->aio_count = 4;
    b->adc_raw = 8;
    b->adc_supported = 8;

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

    b->adv_func->uart_init_pre = &mraa_phyboard_segin_uart_init_pre;
    b->adv_func->spi_init_pre = &mraa_phyboard_segin_spi_init_pre;
    b->adv_func->i2c_init_pre = &mraa_phyboard_segin_i2c_init_pre;
    b->adv_func->pwm_init_replace = &mraa_phyboard_segin_pwm_init_replace;

    strncpy(b->pins[0].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    // X69 connector
    strncpy(b->pins[1].name, "VCC3V3", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[2].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[3].name, "VCC3V3", MRAA_PIN_NAME_SIZE);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[4].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[5].name, "X_JTAG_TDI", MRAA_PIN_NAME_SIZE);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[6].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[7].name, "X_JTAG_TDO", MRAA_PIN_NAME_SIZE);
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
  
    strncpy(b->pins[8].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[9].name, "X_nJTAG_TRST", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

   // strncpy(b->pins[10].name, "nRESET_IN", MRAA_PIN_NAME_SIZE);
   // b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[11].name, "X_JTAG_TCK", MRAA_PIN_NAME_SIZE);
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[12].name, "nRESET_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[13].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[14].name, "X_SPI2_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[14].spi.mux_total = 0;

    strncpy(b->pins[16].name, "X_SPI2_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[16].spi.mux_total = 0;

    strncpy(b->pins[17].name, "X_USB_DM_EXP", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[18].name, "X_SPI2_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[18].spi.mux_total = 0;

    strncpy(b->pins[19].name, "X_USB_DP_EXP", MRAA_PIN_NAME_SIZE);
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[20].name, "X_PWM3_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilities = (mraa_pincapabilities_t){ 1, 0, 1, 0, 0, 0, 0, 0 };
    b->pins[20].pwm.pinmap = 0;
    b->pins[20].pwm.mux_total = 0;


    strncpy(b->pins[21].name, "X_USB_ID_EXP", MRAA_PIN_NAME_SIZE);
    b->pins[21].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[22].name, "X_SPI1_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[22].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[22].spi.mux_total = 0;

    strncpy(b->pins[23].name, "X_UART3_TX", MRAA_PIN_NAME_SIZE);
    b->pins[23].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };

    strncpy(b->pins[24].name, "X_SPI1_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[24].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[24].spi.mux_total = 0;

    strncpy(b->pins[25].name, "X_UART3_RX", MRAA_PIN_NAME_SIZE);
    b->pins[25].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };

    strncpy(b->pins[26].name, "X_SPI1_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[26].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[26].spi.mux_total = 0;

    /* PIN-Number "4" with name "X_GPIO1_IO01__ADC1_IN" */
    strncpy(b->pins[27].name, "X_ADC1_IN1", MRAA_PIN_NAME_SIZE);
    //b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 1, 0, 0, 0, 0 };
    b->pins[27].capabilities.aio = 1;
    b->pins[27].aio.pinmap = 1;
    b->pins[27].aio.mux_total = 0;

    strncpy(b->pins[28].name, "X_I2C1_SCL", MRAA_PIN_NAME_SIZE);
    b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[28].i2c.mux_total = 0;

/*    strncpy(b->pins[29].name, "X_GPIO1_IO1", MRAA_PIN_NAME_SIZE);
    b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[29].gpio.pinmap = ((1-1)*32 + 4);
    b->pins[29].gpio.parent_id = 0;
    b->pins[29].gpio.mux_total = 0;
    b->pins[29].uart.mux_total = 0; */
    
    strncpy(b->pins[30].name, "X_I2C1_SDA", MRAA_PIN_NAME_SIZE);
    b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[30].i2c.mux_total = 0;

    strncpy(b->pins[31].name, "X_JTAG_TMS", MRAA_PIN_NAME_SIZE);
    b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[32].name, "X_I2C2_SCL", MRAA_PIN_NAME_SIZE);
    b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[32].i2c.mux_total = 0;

    strncpy(b->pins[33].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[34].name, "X_I2C2_SDA", MRAA_PIN_NAME_SIZE);
    b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[34].i2c.mux_total = 0;

    strncpy(b->pins[35].name, "X_JTAG_MOD", MRAA_PIN_NAME_SIZE);
    b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[36].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[37].name, "X_GPIO1_IO3", MRAA_PIN_NAME_SIZE);
    b->pins[37].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[37].gpio.pinmap = ((1-1)*32 + 3);
    b->pins[37].gpio.parent_id = 0;
    b->pins[37].gpio.mux_total = 0;
    b->pins[37].uart.mux_total = 0;

    strncpy(b->pins[38].name, "nRESET_OUT", MRAA_PIN_NAME_SIZE);
    b->pins[38].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
   
    strncpy(b->pins[39].name, "X_GPIO4_IO28", MRAA_PIN_NAME_SIZE);
    b->pins[39].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[39].gpio.pinmap = ((4-1)*32 + 28);
    b->pins[39].gpio.parent_id = 0;
    b->pins[39].gpio.mux_total = 0;
    b->pins[39].uart.mux_total = 0;

    strncpy(b->pins[40].name, "X_SPI1_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[40].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[40].spi.mux_total = 0;

    strncpy(b->pins[41].name, "X_SPI1_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[41].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[41].spi.mux_total = 0;

    strncpy(b->pins[42].name, "X_SPI1_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[42].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 1, 0, 0, 0 };
    b->pins[42].spi.mux_total = 0;

    strncpy(b->pins[43].name, "VCC3V3", MRAA_PIN_NAME_SIZE);
    b->pins[43].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[44].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[44].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[45].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[45].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
  
    strncpy(b->pins[46].name, "VCC5V", MRAA_PIN_NAME_SIZE);
    b->pins[46].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[47].name, "X_I2C0_SDA", MRAA_PIN_NAME_SIZE);
    b->pins[47].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[47].i2c.mux_total = 0;

    strncpy(b->pins[48].name, "X_I2C0_SCL", MRAA_PIN_NAME_SIZE);
    b->pins[48].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 1, 0, 0 };
    b->pins[48].i2c.mux_total = 0;


    strncpy(b->pins[49].name, "X_GPIO2_IO11", MRAA_PIN_NAME_SIZE);
    b->pins[49].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[49].gpio.pinmap = ((2-1)*32 + 11);
    b->pins[49].gpio.parent_id = 0;
    b->pins[49].gpio.mux_total = 0;


/* P4-Serial Port RS232 X_UART2_CTS_B= tx X_UART2_TX=rx   */
    strncpy(b->pins[50].name, "X_UART2_TX", MRAA_PIN_NAME_SIZE);
    b->pins[50].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[50].uart.mux_total = 0;

    strncpy(b->pins[51].name, "X_UART2_CTS_B", MRAA_PIN_NAME_SIZE);
    b->pins[51].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 1 };
    b->pins[51].uart.mux_total = 0;

    /*User  button*/
    strncpy(b->pins[52].name, "X_GPIO2_IO8_INT_SW", MRAA_PIN_NAME_SIZE);
    b->pins[52].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[52].gpio.pinmap = ((2-1)*32 + 8);
    b->pins[52].gpio.parent_id = 0;
    b->pins[52].gpio.mux_total = 0;
    
    strncpy(b->pins[53].name, "X_GPIO2_IO12", MRAA_PIN_NAME_SIZE);
    b->pins[53].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[53].gpio.pinmap = ((2-1)*32 + 12);
    b->pins[53].gpio.parent_id = 0;
    b->pins[53].gpio.mux_total = 0;
 
    strncpy(b->pins[54].name, "X_GPIO2_IO13", MRAA_PIN_NAME_SIZE);
    b->pins[54].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[54].gpio.pinmap = ((2-1)*32 + 13);
    b->pins[54].gpio.parent_id = 0;
    b->pins[54].gpio.mux_total = 0;	

 strncpy(b->pins[10].name, "X_GPIO1_IO10", MRAA_PIN_NAME_SIZE);
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[10].gpio.pinmap = ((1-1)*32 + 10);
    b->pins[10].gpio.parent_id = 0;
    b->pins[10].gpio.mux_total = 0;



    // BUS DEFINITIONS
    b->i2c_bus_count=1;
	b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[0].sda = 30;
    b->i2c_bus[0].scl = 28;

    b->spi_bus_count = 3;
    b->def_spi_bus = 0;
    b->spi_bus[1].bus_id = 1;
    b->spi_bus[1].slave_s = 0;
    b->spi_bus[1].cs = 39;
    b->spi_bus[1].mosi = 42;
    b->spi_bus[1].miso = 41;
    b->spi_bus[1].sclk = 40;

    b->uart_dev_count = 4;
    b->def_uart_dev = 3;
    b->uart_dev[3].rx = 25;
    b->uart_dev[3].tx = 23;
//    b->uart_dev[3].device_path = "/dev/ttymxc2";
    b->uart_dev[2].rx = 50;
    b->uart_dev[2].tx = 51;
    b->uart_dev[2].device_path = "/dev/ttymxc1";

    b->gpio_count = 0;
    int i;
    for (i = 0; i < b->phy_pin_count; i++)
        if (b->pins[i].capabilities.gpio)
            b->gpio_count++;
    return b;
error:
    syslog(LOG_CRIT, "phyboard_pico: failed to initialize");
    free(b);

    return NULL;
};
