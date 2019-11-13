
#include <mraa.h>
#include <inttypes.h>

#define LED_PIN      49        /**< The pin where the LED is connected */
#define BTN_PIN      52        /**< Button is connected to this pin */


int main(void)
{
    mraa_gpio_context      ledPin;  /* Will be used to represnt the LED pin */
    mraa_gpio_context      btnPin;  /* Will be used to represnt the button pin */
    
    uint32_t               toggle;   /* Used to toggle the LED */
    uint32_t               btnState; /* Used to capture the state of the button */
    
    
    
    /* Step1: Init the mraa subsystem */
    mraa_init();
    
    /* Step2: Set the ledPin is 49 and btnPin is 52 */
    ledPin = mraa_gpio_init(LED_PIN);
    btnPin = mraa_gpio_init(BTN_PIN);
     
    /* Step3: Set the 'ledPin(49)' pin as output */
    mraa_gpio_dir(ledPin, MRAA_GPIO_OUT);
    
    /* LED is turned off to start with */
    toggle = 0;                     
    mraa_gpio_write(ledPin, toggle);
    
    /* 
     * Step4: Set the 'btnPin(52)' pin as input, will be used to read
     * the button state
     */
    mraa_gpio_dir(btnPin, MRAA_GPIO_IN);
    
    while(1)
    {
      /* Step5: Wait until there is some activty on the button pin */
      while (mraa_gpio_read(btnPin) != 0)
      { }
      /* 
       * Step6: Button state has changed, wait for signal to settle, and 
       * then read the pin again to get the 'stable' signal, this is 
       * simplest form of debouncing.
       */
       usleep(10);  /* man usleep */
       if (mraa_gpio_read(btnPin) == 1)
       {
           /* Step7: Control the LED state based on the button toggle */
           if (toggle == 0)
           {
               toggle = 0;  /* If the LED is off, trun it on */
               
           }
           else
           {
               toggle = 1;  /* Turn off the LED */
           }
           mraa_gpio_write(ledPin, toggle);
       }
       // else: false click, ignore it
    }
    
    return 0;
}


