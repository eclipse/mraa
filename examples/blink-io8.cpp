#include "stdio.h"

#include "maa.h"

int
main(int argc, char **argv)
{
    fprintf(stdout, "MAA Version: %d\n Starting Blinking on IO8", get_version());
    gpio_t gpio;
    gpio_init(&gpio, 26);
    gpio_dir(&gpio, "out");

    while(1){
        gpio_write(&gpio, 0);
        sleep(1);
        gpio_write(&gpio, 1);
        sleep(1);
    }
    return 0;
}
