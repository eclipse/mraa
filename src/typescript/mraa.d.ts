/**
 * Created by barais on 20/01/15.
 */
declare module "mraa" {


    module mraa {

        /**
         * MRAA supported platform types
         */
        export  enum mraa_platform_t{
            MRAA_INTEL_GALILEO_GEN1 = 0, /**< The Generation 1 Galileo platform (RevD) */
            MRAA_INTEL_GALILEO_GEN2 = 1, /**< The Generation 2 Galileo platform (RevG/H) */
            MRAA_INTEL_EDISON_FAB_C = 2, /**< The Intel Edison (FAB C) */
            MRAA_INTEL_DE3815 = 3, /**< The Intel DE3815 Baytrail NUC */
            MRAA_INTEL_MINNOWBOARD_MAX = 4, /**< The Intel Minnow Board Max */
            MRAA_RASPBERRY_PI_B = 5, /**< The Raspberry PI Model B */

            MRAA_UNKNOWN_PLATFORM = 99 /**< An unknown platform type, typically will load INTEL_GALILEO_GEN1 */
        }

        /**
         * Intel edison miniboard numbering enum
         */
        export enum mraa_intel_edison_miniboard_t{
            MRAA_INTEL_EDISON_MINIBOARD_J17_1 = 0,
            MRAA_INTEL_EDISON_MINIBOARD_J17_5 = 4,
            MRAA_INTEL_EDISON_MINIBOARD_J17_7 = 6,
            MRAA_INTEL_EDISON_MINIBOARD_J17_8 = 7,
            MRAA_INTEL_EDISON_MINIBOARD_J17_9 = 8,
            MRAA_INTEL_EDISON_MINIBOARD_J17_10 = 9,
            MRAA_INTEL_EDISON_MINIBOARD_J17_11 = 10,
            MRAA_INTEL_EDISON_MINIBOARD_J17_12 = 11,
            MRAA_INTEL_EDISON_MINIBOARD_J17_14 = 13,
            MRAA_INTEL_EDISON_MINIBOARD_J18_1 = 14,
            MRAA_INTEL_EDISON_MINIBOARD_J18_2 = 15,
            MRAA_INTEL_EDISON_MINIBOARD_J18_6 = 19,
            MRAA_INTEL_EDISON_MINIBOARD_J18_7 = 20,
            MRAA_INTEL_EDISON_MINIBOARD_J18_8 = 21,
            MRAA_INTEL_EDISON_MINIBOARD_J18_10 = 23,
            MRAA_INTEL_EDISON_MINIBOARD_J18_11 = 24,
            MRAA_INTEL_EDISON_MINIBOARD_J18_12 = 25,
            MRAA_INTEL_EDISON_MINIBOARD_J18_13 = 26,
            MRAA_INTEL_EDISON_MINIBOARD_J19_4 = 31,
            MRAA_INTEL_EDISON_MINIBOARD_J19_5 = 32,
            MRAA_INTEL_EDISON_MINIBOARD_J19_6 = 33,
            MRAA_INTEL_EDISON_MINIBOARD_J19_8 = 35,
            MRAA_INTEL_EDISON_MINIBOARD_J19_9 = 36,
            MRAA_INTEL_EDISON_MINIBOARD_J19_10 = 37,
            MRAA_INTEL_EDISON_MINIBOARD_J19_11 = 38,
            MRAA_INTEL_EDISON_MINIBOARD_J19_12 = 39,
            MRAA_INTEL_EDISON_MINIBOARD_J19_13 = 40,
            MRAA_INTEL_EDISON_MINIBOARD_J19_14 = 41,
            MRAA_INTEL_EDISON_MINIBOARD_J20_3 = 44,
            MRAA_INTEL_EDISON_MINIBOARD_J20_4 = 45,
            MRAA_INTEL_EDISON_MINIBOARD_J20_5 = 46,
            MRAA_INTEL_EDISON_MINIBOARD_J20_6 = 47,
            MRAA_INTEL_EDISON_MINIBOARD_J20_7 = 48,
            MRAA_INTEL_EDISON_MINIBOARD_J20_8 = 49,
            MRAA_INTEL_EDISON_MINIBOARD_J20_9 = 50,
            MRAA_INTEL_EDISON_MINIBOARD_J20_10 = 51,
            MRAA_INTEL_EDISON_MINIBOARD_J20_11 = 52,
            MRAA_INTEL_EDISON_MINIBOARD_J20_12 = 53,
            MRAA_INTEL_EDISON_MINIBOARD_J20_13 = 54,
            MRAA_INTEL_EDISON_MINIBOARD_J20_14 = 55
        }

        /**
         * Intel Edison raw GPIO numbering enum
         */
        export enum mraa_intel_edison_t{
            MRAA_INTEL_EDISON_GP182 = 0,
            MRAA_INTEL_EDISON_GP135 = 4,
            MRAA_INTEL_EDISON_GP27 = 6,
            MRAA_INTEL_EDISON_GP20 = 7,
            MRAA_INTEL_EDISON_GP28 = 8,
            MRAA_INTEL_EDISON_GP111 = 0,
            MRAA_INTEL_EDISON_GP109 = 10,
            MRAA_INTEL_EDISON_GP115 = 11,
            MRAA_INTEL_EDISON_GP128 = 13,
            MRAA_INTEL_EDISON_GP13 = 14,
            MRAA_INTEL_EDISON_GP165 = 15,
            MRAA_INTEL_EDISON_GP19 = 19,
            MRAA_INTEL_EDISON_GP12 = 20,
            MRAA_INTEL_EDISON_GP183 = 21,
            MRAA_INTEL_EDISON_GP110 = 23,
            MRAA_INTEL_EDISON_GP114 = 24,
            MRAA_INTEL_EDISON_GP129 = 25,
            MRAA_INTEL_EDISON_GP130 = 26,
            MRAA_INTEL_EDISON_GP44 = 31,
            MRAA_INTEL_EDISON_GP46 = 32,
            MRAA_INTEL_EDISON_GP48 = 33,
            MRAA_INTEL_EDISON_GP131 = 35,
            MRAA_INTEL_EDISON_GP14 = 36,
            MRAA_INTEL_EDISON_GP40 = 37,
            MRAA_INTEL_EDISON_GP43 = 38,
            MRAA_INTEL_EDISON_GP77 = 39,
            MRAA_INTEL_EDISON_GP82 = 40,
            MRAA_INTEL_EDISON_GP83 = 41,
            MRAA_INTEL_EDISON_GP134 = 44,
            MRAA_INTEL_EDISON_GP45 = 45,
            MRAA_INTEL_EDISON_GP47 = 46,
            MRAA_INTEL_EDISON_GP49 = 47,
            MRAA_INTEL_EDISON_GP15 = 48,
            MRAA_INTEL_EDISON_GP84 = 49,
            MRAA_INTEL_EDISON_GP42 = 50,
            MRAA_INTEL_EDISON_GP41 = 51,
            MRAA_INTEL_EDISON_GP78 = 52,
            MRAA_INTEL_EDISON_GP79 = 53,
            MRAA_INTEL_EDISON_GP80 = 54,
            MRAA_INTEL_EDISON_GP81 = 55
        }

        /**
         * MRAA return codes
         */
        export enum mraa_result_t{
            MRAA_SUCCESS = 0, /**< Expected response */
            MRAA_ERROR_FEATURE_NOT_IMPLEMENTED = 1, /**< Feature TODO */
            MRAA_ERROR_FEATURE_NOT_SUPPORTED = 2, /**< Feature not supported by HW */
            MRAA_ERROR_INVALID_VERBOSITY_LEVEL = 3, /**< Verbosity level wrong */
            MRAA_ERROR_INVALID_PARAMETER = 4, /**< Parameter invalid */
            MRAA_ERROR_INVALID_HANDLE = 5, /**< Handle invalid */
            MRAA_ERROR_NO_RESOURCES = 6, /**< No resource of that type avail */
            MRAA_ERROR_INVALID_RESOURCE = 7, /**< Resource invalid */
            MRAA_ERROR_INVALID_QUEUE_TYPE = 8, /**< Queue type incorrect */
            MRAA_ERROR_NO_DATA_AVAILABLE = 9, /**< No data available */
            MRAA_ERROR_INVALID_PLATFORM = 10, /**< Platform not recognised */
            MRAA_ERROR_PLATFORM_NOT_INITIALISED = 11, /**< Board information not initialised */
            MRAA_ERROR_PLATFORM_ALREADY_INITIALISED = 12, /**< Board is already initialised */

            MRAA_ERROR_UNSPECIFIED = 99 /**< Unknown Error */
        }

        /**
         * Enum representing different possible modes for a pin.
         */
        export enum mraa_pinmodes_t{
            MRAA_PIN_VALID = 0, /**< Pin Valid */
            MRAA_PIN_GPIO = 1, /**< General Purpose IO */
            MRAA_PIN_PWM = 2, /**< Pulse Width Modulation */
            MRAA_PIN_FAST_GPIO = 3, /**< Faster GPIO */
            MRAA_PIN_SPI = 4, /**< SPI */
            MRAA_PIN_I2C = 5, /**< I2C */
            MRAA_PIN_AIO = 6, /**< Analog in */
            MRAA_PIN_UART = 7  /**< UART */
        }

        /**
         * Enum reprensenting different i2c speeds/modes
         */
        export enum mraa_i2c_mode_t {
            MRAA_I2C_STD = 0, /**< up to 100Khz */
            MRAA_I2C_FAST = 1, /**< up to 400Khz */
            MRAA_I2C_HIGH = 2  /**< up to 3.4Mhz */
        }


        /**
         * Gpio Output modes
         */
        export enum Mode{
            MODE_STRONG = 0, /**< Default. Strong High and Low */
            MODE_PULLUP = 1, /**< Interupt on rising & falling */
            MODE_PULLDOWN = 2, /**< Interupt on rising only */
            MODE_HIZ = 3  /**< Interupt on falling only */
        }

        /**
         * Gpio Direction options
         */
        export enum Dir{
            DIR_OUT = 0, /**< Output. A Mode can also be set */
            DIR_IN = 1, /**< Input */
            DIR_OUT_HIGH = 2, /**< Output. Init High */
            DIR_OUT_LOW = 3  /**< Output. Init Low */
        }

        /**
         * Gpio Edge types for interupts
         */
        export enum Edge{
            EDGE_NONE = 0, /**< No interrupt on Gpio */
            EDGE_BOTH = 1, /**< Interupt on rising & falling */
            EDGE_RISING = 2, /**< Interupt on rising only */
            EDGE_FALLING = 3  /**< Interupt on falling only */
        }

        export enum Spi_Mode{
            SPI_MODE0 = 0, /**< CPOL = 0, CPHA = 0, Clock idle low, data is clocked in on rising edge, output data (change) on falling edge */
            SPI_MODE1 = 1, /**< CPOL = 0, CPHA = 1, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge */
            SPI_MODE2 = 2, /**< CPOL = 1, CPHA = 0, Clock idle low, data is clocked in on falling edge, output data (change) on rising edge */
            SPI_MODE3 = 3, /**< CPOL = 1, CPHA = 1, Clock idle low, data is clocked in on rising, edge output data (change) on falling edge */
        }


        function getVersion():string;
        function setPriority(priority:number):number

        function getPlatformType():mraa_platform_t

        function printError(result:mraa_result_t)

        function pinModeTest(pin:number, mode:mraa_pinmodes_t):boolean

        function adcRawBits():number

        function adcSupportedBits():number

        function getPlatformName():string

        function setLogLevel(level:number):mraa_result_t

        export class Gpio {
            constructor(pin:number);
            constructor(pin:number, owner:boolean, raw:boolean);
            edge(mode:Edge):mraa_result_t;
            //      isr(Edge mode, void (*fptr)(void *),  args:any):mraa_result_t
            isrExit():mraa_result_t;
            mode(mode:Mode):mraa_result_t;
            dir(mode:Dir):mraa_result_t;
            read():number;
            write(value:number):mraa_result_t;
            useMmap(enable:boolean):mraa_result_t;
            getPin():number;
        }
        export class Aio {
            /**
             * Aio Constructor, takes a pin number which will map directly to the
             * board number
             *
             * @param pin channel number to read ADC inputs
             */
            constructor(pin:number);
            /**
             * Read a value from the AIO pin. By default mraa will shift
             * the raw value up or down to a 10 bit value.
             *
             * @returns The current input voltage. By default, a 10bit value
             */
            read():number;
            /**
             * Set the bit value which mraa will shift the raw reading
             * from the ADC to. I.e. 10bits
             * @param bits the bits the return from read should be i.e 10
             * @return mraa result type
             */
            setBit(bits:number):mraa_result_t ;
            /**
             * Gets the bit value mraa is shifting the analog read to.
             *
             * @return bit value mraa is set return from the read function
             */
            getBit():number  ;
        }

        export class I2C {
            constructor(bus:number, raw:number);
            constructor(bus:number);


            /**
             * Sets the i2c Frequency for communication. Your board may not support
             * the set frequency. Anyone can change this at any time and this will
             * affect every slave on the bus
             *
             * @param mode Frequency to set the bus to
             * @return Result of operation
             */
            frequency(mode:mraa_i2c_mode_t):mraa_result_t;

            /**
             * Set the slave to talk to, typically called before every read/write
             * operation
             *
             * @param address Communicate to the i2c slave on this address (uint8_t)
             * @return Result of operation
             */
            address(address:number):mraa_result_t;

            /**
             * Read exactly one byte from the bus
             *
             * @return char read from the bus
             */
            /*uint8_t */
            readByte():number;

            /**
             * Read length bytes from the bus into *data pointer
             *
             * @param data Data to read into
             * @param length Size of read in bytes to make
             * @return length of read, should match length
             */
            read(data:number, length:number):number;

            /**
             * Read byte from an i2c register
             *
             * @param reg Register to read from
             * @return char read from register
             */
            readReg(reg:number):number;

            /**
             * Read word from an i2c register
             *
             * @param reg Register to read from
             * @return char read from register
             */
            readWordReg(/*uint8_t*/ reg:number):number/*uint16_t*/;

            /**
             * Write a byte on the bus
             *
             * @param data The byte to send on the bus
             * @return Result of operation
             */
            writeByte(data:number):mraa_result_t ;

            /**
             * Write length bytes to the bus, the first byte in the array is the
             * command/register to write
             *
             * @param data Buffer to send on the bus, first byte is i2c command
             * @param length Size of buffer to send
             * @return Result of operation
             */
            write(data:number, length:number):mraa_result_t;

            /**
             * Write a byte to an i2c register
             *
             * @param reg Register to write to
             * @param data Value to write to register
             * @return Result of operation
             */
            writeReg(reg:number, data:number):mraa_result_t ;

            /**
             * Write a word to an i2c register
             *
             * @param reg Register to write to
             * @param data Value to write to register
             * @return Result of operation
             */
            writeWordReg(reg:number, data:number):mraa_result_t ;
        }

        export class Pwm {
            /**
             * instanciates a PWM object on a pin
             *
             * @param pin the pin number used on your board
             * @param owner if you are the owner of the pin the destructor will
             * @param chipid the pwmchip to use, use only in raw mode
             * unexport the pin from sysfs, default behaviour is you are the owner
             * if the pinmapper exported it
             */
            constructor(pin:number, owner:boolean, chipid:number);
            constructor(pin:number);

            /**
             * Set the output duty-cycle percentage, as a float
             *
             * @param percentage A floating-point value representing percentage of
             * output. The value should lie between 0.0f (representing on 0%) and
             * 1.0f Values above or below this range will be set at either 0.0f or
             * 1.0f
             * @return Result of operation
             */
            write(percentage:number):mraa_result_t ;
            /**
             * Read the ouput duty-cycle percentage, as a float
             *
             * @return A floating-point value representing percentage of
             * output. The value should lie between 0.0f (representing on 0%) and
             * 1.0f Values above or below this range will be set at either 0.0f or
             * 1.0f
             */
            read():number;
            /**
             * Set the PWM period as seconds represented in a float
             *
             * @param period Period represented as a float in seconds
             * @return Result of operation
             */
            period(period:number):mraa_result_t;
            /**
             * Set period, milliseconds
             *
             * @param ms milliseconds for period
             * @return Result of operation
             */
            period_ms(ms:number):mraa_result_t ;
            /**
             * Set period, microseconds
             *
             * @param us microseconds as period
             * @return Result of operation
             */
            period_us(us:number):mraa_result_t ;
            /**
             * Set pulsewidth, As represnted by seconds in a (float)
             *
             * @param seconds The duration of a pulse
             * @return Result of operation
             */
            pulsewidth(seconds:number):mraa_result_t;
            /**
             * Set pulsewidth, milliseconds
             *
             * @param ms milliseconds for pulsewidth
             * @return Result of operation
             */
            pulsewidth_ms(ms:number):mraa_result_t ;
            /**
             * The pulsewidth, microseconds
             *
             * @param us microseconds for pulsewidth
             * @return Result of operation
             */
            pulsewidth_us(us:number):mraa_result_t ;
            /**
             * Set the enable status of the PWM pin. None zero will assume on with
             * output being driven and 0 will disable the output
             *
             * @param enable enable status of pin
             * @return Result of operation
             */
            enable(enable:boolean):mraa_result_t;
            /**
             * Set the period and duty of a PWM object.
             *
             * @param period represented in ms.
             * @param duty represnted in ms as float.
             * @return Result of operation
             */
            config_ms(period:number, duty:number):mraa_result_t;
            /**
             * Set the period and duty (percent) of a PWM object.
             *
             * @param period as represented in ms.
             * @param duty percentage i.e. 50% = 0.5f
             * @return Result of operation
             */
            config_percent(period:number, duty:number):mraa_result_t;
        }


        export class Spi {
            /**
             * Initialise SPI object using the board mapping to set muxes
             *
             * @param bus to use, as listed in the platform definition, normally 0
             */
            constructor(bus:number) ;

            /**
             * Set the SPI device mode. see spidev0-3
             *
             * @param mode the mode. See Linux spidev doc
             * @return Result of operation
             */
            mode(mode:Spi_Mode):mraa_result_t ;

            /**
             * Set the SPI device operating clock frequency
             *
             * @param hz the frequency to set in hz
             * @return Result of operation
             */
            frequency(hz:number):mraa_result_t ;

            /**
             * Write single byte to the SPI device
             *
             * @param data the byte to send
             * @return data received on the miso line
             */
            writeByte(data:number):number ;

            /**
             * Write buffer of bytes to SPI device The pointer return has to be
             * free'd by the caller. It will return a NULL pointer in cases of
             * error
             *
             * @param txBuf buffer to send
             * @param length size of buffer to send
             * @return uint8_t* data received on the miso line. Same length as passed in
             */
            write(txBuf:number, length:number):number;

            /**
             * Transfer data to and from SPI device Receive pointer may be null if
             * return data is not needed.
             *
             * @param data buffer to send
             * @param rxBuf buffer to optionally receive data from spi device
             * @param length size of buffer to send
             * @return Result of operation
             */
            transfer(txBuf:number, rxBuf:number, length:number):mraa_result_t;

            /**
             * Change the SPI lsb mode
             *
             * @param lsb Use least significant bit transmission - 0 for msbi
             * @return Result of operation
             */
            lsbmode(lsb:boolean):mraa_result_t;

            /**
             * Set bits per mode on transaction, default is 8
             *
             * @param bits bits per word
             * @return Result of operation
             */
            bitPerWord(bits:number):mraa_result_t ;

        }
        /**
         * @brief API to UART (enabling only)
         *
         * This file defines the UART interface for libmraa
         */
        export class Uart {
            /**
             * Uart Constructor, takes a pin number which will map directly to the
             * linux uart number, this 'enables' the uart, nothing more
             *
             * @param uart the index of the uart set to use
             */
            constructor(uart:number);
        }


    }
    export = mraa;
}

