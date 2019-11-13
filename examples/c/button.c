#include <mraa.h>
#include <inttypes.h>

#define BTN_PIN      52       /**< Button is connected to this pin */


int main(void)
{
	mraa_gpio_context      btnPin;  /* Will be used to represnt the button pin */

	uint32_t               status;   /* Used to toggle the LED */
	uint32_t               btnState; /* Used to capture the state of the button */
	
	mraa_init();

	btnPin = mraa_gpio_init(BTN_PIN);

	mraa_gpio_dir(btnPin, MRAA_GPIO_IN);

	while(1)
	{
		status = mraa_gpio_read(btnPin);
		printf("STATUS = %d..\n\n",status);
		if(status == 0)
			printf("button pressed \n");
	sleep(10);
	}

	return 0;
}
