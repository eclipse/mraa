/*
 * Author: Daniel Bovensiepen <oss@bovi.li>
 * Author: Zhu Jia Xing <jiaxing.zhu@siemens.com>
 * Copyright (c) 2022 Siemens Ltd. China.
 *
 * SPDX-License-Identifier: MIT
 */

#include <dirent.h>
#include <mraa/common.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "riscv/visionfive.h"
#include "common.h"

#define MRAA_VISIONFIVE_V1_JH7100 1
#define MRAA_VISIONFIVE_V2_JH7110 2

#define PLATFORM_NAME_VISIONFIVE_V1 "VisionFive"
#define PLATFORM_NAME_VISIONFIVE_V2 "VisionFive-v2"

/* TODO: currently no TRM doc of JH7100 available, we use dummy values for now */
#define DEFAULT_PERIOD_US 40000
#define MAX_PERIOD_US     5000000
#define MIN_PERIOD_US     1

mraa_board_t*
mraa_visionfive()
{
    unsigned char detected_platform = MRAA_VISIONFIVE_V1_JH7100;
    int pinbase = 0;
    
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    if (b == NULL) {
        return NULL;
    }
 
    if (mraa_file_contains("/proc/device-tree/compatible", "visionfive-v2")) {
        detected_platform = MRAA_VISIONFIVE_V2_JH7110;
    } else if (mraa_file_contains("/proc/device-tree/compatible", "visionfive")) {
        detected_platform = MRAA_VISIONFIVE_V1_JH7100;
    }

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->platform_name = PLATFORM_NAME_VISIONFIVE_V1;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->platform_name = PLATFORM_NAME_VISIONFIVE_V2;
    }
    
    b->phy_pin_count = MRAA_VISIONFIVE_PINCOUNT;

    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->pwm_default_period = DEFAULT_PERIOD_US;
    b->pwm_max_period = MAX_PERIOD_US;
    b->pwm_min_period = MIN_PERIOD_US;

    if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        pinbase = 0;
    } else if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        pinbase = 448;
    }

    b->adv_func = (mraa_adv_func_t*) calloc(1, sizeof(mraa_adv_func_t));
    if (b->adv_func == NULL) {
        goto error;
    }
    
    b->pins = (mraa_pininfo_t*) calloc(MRAA_VISIONFIVE_PINCOUNT, sizeof(mraa_pininfo_t));
    if(b->pins == NULL) {
        free(b->adv_func);
        goto error;
    }
  
    strncpy(b->pins[0].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[1].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[2].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[3].name, "SDA0", MRAA_PIN_NAME_SIZE);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[3].gpio.pinmap = pinbase + 48;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[3].gpio.pinmap = pinbase + 58;
    } 
    b->pins[3].gpio.mux_total = 0;
    b->pins[3].i2c.pinmap = 0;
    b->pins[3].i2c.mux_total = 0;
    
    strncpy(b->pins[4].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[5].name, "SCL0", MRAA_PIN_NAME_SIZE);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[5].gpio.pinmap = pinbase + 47;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[5].gpio.pinmap = pinbase + 57;
    }
    b->pins[5].gpio.mux_total = 0;
    b->pins[5].i2c.pinmap = 0;
    b->pins[5].i2c.mux_total = 0;
    
    strncpy(b->pins[6].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[7].name, "GPIO46", MRAA_PIN_NAME_SIZE);        
        b->pins[7].gpio.pinmap = pinbase + 46;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[7].name, "GPIO55", MRAA_PIN_NAME_SIZE);        
        b->pins[7].gpio.pinmap = pinbase + 55;   
    }
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.mux_total = 0;
    
    strncpy(b->pins[8].name, "UART_TX", MRAA_PIN_NAME_SIZE);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[8].gpio.pinmap = pinbase + 14;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[8].gpio.pinmap = pinbase + 5;
    }
    b->pins[8].gpio.mux_total = 0;
    b->pins[8].uart.pinmap = 0;
    b->pins[8].uart.mux_total = 0;
    b->pins[8].uart.parent_id = 0;
    
    strncpy(b->pins[9].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[10].name, "UART_RX", MRAA_PIN_NAME_SIZE);
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[10].gpio.pinmap = pinbase + 13;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[10].gpio.pinmap = pinbase + 6;
    }
    b->pins[10].gpio.mux_total = 0;
    b->pins[10].uart.pinmap = 0;
    b->pins[10].uart.mux_total = 0;
    b->pins[10].uart.parent_id = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[11].name, "GPIO44", MRAA_PIN_NAME_SIZE);        
        b->pins[11].gpio.pinmap = pinbase + 44;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[11].name, "GPIO42", MRAA_PIN_NAME_SIZE);        
        b->pins[11].gpio.pinmap = pinbase + 42;
    } 
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[11].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[12].name, "GPIO45", MRAA_PIN_NAME_SIZE);        
        b->pins[12].gpio.pinmap = pinbase + 45;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[12].name, "GPIO38", MRAA_PIN_NAME_SIZE);        
        b->pins[12].gpio.pinmap = pinbase + 38;
    } 
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[12].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[13].name, "GPIO22", MRAA_PIN_NAME_SIZE);        
        b->pins[13].gpio.pinmap = pinbase + 22;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[13].name, "GPIO43", MRAA_PIN_NAME_SIZE);        
        b->pins[13].gpio.pinmap = pinbase + 43;
    } 
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.mux_total = 0;
    
    strncpy(b->pins[14].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[15].name, "GPIO20", MRAA_PIN_NAME_SIZE);        
        b->pins[15].gpio.pinmap = pinbase + 20;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[15].name, "GPIO47", MRAA_PIN_NAME_SIZE);        
        b->pins[15].gpio.pinmap = pinbase + 47;
    } 
    b->pins[15].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[15].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[16].name, "GPIO21", MRAA_PIN_NAME_SIZE);        
        b->pins[16].gpio.pinmap = pinbase + 21;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[16].name, "GPIO54", MRAA_PIN_NAME_SIZE);        
        b->pins[16].gpio.pinmap = pinbase + 54;
    } 
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[16].gpio.mux_total = 0;

    strncpy(b->pins[17].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[18].name, "GPIO19", MRAA_PIN_NAME_SIZE);        
        b->pins[18].gpio.pinmap = pinbase + 19;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[18].name, "GPIO51", MRAA_PIN_NAME_SIZE);        
        b->pins[18].gpio.pinmap = pinbase + 51;
    } 
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[18].gpio.mux_total = 0;
    
    strncpy(b->pins[19].name, "SPI_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[19].gpio.pinmap = pinbase + 18;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[19].gpio.pinmap = pinbase + 52;
    }     
    b->pins[19].gpio.mux_total = 0;
    b->pins[19].spi.pinmap = 0;
    b->pins[19].spi.mux_total = 0;
    
    strncpy(b->pins[20].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    
    strncpy(b->pins[21].name, "SPI_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[21].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[21].gpio.pinmap = pinbase + 16;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[21].gpio.pinmap = pinbase + 53;
    }  
    b->pins[21].gpio.mux_total = 0;
    b->pins[21].spi.pinmap = 0;
    b->pins[21].spi.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[22].name, "GPIO17", MRAA_PIN_NAME_SIZE);        
        b->pins[22].gpio.pinmap = pinbase + 17;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[22].name, "GPIO50", MRAA_PIN_NAME_SIZE);        
        b->pins[22].gpio.pinmap = pinbase + 50;
    } 
    b->pins[22].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[22].gpio.mux_total = 0;
    
    strncpy(b->pins[23].name, "SPI_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[23].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[23].gpio.pinmap = pinbase + 12;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[23].gpio.pinmap = pinbase + 48;
    }
    b->pins[23].gpio.mux_total = 0;
    b->pins[23].spi.pinmap = 0;
    b->pins[23].spi.mux_total = 0;
    
    strncpy(b->pins[24].name, "SPI_CS0", MRAA_PIN_NAME_SIZE);
    b->pins[24].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[24].gpio.pinmap = pinbase + 15;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[24].gpio.pinmap = pinbase + 49;
    }
    b->pins[24].gpio.mux_total = 0;
    b->pins[24].spi.pinmap = 0;
    b->pins[24].spi.mux_total = 0;
    
    strncpy(b->pins[25].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[25].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[26].name, "SPI_CS1", MRAA_PIN_NAME_SIZE);
        b->pins[26].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        b->pins[26].gpio.pinmap = pinbase + 11;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[26].name, "GPIO56", MRAA_PIN_NAME_SIZE);
        b->pins[26].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
        b->pins[26].gpio.pinmap = pinbase + 56; 
    }
    b->pins[26].gpio.mux_total = 0;
    b->pins[26].spi.pinmap = 0;
    b->pins[26].spi.mux_total = 0;
    
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[27].name, "GPIO9", MRAA_PIN_NAME_SIZE);        
        b->pins[27].gpio.pinmap = pinbase + 9;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[27].name, "GPIO45", MRAA_PIN_NAME_SIZE);        
        b->pins[27].gpio.pinmap = pinbase + 45;
    } 
    b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[27].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[28].name, "GPIO10", MRAA_PIN_NAME_SIZE);        
        b->pins[28].gpio.pinmap = pinbase + 10;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[28].name, "GPI40", MRAA_PIN_NAME_SIZE);        
        b->pins[28].gpio.pinmap = pinbase + 40;
    } 
    b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[28].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[29].name, "GPIO8", MRAA_PIN_NAME_SIZE);        
        b->pins[29].gpio.pinmap = pinbase + 8;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[29].name, "GPIO37", MRAA_PIN_NAME_SIZE);        
        b->pins[29].gpio.pinmap = pinbase + 37;
    } 
    b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[29].gpio.mux_total = 0;
    
    strncpy(b->pins[30].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[31].name, "GPIO6", MRAA_PIN_NAME_SIZE);        
        b->pins[31].gpio.pinmap = pinbase + 6;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[31].name, "GPIO39", MRAA_PIN_NAME_SIZE);        
        b->pins[31].gpio.pinmap = pinbase + 39;
    } 
    b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[31].gpio.mux_total = 0;
    
    strncpy(b->pins[32].name, "PWM0", MRAA_PIN_NAME_SIZE);
    b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[32].gpio.pinmap = pinbase + 7;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[32].gpio.pinmap = pinbase + 46;
    } 
    b->pins[32].gpio.mux_total = 0;
    
    strncpy(b->pins[33].name, "PWM1", MRAA_PIN_NAME_SIZE);
    b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        b->pins[33].gpio.pinmap = pinbase + 5;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->pins[33].gpio.pinmap = pinbase + 59;
    } 
    b->pins[33].gpio.mux_total = 0;
    
    strncpy(b->pins[34].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[35].name, "GPIO3", MRAA_PIN_NAME_SIZE);        
        b->pins[35].gpio.pinmap = pinbase + 3;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[35].name, "GPIO63", MRAA_PIN_NAME_SIZE);        
        b->pins[35].gpio.pinmap = pinbase + 63;
    } 
    b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[35].gpio.mux_total = 0;
    b->pins[35].uart.pinmap = 0;
    b->pins[35].uart.mux_total = 0;
    b->pins[35].uart.parent_id = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[36].name, "GPIO4", MRAA_PIN_NAME_SIZE);        
        b->pins[36].gpio.pinmap = pinbase + 4;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[36].name, "GPIO36", MRAA_PIN_NAME_SIZE);        
        b->pins[36].gpio.pinmap = pinbase + 36;
    } 
    b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[36].gpio.mux_total = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[37].name, "GPIO1", MRAA_PIN_NAME_SIZE);        
        b->pins[37].gpio.pinmap = pinbase + 1;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[37].name, "GPIO60", MRAA_PIN_NAME_SIZE);        
        b->pins[37].gpio.pinmap = pinbase + 60;
    } 
    b->pins[37].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[37].gpio.mux_total = 0;
    b->pins[37].uart.pinmap = 0;
    b->pins[37].uart.mux_total = 0;
    b->pins[37].uart.parent_id = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[38].name, "GPIO2", MRAA_PIN_NAME_SIZE);        
        b->pins[38].gpio.pinmap = pinbase + 2;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[38].name, "GPIO61", MRAA_PIN_NAME_SIZE);        
        b->pins[38].gpio.pinmap = pinbase + 61;
    } 
    b->pins[38].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[38].gpio.mux_total = 0;
    
    strncpy(b->pins[39].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[39].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {
        strncpy(b->pins[40].name, "GPIO0", MRAA_PIN_NAME_SIZE);        
        b->pins[40].gpio.pinmap = pinbase + 0;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        strncpy(b->pins[40].name, "GPIO44", MRAA_PIN_NAME_SIZE);        
        b->pins[40].gpio.pinmap = pinbase + 44;
    } 
    b->pins[40].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[40].gpio.mux_total = 0;
    
    b->gpio_count = 29;

    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;

    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {      
        b->i2c_bus[0].bus_id = 1; 
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->i2c_bus[0].bus_id = 0; 
    } 
    
    b->i2c_bus[0].sda = 3;
    b->i2c_bus[0].scl = 5;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    if (detected_platform == MRAA_VISIONFIVE_V1_JH7100) {      
        b->spi_bus[0].bus_id = 0;
    } else if (detected_platform == MRAA_VISIONFIVE_V2_JH7110) {
        b->spi_bus[0].bus_id = 1;
    } 
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 24;
    b->spi_bus[0].mosi = 19;
    b->spi_bus[0].miso = 21;
    b->spi_bus[0].sclk = 23;

    b->uart_dev_count = 2;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 10;
    b->uart_dev[0].tx = 8;
    b->uart_dev[0].name = "UART0";
    b->uart_dev[0].device_path = "/dev/ttyS0";
    b->uart_dev[1].rx = 37;
    b->uart_dev[1].tx = 35;
    b->uart_dev[1].name = "UART1";
    b->uart_dev[1].device_path = "/dev/ttyS1";
  
    return b;

error:
    syslog(LOG_CRIT, "visionfive: Platform failed to initialise");
    free(b);
    
    return NULL;
}
