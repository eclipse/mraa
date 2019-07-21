/*
 * Author: Thomas Ingleby <thomas.c.ingleby@intel.com>
 * Author: Michael Ring <mail@michael-ring.org>
 * Copyright (c) 2014 Intel Corporation.
 *
 * SPDX-License-Identifier: MIT
 */

#include <dirent.h>
#include <mraa/common.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "arm/raspberry_pi.h"
#include "common.h"

#define PLATFORM_NAME_RASPBERRY_PI_B_REV_1 "Raspberry Pi Model B Rev 1"
#define PLATFORM_NAME_RASPBERRY_PI_A_REV_2 "Raspberry Pi Model A Rev 2"
#define PLATFORM_NAME_RASPBERRY_PI_B_REV_2 "Raspberry Pi Model B Rev 2"
#define PLATFORM_NAME_RASPBERRY_PI_B_PLUS_REV_1 "Raspberry Pi Model B+ Rev 1"
#define PLATFORM_NAME_RASPBERRY_PI_COMPUTE_MODULE_REV_1 "Raspberry Pi Compute Module Rev 1"
#define PLATFORM_NAME_RASPBERRY_PI_A_PLUS_REV_1 "Raspberry Pi Model A+ Rev 1"
#define PLATFORM_NAME_RASPBERRY_PI2_B_REV_1 "Raspberry Pi 2 Model B Rev 1"
#define PLATFORM_NAME_RASPBERRY_PI_ZERO "Raspberry Pi Zero"
#define PLATFORM_NAME_RASPBERRY_PI3_B "Raspberry Pi 3 Model B"
#define PLATFORM_NAME_RASPBERRY_PI_ZERO_W "Raspberry Pi Zero W"
#define PLATFORM_NAME_RASPBERRY_PI3_B_PLUS "Raspberry Pi 3 Model B+"
#define PLATFORM_NAME_RASPBERRY_PI3_A_PLUS "Raspberry Pi 3 Model A+"
#define PLATFORM_NAME_RASPBERRY_PI4_B "Raspberry Pi 4 Model B"
#define PLATFORM_RASPBERRY_PI_B_REV_1 1
#define PLATFORM_RASPBERRY_PI_A_REV_2 2
#define PLATFORM_RASPBERRY_PI_B_REV_2 3
#define PLATFORM_RASPBERRY_PI_B_PLUS_REV_1 4
#define PLATFORM_RASPBERRY_PI_COMPUTE_MODULE_REV_1 5
#define PLATFORM_RASPBERRY_PI_A_PLUS_REV_1 6
#define PLATFORM_RASPBERRY_PI2_B_REV_1 7
#define PLATFORM_RASPBERRY_PI_ZERO 8
#define PLATFORM_RASPBERRY_PI3_B 9
#define PLATFORM_RASPBERRY_PI_ZERO_W 10
#define PLATFORM_RASPBERRY_PI3_B_PLUS 11
#define PLATFORM_RASPBERRY_PI3_A_PLUS 12
#define PLATFORM_RASPBERRY_PI4_B 13
#define MMAP_PATH "/dev/mem"
#define BCM2835_PERI_BASE 0x20000000
#define BCM2836_PERI_BASE 0x3f000000
#define BCM2835_BLOCK_SIZE (4 * 1024)
#define BCM2836_BLOCK_SIZE (4 * 1024)
#define BCM2837_PERI_BASE (0x3F000000)
#define BCM2837_BLOCK_SIZE (4 * 1024)
#define BCM283X_GPSET0 0x001c
#define BCM283X_GPCLR0 0x0028
#define BCM2835_GPLEV0 0x0034
#define MAX_SIZE 64

#define GPIO_OFFSET (0x200000)
#define PWM_OFFSET (0x20C000)
#define CLOCK_OFFSET (0x101000)

// PWM
#define PWM_BASE_FREQ (19200000)

// Base register addresses
#define PWM_CTL (0)
#define PWM_RNG1 (4)
#define PWM_DAT1 (5)
#define PWMCLK_CNTL (40)
#define PWMCLK_DIV (41)
// Register addresses offsets divided by 4 (register addresses are word (32-bit) aligned
#define PWM_RNG (4095)
#define DEFAULT_PERIOD_US 500

#define MIN_PERIOD_US 400
#define MAX_PERIOD_US 500000

static volatile unsigned* clk_reg = NULL;
static volatile unsigned* gpio_reg = NULL;
static volatile unsigned* pwm_reg = NULL;


// GPIO MMAP
static uint8_t* mmap_reg = NULL;
static int mmap_fd = 0;
static int mmap_size;
static unsigned int mmap_count = 0;
static int platform_detected = 0;
static uint32_t peripheral_base = BCM2835_PERI_BASE;
static uint32_t block_size = BCM2835_BLOCK_SIZE;

static bool pwm_enabled = false;

/**
* Calculate the PWM clk divisor given a desired period
*/
static inline uint32_t
calc_divisor(uint32_t desired_period_us)
{
    float desired_freq = 1.0 / (desired_period_us / 1000000.0);
    float divisor = PWM_BASE_FREQ / (desired_freq * PWM_RNG);
    return (uint32_t)(divisor + .5); // Round to nearest integer
}

/**
* Memory map an arbitrary address
*/
static volatile unsigned*
mmap_reg_addr(unsigned long base_addr)
{
    int mem_fd = 0;
    void* reg_addr_map = MAP_FAILED;

    /* open /dev/mem.....need to run program as root i.e. use sudo or su */
    if (!mem_fd) {
        if ((mem_fd = open("/dev/mem", O_RDWR | O_SYNC)) < 0) {
            perror("can't open /dev/mem");
            return NULL;
        }
    }

    /* mmap IO */
    reg_addr_map =
    mmap(NULL,                               // Any adddress in our space will do
         block_size,                         // Map length
         PROT_READ | PROT_WRITE | PROT_EXEC, // Enable reading & writting to mapped memory
         MAP_SHARED | MAP_LOCKED,            // Shared with other processes
         mem_fd,                             // File to map
         base_addr                           // Offset to base address
         );

    if (reg_addr_map == MAP_FAILED) {
        perror("mmap error");
        close(mem_fd);
        return NULL;
    }

    if (close(mem_fd) < 0) { // No need to keep mem_fd open after mmap
        // i.e. we can close /dev/mem
        perror("couldn't close /dev/mem file descriptor");
        return NULL;
    }
    return (volatile unsigned*) reg_addr_map;
}

/**
* Memory map PWM registers
*/
static int8_t
mmap_regs()
{
    // Already mapped!
    if (clk_reg || gpio_reg || pwm_reg) {
        return 0;
    }

    clk_reg = mmap_reg_addr(peripheral_base + CLOCK_OFFSET);
    gpio_reg = mmap_reg_addr(peripheral_base + GPIO_OFFSET);
    pwm_reg = mmap_reg_addr(peripheral_base + PWM_OFFSET);

    // If all 3 weren't mapped correctly
    if (!(clk_reg && gpio_reg && pwm_reg)) {
        return -1;
    }

    return 0;
}

static void
pwm_stop()
{
    // stop clock and waiting for busy flag doesn't work, so kill clock
    *(clk_reg + PWMCLK_CNTL) = 0x5A000000 | (1 << 5);
    usleep(10);

    // wait until busy flag is set
    while ((*(clk_reg + PWMCLK_CNTL)) & 0x00000080) {
    }

    *(pwm_reg + PWM_CTL) = 0;
    usleep(10);
}

static void
pwm_start()
{
    // source=osc and enable clock
    *(clk_reg + PWMCLK_CNTL) = 0x5A000011;

    // start PWM1 in mark space mode
    *(pwm_reg + PWM_CTL) |= ((1 << 7) | (1 << 0));

    usleep(10);
}


static void
config_pwm1(uint32_t divisor)
{
    /*GPIO 18 in ALT5 mode for PWM0 */
    // Let's first set pin 18 to input
    // taken from #define INP_GPIO(g) *(gpio+((g)/10)) &= ~(7<<(((g)%10)*3))
    *(gpio_reg + 1) &= ~(7 << 24);
    // then set it to ALT5 function PWM0
    // taken from #define SET_GPIO_ALT(g,a) *(gpio+(((g)/10))) |=
    // (((a)<=3?(a)+4:(a)==4?3:2)<<(((g)%10)*3))
    *(gpio_reg + 1) |= (2 << 24);

    pwm_stop();

    // set divisor
    *(clk_reg + PWMCLK_DIV) = 0x5A000000 | (divisor << 12);

    // disable PWM & start from a clean slate
    *(pwm_reg + PWM_CTL) = 0;

    // needs some time until the PWM module gets disabled, without the delay the PWM module crashs
    usleep(10);

    // set the number of counts that constitute a period with 0 for 20 milliseconds = 320 bits
    *(pwm_reg + PWM_RNG1) = PWM_RNG;
    usleep(10);

    // If the user has pwm set to enabled
    if (pwm_enabled) {
        pwm_start();
    }
}


// This shouldn't be a float, since it's called in mraa_pwm_read_duty, which
// seems to return the duty cycle in us
float
mraa_raspberry_pi_duty_cycle_read_replace(mraa_pwm_context dev)
{
    uint32_t duty_counts = *(pwm_reg + PWM_DAT1);
    return (((float) duty_counts) / PWM_RNG) * dev->period;
}


mraa_result_t
mraa_raspberry_pi_pwm_period_us_replace(mraa_pwm_context dev, int period)
{
    config_pwm1(calc_divisor(period / 1000));
    dev->period = period;
    return MRAA_SUCCESS;
}

// Used in pwm.c, duty is actually just the raw counts
// Shouldn't be a float
mraa_result_t
mraa_raspberry_pi_pwm_write_duty_replace(mraa_pwm_context dev, float duty)
{
    // We are given duty as a percentage of the period
    // Convert it to be a percentage of the range
    float duty_pct = duty / dev->period;
    *(pwm_reg + PWM_DAT1) = (uint32_t)(duty_pct * PWM_RNG);
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_raspberry_pi_pwm_enable_replace(mraa_pwm_context dev, int enable)
{
    if (enable == 1) {
        pwm_enabled = true;
        pwm_start();
    } else {
        pwm_enabled = false;
        pwm_stop();
    }

    return MRAA_SUCCESS;
}

mraa_result_t
mraa_raspberry_pi_pwm_initraw_replace(mraa_pwm_context dev, int pin)
{
    if (pin != 0) {
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    if (mmap_regs() != 0) {
        return MRAA_ERROR_UNSPECIFIED;
    }

    config_pwm1(DEFAULT_PERIOD_US);
    *(pwm_reg + PWM_DAT1) = 0;
    dev->period = DEFAULT_PERIOD_US;

    return MRAA_SUCCESS;
}


mraa_result_t
mraa_raspberry_pi_spi_init_pre(int index)
{
    char devpath[MAX_SIZE];
    sprintf(devpath, "/dev/spidev%u.0", plat->spi_bus[index].bus_id);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_ERR, "spi: Device not initialized");
        syslog(LOG_ERR, "spi: If you run a kernel >=3.18 then you will have to add dtparam=spi=on "
                        "to /boot/config.txt and reboot");
        syslog(LOG_INFO, "spi: trying modprobe for spi-bcm2708");
        system("modprobe spi-bcm2708 >/dev/null 2>&1");
        system("modprobe spi_bcm2708 >/dev/null 2>&1");
        if (!mraa_file_exist(devpath)) {
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_raspberry_pi_i2c_init_pre(unsigned int bus)
{
    char devpath[MAX_SIZE];
    sprintf(devpath, "/dev/i2c-%u", bus);
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_INFO, "spi: trying modprobe for i2c-bcm2708 & i2c-dev");
        system("modprobe i2c-bcm2708 >/dev/null 2>&1");
        system("modprobe i2c-dev >/dev/null 2>&1");
        system("modprobe i2c_bcm2708 >/dev/null 2>&1");
        system("modprobe i2c_dev >/dev/null 2>&1");
    }
    if (!mraa_file_exist(devpath)) {
        syslog(LOG_ERR, "i2c: Device not initialized");
        if (platform_detected == PLATFORM_RASPBERRY_PI_B_REV_1) {
            syslog(LOG_ERR, "i2c: If you run a kernel >=3.18 then you will have to add "
                            "dtparam=i2c0=on to /boot/config.txt and reboot");
        } else {
            syslog(LOG_ERR, "i2c: If you run a kernel >=3.18 then you will have to add "
                            "dtparam=i2c1=on to /boot/config.txt and reboot");
        }
        return MRAA_ERROR_NO_RESOURCES;
    }
    return MRAA_SUCCESS;
}

mraa_result_t
mraa_raspberry_pi_mmap_write(mraa_gpio_context dev, int value)
{
    if (value) {
        *(volatile uint32_t*) (mmap_reg + BCM283X_GPSET0 + (dev->pin / 32) * 4) =
        (uint32_t)(1 << (dev->pin % 32));
    } else {
        *(volatile uint32_t*) (mmap_reg + BCM283X_GPCLR0 + (dev->pin / 32) * 4) =
        (uint32_t)(1 << (dev->pin % 32));
    }
    return MRAA_SUCCESS;
}

static mraa_result_t
mraa_raspberry_pi_mmap_unsetup()
{
    if (mmap_reg == NULL) {
        syslog(LOG_ERR, "raspberry mmap: null register cant unsetup");
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
mraa_raspberry_pi_mmap_read(mraa_gpio_context dev)
{
    uint32_t value = *(volatile uint32_t*) (mmap_reg + BCM2835_GPLEV0 + (dev->pin / 32) * 4);
    if (value & (uint32_t)(1 << (dev->pin % 32))) {
        return 1;
    }
    return 0;
}

mraa_result_t
mraa_raspberry_pi_mmap_setup(mraa_gpio_context dev, mraa_boolean_t en)
{
    if (dev == NULL) {
        syslog(LOG_ERR, "raspberry mmap: context not valid");
        return MRAA_ERROR_INVALID_HANDLE;
    }

    if (en == 0) {
        if (dev->mmap_write == NULL && dev->mmap_read == NULL) {
            syslog(LOG_ERR, "raspberry mmap: can't disable disabled mmap gpio");
            return MRAA_ERROR_INVALID_PARAMETER;
        }
        dev->mmap_write = NULL;
        dev->mmap_read = NULL;
        mmap_count--;
        if (mmap_count == 0) {
            return mraa_raspberry_pi_mmap_unsetup();
        }
        return MRAA_SUCCESS;
    }

    if (dev->mmap_write != NULL && dev->mmap_read != NULL) {
        syslog(LOG_ERR, "raspberry mmap: can't enable enabled mmap gpio");
        return MRAA_ERROR_INVALID_PARAMETER;
    }

    // Might need to make some elements of this thread safe.
    // For example only allow one thread to enter the following block
    // to prevent mmap'ing twice.
    if (mmap_reg == NULL) {
        if ((mmap_fd = open(MMAP_PATH, O_RDWR)) < 0) {
            syslog(LOG_ERR, "raspberry map: unable to open resource0 file");
            return MRAA_ERROR_INVALID_HANDLE;
        }

        mmap_reg = (uint8_t*) mmap(NULL, block_size, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED,
                                   mmap_fd, peripheral_base + GPIO_OFFSET);
        if (mmap_reg == MAP_FAILED) {
            syslog(LOG_ERR, "raspberry mmap: failed to mmap");
            mmap_reg = NULL;
            close(mmap_fd);
            return MRAA_ERROR_NO_RESOURCES;
        }
    }
    dev->mmap_write = &mraa_raspberry_pi_mmap_write;
    dev->mmap_read = &mraa_raspberry_pi_mmap_read;
    mmap_count++;

    return MRAA_SUCCESS;
}

mraa_board_t*
mraa_raspberry_pi()
{
    mraa_board_t* b = (mraa_board_t*) calloc(1, sizeof(mraa_board_t));
    int pin_base = 0;

    if (b == NULL) {
        return NULL;
    }
    b->phy_pin_count = 0;

    size_t len = 100;
    char* line = calloc(len, sizeof(char));

    mraa_boolean_t tweakedCpuinfo = 0;

    FILE* fh = fopen("/proc/cpuinfo", "r");
    if (fh != NULL) {
        while (getline(&line, &len, fh) != -1) {
            if (strncmp(line, "Revision", 8) == 0) {
                tweakedCpuinfo = 1;
                if (strstr(line, "0002") || strstr(line, "0003")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI_B_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_B_REV_1_PINCOUNT;
                } else if (strstr(line, "0004") || strstr(line, "0005") || strstr(line, "0006") ||
                           strstr(line, "000d") || strstr(line, "000e") || strstr(line, "000f")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_2;
                    platform_detected = PLATFORM_RASPBERRY_PI_B_REV_2;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_AB_REV_2_PINCOUNT;
                } else if (strstr(line, "900092") || strstr(line, "900093") || strstr(line, "920093")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_ZERO;
                    platform_detected = PLATFORM_RASPBERRY_PI_ZERO;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_ZERO_PINCOUNT;
                } else if (strstr(line, "0007") || strstr(line, "0008") || strstr(line, "0009")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_A_REV_2;
                    platform_detected = PLATFORM_RASPBERRY_PI_A_REV_2;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_AB_REV_2_PINCOUNT;
                } else if (strstr(line, "0010")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_PLUS_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI_B_PLUS_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_AB_PLUS_PINCOUNT;
                } else if (strstr(line, "0011")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_COMPUTE_MODULE_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI_COMPUTE_MODULE_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_COMPUTE_MODULE_PINCOUNT;
                } else if (strstr(line, "0012")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_A_PLUS_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI_A_PLUS_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_AB_PLUS_PINCOUNT;
                } else if (strstr(line, "a01041") || strstr(line, "a21041")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI2_B_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI2_B_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI2_B_REV_1_PINCOUNT;
                    peripheral_base = BCM2836_PERI_BASE;
                    block_size = BCM2836_BLOCK_SIZE;
                } else if (strstr(line, "a02082") || strstr(line, "a020a0") ||
                           strstr(line, "a22082") || strstr(line, "a32082")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_B;
                    platform_detected = PLATFORM_RASPBERRY_PI3_B;
                    b->phy_pin_count = MRAA_RASPBERRY_PI3_B_PINCOUNT;
                    peripheral_base = BCM2837_PERI_BASE;
                    block_size = BCM2837_BLOCK_SIZE;
                } else if (strstr(line, "9000c1")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_ZERO_W;
                    platform_detected = PLATFORM_RASPBERRY_PI_ZERO_W;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_ZERO_W_PINCOUNT;
                } else if (strstr(line, "a020d3")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_B_PLUS;
                    platform_detected = PLATFORM_RASPBERRY_PI3_B_PLUS;
                    b->phy_pin_count = MRAA_RASPBERRY_PI3_B_PLUS_PINCOUNT;
                    peripheral_base = BCM2837_PERI_BASE;
                    block_size = BCM2837_BLOCK_SIZE; 
                } else if (strstr(line, "9020e0")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_A_PLUS;
                    platform_detected = PLATFORM_RASPBERRY_PI3_A_PLUS;
                    b->phy_pin_count = MRAA_RASPBERRY_PI3_A_PLUS_PINCOUNT;
                    peripheral_base = BCM2837_PERI_BASE;
                    block_size = BCM2837_BLOCK_SIZE;
                } else if (strstr(line, "a03111") || strstr(line, "b03111") ||
                    strstr(line, "c03111")) {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI4_B;
                    platform_detected = PLATFORM_RASPBERRY_PI4_B;
                    b->phy_pin_count = MRAA_RASPBERRY_PI4_B_PINCOUNT;
                    peripheral_base = BCM2837_PERI_BASE;
                    block_size = BCM2837_BLOCK_SIZE;
                } else {
                    b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_1;
                    platform_detected = PLATFORM_RASPBERRY_PI_B_REV_1;
                    b->phy_pin_count = MRAA_RASPBERRY_PI_B_REV_1_PINCOUNT;
                }
            }
        }
        fclose(fh);
    }
    free(line);

    // Some distros have a Revision line in /proc/cpuinfo for rpi.
    // As this may not be the case for all distros, we need to find
    // another way to guess the raspberry pi model.
    if (!tweakedCpuinfo) {
        // See Documentation/devicetree/bindings/arm/bcm/brcm,bcm2835.txt
        // for the values
        const char* compatible_path = "/proc/device-tree/compatible";
        if (mraa_file_contains(compatible_path, "raspberrypi,model-b")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI_B_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI_B_REV_1_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-b-rev2")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_2;
            platform_detected = PLATFORM_RASPBERRY_PI_B_REV_2;
            b->phy_pin_count = MRAA_RASPBERRY_PI_AB_REV_2_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-zero")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_ZERO;
            platform_detected = PLATFORM_RASPBERRY_PI_ZERO;
            b->phy_pin_count = MRAA_RASPBERRY_PI_ZERO_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-a")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_A_REV_2;
            platform_detected = PLATFORM_RASPBERRY_PI_A_REV_2;
            b->phy_pin_count = MRAA_RASPBERRY_PI_AB_REV_2_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-b-plus")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_PLUS_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI_B_PLUS_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI_AB_PLUS_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,compute-module")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_COMPUTE_MODULE_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI_COMPUTE_MODULE_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI_COMPUTE_MODULE_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-a-plus")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_A_PLUS_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI_A_PLUS_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI_AB_PLUS_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,2-model-b")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI2_B_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI2_B_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI2_B_REV_1_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-b")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_B_REV_1;
            platform_detected = PLATFORM_RASPBERRY_PI_B_REV_1;
            b->phy_pin_count = MRAA_RASPBERRY_PI_B_REV_1_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,3-model-b")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_B;
            platform_detected = PLATFORM_RASPBERRY_PI3_B;
            b->phy_pin_count = MRAA_RASPBERRY_PI3_B_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,model-zero-w")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI_ZERO_W;
            platform_detected = PLATFORM_RASPBERRY_PI_ZERO_W;
            b->phy_pin_count = MRAA_RASPBERRY_PI_ZERO_W_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,3-model-b-plus")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_B_PLUS;
            platform_detected = PLATFORM_RASPBERRY_PI3_B_PLUS;
            b->phy_pin_count = MRAA_RASPBERRY_PI3_B_PLUS_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,3-model-a-plus")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI3_A_PLUS;
            platform_detected = PLATFORM_RASPBERRY_PI3_A_PLUS;
            b->phy_pin_count = MRAA_RASPBERRY_PI3_A_PLUS_PINCOUNT;
        } else if (mraa_file_contains(compatible_path, "raspberrypi,4-model-b")) {
            b->platform_name = PLATFORM_NAME_RASPBERRY_PI4_B;
            platform_detected = PLATFORM_RASPBERRY_PI4_B;
            b->phy_pin_count = MRAA_RASPBERRY_PI4_B_PINCOUNT;
        }
    }

    b->aio_count = 0;
    b->adc_raw = 0;
    b->adc_supported = 0;
    b->pwm_default_period = DEFAULT_PERIOD_US;
    b->pwm_max_period = MAX_PERIOD_US;
    b->pwm_min_period = MIN_PERIOD_US;

    if (b->phy_pin_count == 0) {
        free(b);
        syslog(LOG_ERR, "raspberrypi: Failed to detect platform revision");
        return NULL;
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

    // Detect the base of the gpiochip, raspbian hardcodes it to 0 in the kernel
    // while upstream kernel does not.
    DIR* gpio_dir = opendir("/sys/class/gpio");
    if (gpio_dir == NULL) {
        free(b->adv_func);
        free(b);
        return NULL;
    }

    struct dirent* child;
    while ((child = readdir(gpio_dir)) != NULL) {
        if (strstr(child->d_name, "gpiochip")) {
            char chip_path[MAX_SIZE];
            sprintf(chip_path, "/sys/class/gpio/%s/label", child->d_name);
            if (mraa_file_contains(chip_path, "bcm2835")) {
                if (mraa_atoi(child->d_name + 8, &pin_base) != MRAA_SUCCESS) {
                    free(b->adv_func);
                    free(b);
                    return NULL;
                }
                break;
            }
        }
    }

    b->adv_func->spi_init_pre = &mraa_raspberry_pi_spi_init_pre;
    b->adv_func->i2c_init_pre = &mraa_raspberry_pi_i2c_init_pre;
    b->adv_func->gpio_mmap_setup = &mraa_raspberry_pi_mmap_setup;
    b->adv_func->pwm_init_raw_replace = &mraa_raspberry_pi_pwm_initraw_replace;
    b->adv_func->pwm_write_replace = &mraa_raspberry_pi_pwm_write_duty_replace;
    b->adv_func->pwm_period_replace = &mraa_raspberry_pi_pwm_period_us_replace;
    b->adv_func->pwm_read_replace = &mraa_raspberry_pi_duty_cycle_read_replace;
    b->adv_func->pwm_enable_replace = &mraa_raspberry_pi_pwm_enable_replace;

    strncpy(b->pins[0].name, "INVALID", MRAA_PIN_NAME_SIZE);
    b->pins[0].capabilities = (mraa_pincapabilities_t){ 0, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[1].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[1].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[2].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[2].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[3].name, "SDA0", MRAA_PIN_NAME_SIZE);
    b->pins[3].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    b->pins[3].gpio.pinmap = pin_base + 2;
    b->pins[3].gpio.mux_total = 0;
    b->pins[3].i2c.pinmap = 0;
    b->pins[3].i2c.mux_total = 0;

    strncpy(b->pins[4].name, "5V", MRAA_PIN_NAME_SIZE);
    b->pins[4].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[5].name, "SCL0", MRAA_PIN_NAME_SIZE);
    b->pins[5].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 1, 0, 0 };
    b->pins[5].gpio.pinmap = pin_base + 3;
    b->pins[5].gpio.mux_total = 0;
    b->pins[5].i2c.pinmap = 0;
    b->pins[5].i2c.mux_total = 0;

    strncpy(b->pins[6].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[6].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[7].name, "GPIO4", MRAA_PIN_NAME_SIZE);
    b->pins[7].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[7].gpio.pinmap = pin_base + 4;
    b->pins[7].gpio.mux_total = 0;

    strncpy(b->pins[8].name, "UART_TX", MRAA_PIN_NAME_SIZE);
    b->pins[8].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[8].gpio.pinmap = pin_base + 14;
    b->pins[8].gpio.mux_total = 0;
    b->pins[8].uart.parent_id = 0;
    b->pins[8].uart.mux_total = 0;

    strncpy(b->pins[9].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[9].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[10].name, "UART_RX", MRAA_PIN_NAME_SIZE);
    b->pins[10].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 1 };
    b->pins[10].gpio.pinmap = pin_base + 15;
    b->pins[10].gpio.mux_total = 0;
    b->pins[10].uart.parent_id = 0;
    b->pins[10].uart.mux_total = 0;

    strncpy(b->pins[11].name, "GPIO17", MRAA_PIN_NAME_SIZE);
    b->pins[11].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[11].gpio.pinmap = pin_base + 17;
    b->pins[11].gpio.mux_total = 0;

    strncpy(b->pins[12].name, "GPIO18", MRAA_PIN_NAME_SIZE);
    b->pins[12].capabilities = (mraa_pincapabilities_t){ 1, 1, 1, 0, 0, 0, 0, 0 };
    b->pins[12].gpio.pinmap = pin_base + 18;
    b->pins[12].gpio.mux_total = 0;
    b->pins[12].pwm.pinmap = 0;

    if (platform_detected == PLATFORM_RASPBERRY_PI_B_REV_1) {
        strncpy(b->pins[13].name, "GPIO21", MRAA_PIN_NAME_SIZE);
        b->pins[13].gpio.pinmap = pin_base + 21;
    } else {
        strncpy(b->pins[13].name, "GPIO27", MRAA_PIN_NAME_SIZE);
        b->pins[13].gpio.pinmap = pin_base + 27;
    }
    b->pins[13].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[13].gpio.mux_total = 0;

    strncpy(b->pins[14].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[14].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[15].name, "GPIO22", MRAA_PIN_NAME_SIZE);
    b->pins[15].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[15].gpio.pinmap = pin_base + 22;
    b->pins[15].gpio.mux_total = 0;

    strncpy(b->pins[16].name, "GPIO23", MRAA_PIN_NAME_SIZE);
    b->pins[16].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[16].gpio.pinmap = pin_base + 23;
    b->pins[16].gpio.mux_total = 0;

    strncpy(b->pins[17].name, "3V3", MRAA_PIN_NAME_SIZE);
    b->pins[17].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[18].name, "GPIO24", MRAA_PIN_NAME_SIZE);
    b->pins[18].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[18].gpio.pinmap = pin_base + 24;
    b->pins[18].gpio.mux_total = 0;

    strncpy(b->pins[19].name, "SPI_MOSI", MRAA_PIN_NAME_SIZE);
    b->pins[19].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[19].gpio.pinmap = pin_base + 10;
    b->pins[19].gpio.mux_total = 0;
    b->pins[19].spi.pinmap = 0;
    b->pins[19].spi.mux_total = 0;

    strncpy(b->pins[20].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[20].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[21].name, "SPI_MISO", MRAA_PIN_NAME_SIZE);
    b->pins[21].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[21].gpio.pinmap = pin_base + 9;
    b->pins[21].gpio.mux_total = 0;
    b->pins[21].spi.pinmap = 0;
    b->pins[21].spi.mux_total = 0;

    strncpy(b->pins[22].name, "GPIO25", MRAA_PIN_NAME_SIZE);
    b->pins[22].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
    b->pins[22].gpio.pinmap = pin_base + 25;
    b->pins[22].gpio.mux_total = 0;

    strncpy(b->pins[23].name, "SPI_CLK", MRAA_PIN_NAME_SIZE);
    b->pins[23].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[23].gpio.pinmap = pin_base + 11;
    b->pins[23].gpio.mux_total = 0;
    b->pins[23].spi.pinmap = 0;
    b->pins[23].spi.mux_total = 0;

    strncpy(b->pins[24].name, "SPI_CS0", MRAA_PIN_NAME_SIZE);
    b->pins[24].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[24].gpio.pinmap = pin_base + 8;
    b->pins[24].gpio.mux_total = 0;
    b->pins[24].spi.pinmap = 0;
    b->pins[24].spi.mux_total = 0;

    strncpy(b->pins[25].name, "GND", MRAA_PIN_NAME_SIZE);
    b->pins[25].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

    strncpy(b->pins[26].name, "SPI_CS1", MRAA_PIN_NAME_SIZE);
    b->pins[26].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 1, 0, 0, 0 };
    b->pins[26].gpio.pinmap = pin_base + 7;
    b->pins[26].gpio.mux_total = 0;
    b->pins[26].spi.pinmap = 0;
    b->pins[26].spi.mux_total = 0;

    if ((platform_detected == PLATFORM_RASPBERRY_PI_A_REV_2) ||
        (platform_detected == PLATFORM_RASPBERRY_PI_B_REV_2)) {
        strncpy(b->pins[27].name, "5V", MRAA_PIN_NAME_SIZE);
        b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[28].name, "3V3", MRAA_PIN_NAME_SIZE);
        b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[29].name, "GPIO8", MRAA_PIN_NAME_SIZE);
        b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[29].gpio.pinmap = pin_base + 8;
        b->pins[29].gpio.mux_total = 0;

        strncpy(b->pins[30].name, "GPIO9", MRAA_PIN_NAME_SIZE);
        b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[30].gpio.pinmap = pin_base + 9;
        b->pins[30].gpio.mux_total = 0;

        strncpy(b->pins[31].name, "GPIO10", MRAA_PIN_NAME_SIZE);
        b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[31].gpio.pinmap = pin_base + 10;
        b->pins[31].gpio.mux_total = 0;

        strncpy(b->pins[32].name, "GPIO11", MRAA_PIN_NAME_SIZE);
        b->pins[32].gpio.pinmap = pin_base + 11;
        b->pins[32].gpio.mux_total = 0;
        b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[33].name, "GND", MRAA_PIN_NAME_SIZE);
        b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[34].name, "GND", MRAA_PIN_NAME_SIZE);
        b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };
    }

    // BUS DEFINITIONS
    b->i2c_bus_count = 1;
    b->def_i2c_bus = 0;
    if (platform_detected == PLATFORM_RASPBERRY_PI_B_REV_1)
        b->i2c_bus[0].bus_id = 0;
    else
        b->i2c_bus[0].bus_id = 1;
    b->i2c_bus[0].sda = 3;
    b->i2c_bus[0].scl = 5;

    b->spi_bus_count = 1;
    b->def_spi_bus = 0;
    b->spi_bus[0].bus_id = 0;
    b->spi_bus[0].slave_s = 0;
    b->spi_bus[0].cs = 24;
    b->spi_bus[0].mosi = 19;
    b->spi_bus[0].miso = 21;
    b->spi_bus[0].sclk = 23;

    b->uart_dev_count = 1;
    b->def_uart_dev = 0;
    b->uart_dev[0].rx = 10;
    b->uart_dev[0].tx = 8;

    if ((platform_detected == PLATFORM_RASPBERRY_PI_A_PLUS_REV_1) ||
        (platform_detected == PLATFORM_RASPBERRY_PI_B_PLUS_REV_1) ||
        (platform_detected == PLATFORM_RASPBERRY_PI2_B_REV_1) ||
        (platform_detected == PLATFORM_RASPBERRY_PI3_B) ||
        (platform_detected == PLATFORM_RASPBERRY_PI_ZERO) ||
        (platform_detected == PLATFORM_RASPBERRY_PI_ZERO_W) ||
        (platform_detected == PLATFORM_RASPBERRY_PI3_B_PLUS) ||
        (platform_detected == PLATFORM_RASPBERRY_PI3_A_PLUS) ||
        (platform_detected == PLATFORM_RASPBERRY_PI4_B)) {

        strncpy(b->pins[27].name, "ID_SD", MRAA_PIN_NAME_SIZE);
        b->pins[27].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[28].name, "ID_SC", MRAA_PIN_NAME_SIZE);
        b->pins[28].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[29].name, "GPIO05", MRAA_PIN_NAME_SIZE);
        b->pins[29].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[29].gpio.pinmap = pin_base + 5;
        b->pins[29].gpio.mux_total = 0;

        strncpy(b->pins[30].name, "GND", MRAA_PIN_NAME_SIZE);
        b->pins[30].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[31].name, "GPIO06", MRAA_PIN_NAME_SIZE);
        b->pins[31].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[31].gpio.pinmap = pin_base + 6;
        b->pins[31].gpio.mux_total = 0;

        strncpy(b->pins[32].name, "GPIO12", MRAA_PIN_NAME_SIZE);
        b->pins[32].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[32].gpio.pinmap = pin_base + 12;
        b->pins[32].gpio.mux_total = 0;

        strncpy(b->pins[33].name, "GPIO13", MRAA_PIN_NAME_SIZE);
        b->pins[33].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[33].gpio.pinmap = pin_base + 13;
        b->pins[33].gpio.mux_total = 0;

        strncpy(b->pins[34].name, "GND", MRAA_PIN_NAME_SIZE);
        b->pins[34].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[35].name, "GPIO19", MRAA_PIN_NAME_SIZE);
        b->pins[35].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[35].gpio.pinmap = pin_base + 19;
        b->pins[35].gpio.mux_total = 0;

        strncpy(b->pins[36].name, "GPIO16", MRAA_PIN_NAME_SIZE);
        b->pins[36].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[36].gpio.pinmap = pin_base + 16;
        b->pins[36].gpio.mux_total = 0;

        strncpy(b->pins[37].name, "GPIO26", MRAA_PIN_NAME_SIZE);
        b->pins[37].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[37].gpio.pinmap = pin_base + 26;
        b->pins[37].gpio.mux_total = 0;

        strncpy(b->pins[38].name, "GPIO20", MRAA_PIN_NAME_SIZE);
        b->pins[38].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[38].gpio.pinmap = pin_base + 20;
        b->pins[38].gpio.mux_total = 0;

        strncpy(b->pins[39].name, "GND", MRAA_PIN_NAME_SIZE);
        b->pins[39].capabilities = (mraa_pincapabilities_t){ 1, 0, 0, 0, 0, 0, 0, 0 };

        strncpy(b->pins[40].name, "GPIO21", MRAA_PIN_NAME_SIZE);
        b->pins[40].capabilities = (mraa_pincapabilities_t){ 1, 1, 0, 0, 0, 0, 0, 0 };
        b->pins[40].gpio.pinmap = pin_base + 21;
        b->pins[40].gpio.mux_total = 0;
    }

    b->gpio_count = 0;
    int i;
    for (i = 0; i < b->phy_pin_count; i++) {
        if (b->pins[i].capabilities.gpio) {
            b->gpio_count++;
        }
    }

    return b;
}
