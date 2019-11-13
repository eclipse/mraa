/* standard headers */
#include <stdlib.h>
#include <unistd.h>

/* mraa header */
#include "mraa/gpio.h"


int
main(int argc,char **argv)
{
    int running = 0;
    mraa_result_t status = MRAA_SUCCESS;
    mraa_gpio_context gpio; 
    int GPIO_PIN = atoi(argv[1]);
    /* initialize mraa for the platform (not needed most of the times) */
    mraa_init();

    //! [Interesting]
    /* initialize GPIO pin */
    gpio = mraa_gpio_init(GPIO_PIN);
    if (gpio == NULL) {
        fprintf(stderr, "Failed to initialize GPIO %d\n", GPIO_PIN);
        mraa_deinit();
        return EXIT_FAILURE;
    }

    /* set GPIO to input */
    status = mraa_gpio_dir(gpio, MRAA_GPIO_OUT);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }
#if 0
    /* configure ISR for GPIO */
    status = mraa_gpio_isr(gpio, MRAA_GPIO_EDGE_BOTH, &int_handler, NULL);
    if (status != MRAA_SUCCESS) {
        goto err_exit;
    }

    /* wait 30 seconds isr trigger */
    sleep(30);

    /* close GPIO */
    mraa_gpio_close(gpio);
#endif

    while (running == 0) {
	    mraa_gpio_write(gpio,1);
	    sleep(5);
	     mraa_gpio_write(gpio,0);
	    sleep(5);

    }

    //! [Interesting]
    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_SUCCESS;

err_exit:
    mraa_result_print(status);

    /* deinitialize mraa for the platform (not needed most of the times) */
    mraa_deinit();

    return EXIT_FAILURE;
}
