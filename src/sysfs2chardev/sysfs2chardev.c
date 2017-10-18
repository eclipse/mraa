#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "linux/gpio.h"
#include "gpio/gpio_chardev.h"


#define SYSFS_CLASS_GPIO "/sys/class/gpio"
#define MAX_SIZE 64
#define GPIO_OPEN 0
#define GPIO_CLOSE 1

int num_chips = 0;

typedef struct {
    int gpio_chip;
    int gpio_line;
} chardev_gpio;

int mraa_sysfs_toggle_gpio(int gpio_number, int mode)
{
    char bu[MAX_SIZE];
    int export, length;

    if (mode == GPIO_OPEN) {
        export = open(SYSFS_CLASS_GPIO "/export", O_WRONLY);
    } else if (mode == GPIO_CLOSE) {
        export = open(SYSFS_CLASS_GPIO "/unexport", O_WRONLY);
    } else {
        syslog(LOG_ERR, "[sysfs2chardev]: Unknown mode for mraa_sysfs_toggle_gpio()");
        return -1;
    }

    if (export == -1) {
        syslog(LOG_ERR, "[sysfs2chardev]: Failed to open 'export' for gpio: %d", gpio_number);
        return -1;
    }

    length = snprintf(bu, sizeof(bu), "%d", gpio_number);
    if (write(export, bu, length * sizeof(char)) == -1) {
        syslog(LOG_ERR, "[sysfs2chardev]: Failed to write to 'export' for gpio: %d:", gpio_number);
        close(export);
        return -1;
    }

    close(export);

    return 0;
}

chardev_gpio mraa_translate_gpio_pin(int gpio_number)
{
    mraa_gpiod_chip_info *cinfo;
    mraa_gpiod_line_info *linfo;
    chardev_gpio gpio = {-1, -1};
    int found = 0;

    if (mraa_sysfs_toggle_gpio(gpio_number, GPIO_OPEN) != 0) {
        syslog(LOG_ERR, "[sysfs2chardev]: Error opening gpio %d. Maybe gpio is already in use by sysfs.", gpio_number);
        return gpio;
    }

    for (int i = 0; i < num_chips; ++i) {
        cinfo = mraa_get_chip_info_by_number(i);
        if (cinfo == NULL) {
            syslog(LOG_ERR, "[sysfs2chardev]: Chip info is NULL");
            return gpio;
        }


        for (int j = 0; j < cinfo->chip_info.lines; ++j) {
            linfo = mraa_get_line_info_from_descriptor(cinfo->chip_fd, j);

            if (linfo == NULL) {
                syslog(LOG_ERR, "[sysfs2chardev]: Line info is NULL");
                return gpio;
            }

            if (strncmp(linfo->consumer, "sysfs", 5) == 0) {
                gpio.gpio_chip = i;
                gpio.gpio_line = j;

                free(linfo);
                found = 1;
                break;
            }

            free(linfo);
        }

        free(cinfo);

        if (found)
            break;
    }

    if (mraa_sysfs_toggle_gpio(gpio_number, GPIO_CLOSE) != 0) {
        syslog(LOG_ERR, "[sysfs2chardev]: Error closing gpio %d.", gpio_number);
    }

    return gpio;
}

int main(int argc, char **argv)
{
    int gpio_num;
    chardev_gpio gpio;

    if (argc < 2) {
        fprintf(stdout, "Usage: ./sysfs2chardev gpio_1 gpio_2 ...\n. The gpio numbers must be the ones known by OS.(like [...].gpio.pinmap field)\n");
        return -1;
    }

    num_chips = mraa_get_number_of_gpio_chips();
    if (num_chips <= 0) {
        syslog(LOG_ERR, "[sysfs2chardev]: Invalid number of gpio chips detected!");
        return -1;
    }

    for (int i = 1; i < argc; ++i) {
        gpio_num = strtol(argv[i], NULL, 10);
        gpio = mraa_translate_gpio_pin(gpio_num);
        if (gpio.gpio_chip == -1) {
            fprintf(stdout, "GPIO %3d:\tError. Please check syslog.\n", gpio_num);
        } else {
            fprintf(stdout, "GPIO %3d:\tChip: %3d; Line: %3d\n", gpio_num, gpio.gpio_chip, gpio.gpio_line);
        }
    }

    return 0;
}