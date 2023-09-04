#include <mraa/common.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/radxa_rock_5b.h"
#include "common.h"

#define DT_BASE "/proc/device-tree"

const char* radxa_rock_5b_serialdev[MRAA_RADXA_ROCK_5B_UART_COUNT] = { "/dev/ttyS2", "/dev/ttyS3", "/dev/ttyS4", "/dev/ttyS7"};

void
mraa_radxa_rock_5b_pininfo(mraa_board_t* board, int index, int gpio_chip, int gpio_line, mraa_pincapabilities_t pincapabilities_t, char* pin_name)
{
    if (index > board->phy_pin_count)
        return;

    mraa_pininfo_t* pininfo = &board->pins[index];
    strncpy(pininfo->name, pin_name, MRAA_PIN_NAME_SIZE);

    if(pincapabilities_t.gpio == 1) {
        pininfo->gpio.gpio_chip = gpio_chip;
        pininfo->gpio.gpio_line = gpio_line;
    }

    pininfo->capabilities = pincapabilities_t;

    pininfo->gpio.mux_total = 0;
}

mraa_board_t*
mraa_radxa_rock_5b()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        free(b);
        return NULL;
    }

    // pin mux for buses are setup by default by kernel so tell mraa to ignore them
    b->no_bus_mux = 1;
    b->phy_pin_count = MRAA_RADXA_ROCK_5B_PIN_COUNT + 1;

    // UART
    b->uart_dev_count = MRAA_RADXA_ROCK_5B_UART_COUNT;
    b->platform_name = PLATFORM_NAME_RADXA_ROCK_5B;
    b->def_uart_dev = 0;
    b->uart_dev[0].index = 2;
    b->uart_dev[1].index = 3;
    b->uart_dev[2].index = 4;
    b->uart_dev[3].index = 7;
    b->uart_dev[0].device_path = (char*) radxa_rock_5b_serialdev[0];
    b->uart_dev[1].device_path = (char*) radxa_rock_5b_serialdev[1];
    b->uart_dev[2].device_path = (char*) radxa_rock_5b_serialdev[2];
    b->uart_dev[3].device_path = (char*) radxa_rock_5b_serialdev[3];
    
    // I2C
    b->i2c_bus_count = MRAA_RADXA_ROCK_5B_I2C_COUNT;
    b->def_i2c_bus = 0;
    b->i2c_bus[0].bus_id = 0;
    b->i2c_bus[1].bus_id = 1;
    b->i2c_bus[2].bus_id = 3;
    b->i2c_bus[3].bus_id = 8;

    // SPI
    b->spi_bus_count = MRAA_RADXA_ROCK_5B_SPI_COUNT;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[1].bus_id = 1;

    b->pwm_dev_count = MRAA_RADXA_ROCK_5B_PWM_COUNT;
    b->pwm_default_period = 500;
    b->pwm_max_period = 2147483;
    b->pwm_min_period = 1;

    b->pins = (mraa_pininfo_t*) malloc(sizeof(mraa_pininfo_t) * b->phy_pin_count);
    if (b->pins == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    b->pins[36].pwm.parent_id = 2;   // PWM2_M1
    b->pins[36].pwm.mux_total = 0;
    b->pins[36].pwm.pinmap = 0;
    b->pins[38].pwm.parent_id = 3;   // PWM3_M1
    b->pins[38].pwm.mux_total = 0;
    b->pins[38].pwm.pinmap = 0;
    b->pins[18].pwm.parent_id = 5;   // PWM5_M2
    b->pins[18].pwm.mux_total = 0;
    b->pins[18].pwm.pinmap = 0;
    b->pins[28].pwm.parent_id = 6;   // PWM6_M2
    b->pins[28].pwm.mux_total = 0;
    b->pins[28].pwm.pinmap = 0;
    b->pins[27].pwm.parent_id = 7;   // PWM7_M3
    b->pins[27].pwm.mux_total = 0;
    b->pins[27].pwm.pinmap = 0;
    b->pins[33].pwm.parent_id = 8;   // PWM8_M0
    b->pins[33].pwm.mux_total = 0;
    b->pins[33].pwm.pinmap = 0;
    b->pins[12].pwm.parent_id = 12;   // PWM12_M0
    b->pins[12].pwm.mux_total = 0;
    b->pins[12].pwm.pinmap = 0;
    b->pins[35].pwm.parent_id = 13;   // PWM13_M0
    b->pins[35].pwm.mux_total = 0;
    b->pins[35].pwm.pinmap = 0;
    b->pins[31].pwm.parent_id = 13;   // PWM13_M2
    b->pins[31].pwm.mux_total = 0;
    b->pins[31].pwm.pinmap = 0;
    b->pins[32].pwm.parent_id = 14;   // PWM14_M0
    b->pins[32].pwm.mux_total = 0;
    b->pins[32].pwm.pinmap = 0;
    b->pins[5].pwm.parent_id = 14;    // PWM14_M2
    b->pins[5].pwm.mux_total = 0;
    b->pins[5].pwm.pinmap = 0;
    b->pins[7].pwm.parent_id = 15;   // PWM15_M0
    b->pins[7].pwm.mux_total = 0;
    b->pins[7].pwm.pinmap = 0;
    b->pins[3].pwm.parent_id = 15;    // PWM15_M1
    b->pins[3].pwm.mux_total = 0;
    b->pins[3].pwm.pinmap = 0;
    b->pins[29].pwm.parent_id = 7;   // PWM15_M3
    b->pins[29].pwm.mux_total = 0;
    b->pins[29].pwm.pinmap = 0;

    b->aio_count = MRAA_RADXA_ROCK_5B_AIO_COUNT;
    b->adc_raw = 10;
    b->adc_supported = 10;
    b->aio_dev[0].pin = 22;
    b->aio_non_seq = 1;
    b->chardev_capable = 1;

    mraa_radxa_rock_5b_pininfo(b, 0, -1, -1, (mraa_pincapabilities_t){0,0,0,0,0,0,0,0}, "INVALID");
    mraa_radxa_rock_5b_pininfo(b, 1, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_5b_pininfo(b, 2, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_5b_pininfo(b, 3,  4, 11, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO4_B3");  // i2c7 channel is occupied by i2c7-m0 in kernel
    mraa_radxa_rock_5b_pininfo(b, 4, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "5V");
    mraa_radxa_rock_5b_pininfo(b, 5,  4, 10, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO4_B2");  // i2c7 channel is occupied by i2c7-m0 in kernel
    mraa_radxa_rock_5b_pininfo(b, 6, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 7,  3, 19, (mraa_pincapabilities_t){1,1,1,0,1,1,0,1}, "GPIO3_C3");
    mraa_radxa_rock_5b_pininfo(b, 8,  4, 13, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B5");  // GPIO0_B5 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_rock_5b_pininfo(b, 9, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 10, 0, 14, (mraa_pincapabilities_t){1,0,0,0,0,1,0,1}, "GPIO0_B6");  // GPIO0_B6 was used by fiq_debugger, function GPIO cannot be enabled
    mraa_radxa_rock_5b_pininfo(b, 11, 3, 17, (mraa_pincapabilities_t){1,1,1,0,1,0,0,1}, "GPIO3_C1");
    mraa_radxa_rock_5b_pininfo(b, 12, 3, 13, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B5");
    mraa_radxa_rock_5b_pininfo(b, 13, 3, 15, (mraa_pincapabilities_t){1,1,0,0,1,1,0,0}, "GPIO3_B7");
    mraa_radxa_rock_5b_pininfo(b, 14, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 15, 3, 16, (mraa_pincapabilities_t){1,0,0,0,1,1,0,1}, "GPIO3_C0");
    mraa_radxa_rock_5b_pininfo(b, 16, 3, 4, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO3_A4");
    mraa_radxa_rock_5b_pininfo(b, 17, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "3V3");
    mraa_radxa_rock_5b_pininfo(b, 18, 4, 20, (mraa_pincapabilities_t){1,1,1,0,1,0,0,0}, "GPIO4_C4");
    mraa_radxa_rock_5b_pininfo(b, 19, 1, 10, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B2");
    mraa_radxa_rock_5b_pininfo(b, 20, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 21, 1, 9, (mraa_pincapabilities_t){1,1,0,0,1,0,0,0}, "GPIO1_B1");
    mraa_radxa_rock_5b_pininfo(b, 22, -1, -1, (mraa_pincapabilities_t){1,0,0,0,1,0,1,0}, "SARADC_IN4");
    mraa_radxa_rock_5b_pininfo(b, 23, 1, 11, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B3");
    mraa_radxa_rock_5b_pininfo(b, 24, 1, 12, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B4");
    mraa_radxa_rock_5b_pininfo(b, 25, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 26, 1, 13, (mraa_pincapabilities_t){1,1,0,0,1,0,0,1}, "GPIO1_B5");
    mraa_radxa_rock_5b_pininfo(b, 27, 4, 22, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO4_C6");
    mraa_radxa_rock_5b_pininfo(b, 28, 4, 21, (mraa_pincapabilities_t){1,1,1,0,1,1,0,0}, "GPIO4_C5");
    mraa_radxa_rock_5b_pininfo(b, 29, 1, 31, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO1_D7");
    mraa_radxa_rock_5b_pininfo(b, 30, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 31, 1, 15, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO1_B7");
    mraa_radxa_rock_5b_pininfo(b, 32, 3, 18, (mraa_pincapabilities_t){1,1,1,0,1,1,0,1}, "GPIO3_C2");
    mraa_radxa_rock_5b_pininfo(b, 33, 3, 7, (mraa_pincapabilities_t){1,1,1,0,0,0,0,0}, "GPIO3_A7");
    mraa_radxa_rock_5b_pininfo(b, 34, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 35, 3, 14, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B6");
    mraa_radxa_rock_5b_pininfo(b, 36, 3, 9, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B1");
    mraa_radxa_rock_5b_pininfo(b, 37, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,1,0}, "NC");
    mraa_radxa_rock_5b_pininfo(b, 38, 3, 10, (mraa_pincapabilities_t){1,1,1,0,0,0,0,1}, "GPIO3_B2");
    mraa_radxa_rock_5b_pininfo(b, 39, -1, -1, (mraa_pincapabilities_t){1,0,0,0,0,0,0,0}, "GND");
    mraa_radxa_rock_5b_pininfo(b, 40, 3, 11, (mraa_pincapabilities_t){1,1,0,0,0,0,0,0}, "GPIO3_B3");

    return b;
}
