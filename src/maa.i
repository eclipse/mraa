%{
    #include "maa.h"
    #include "gpio.hpp"
    #include "pwm.hpp"
    #include "i2c.hpp"
    #include "spi.hpp"
    #include "aio.hpp"
%}

%init %{
    //Adding maa_init() to the module initialisation process
    maa_init();
%}

%rename(getVersion) maa_get_version();
const char * maa_get_version();

%rename(printError) maa_result_print(maa_result_t error);
void maa_result_print(maa_result_t error);

/**
 * MAA return codes
 */
typedef enum {
    MAA_SUCCESS                              =  0, /**< Expected response */
    MAA_ERROR_FEATURE_NOT_IMPLEMENTED        =  1, /**< Feature TODO */
    MAA_ERROR_FEATURE_NOT_SUPPORTED          =  2, /**< Feature not supported by HW */
    MAA_ERROR_INVALID_VERBOSITY_LEVEL        =  3, /**< Verbosity level wrong */
    MAA_ERROR_INVALID_PARAMETER              =  4, /**< Parameter invalid */
    MAA_ERROR_INVALID_HANDLE                 =  5, /**< Handle invalid */
    MAA_ERROR_NO_RESOURCES                   =  6, /**< No resource of that type avail */
    MAA_ERROR_INVALID_RESOURCE               =  7, /**< Resource invalid */
    MAA_ERROR_INVALID_QUEUE_TYPE             =  8, /**< Queue type incorrect */
    MAA_ERROR_NO_DATA_AVAILABLE              =  9, /**< No data available */
    MAA_ERROR_INVALID_PLATFORM               = 10, /**< Platform not recognised */
    MAA_ERROR_PLATFORM_NOT_INITIALISED       = 11, /**< Board information not initialised */
    MAA_ERROR_PLATFORM_ALREADY_INITIALISED   = 12, /**< Board is already initialised */

    MAA_ERROR_UNSPECIFIED                    = 99 /**< Unknown Error */
} maa_result_t;

#### GPIO ####

%include "gpio.hpp"

#### i2c ####

%include "i2c.hpp"

#### PWM ####

%include "pwm.hpp"

#### SPI ####

%include "spi.hpp"

#### AIO ####

%include "aio.hpp"
