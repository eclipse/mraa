JSON platform {#jsonplat}
=============

JSON platform loading allows libmraa users who work with **simple** and unsupported
boards to create JSON configuration files which libmraa understands, allowing
libmraa to just 'work' with the unsupported platform. JSON platform **does not work**
with boards which require advanced functions which overwrite default functionality or
boards that require the mux functionality.

Syntax
------

The JSON file that libmraa reads needs specific values to be set by default, they
are required in every JSON file intended to be used to load the platform.
The keywords are case sensitive so be aware when writing the JSON file.
Each heading is a top level key in the JSON file, each key's type is Array

### platform
|Key          |Type   |Required   |Description                                    |
|-------------|-------|-----------|-----------------------------------------------|
|id           |int    |no         | An ID that can be assigned                    |
|name         |string |yes        | The platform name                             |
|pin_count    |int    |yes        | The total pin count                           |
|gpio_count   |int    |no         | The subset of pincount which able to do GPIO  |
|i2c_count    |int    |no         | The number of usable I2C ports                |
|aio_count    |int    |no         | The number of usable AIO ports                |
|spi_count    |int    |no         | The number of usable SPI ports                |
|uart_count   |int    |no         | The total UART ports                          |
|pwmDefPeriod |int    |no         | The default PWM period                        |
|pwmMaxPeriod |int    |no         | The max PWM period                            |
|pwmMinPeriod |int    |no         | The min PWM period                            |

### layout

**THIS INDEX'S THE PIN ARRAY**
Each available pin specified using pin_count requires a label. There is no need to use an
index identifier as the json objects are indexed as they are read, starting with the first
json object being treated as index 0.

The number of json objects in layout should be equal to the pin_count.

|Key        |Type   |Required   |Description                                    |
|-----------|-------|-----------|-----------------------------------------------|
|label      |string |yes        | The label used to describe the pin (11 characters max) |
|invalid    |boolean|no         | Sets the labeled pin as an invalid pin        |

### GPIO

**THIS INDEX'S THE PIN ARRAY**
For the supported GPIO pins here is where we set the raw linux pin it needs to use, the
index cannot exceed the total pin_count and the number of JSON ojects cannot exceed the
gpio_count total. The index is also used to set the specified pin's capabilities. Each set
of keys should be enclosed in a json object.

|Key        |Type   |Required   |Description                          |
|-----------|-------|-----------|-------------------------------------|
|index      |int    |yes        | used to index the pin array         |
|rawpin     |int    |yes        | used to reference the actual IO pin |

### I2C

**THIS INDEX'S THE I2C ARRAY**
This is used to setup the I2C functionality. The total number of json objects and the index
cannot exceed the i2c_count. Each set of keys should be enclosed in a json object.

|Key        |Type   |Required   |Description                          |
|-----------|-------|-----------|-------------------------------------|
|index      |int    |yes        | Used to index the I2C array         |
|rawpin     |int    |yes        | The sysfs pin                       |
|sclpin     |int    |no         | The clock pin                       |
|sdapin     |int    |no         | The data pin                        |
|default    |boolean|no         | Sets the default I2C device used    |

### AIO

**THIS INDEX'S THE PIN ARRAY**
This is used to setup which pins are able to perform AIO. Each set of keys should be
enclosed in a json object.

|Key        |Type   |Required   |Description                          |
|-----------|-------|-----------|-------------------------------------|
|index      |int    |yes        | Used to index the pin array         |
|rawpin     |int    |yes        | The sysfs pin                       |

### PWM

**THIS INDEX'S THE PIN ARRAY**
This is used to setup which IO pins can do PWM Each set of keys should be enclosed in a
json object.

|Key        |Type   |Required   |Description                    |
|-----------|-------|-----------|-------------------------------|
|index      |int    |yes        | Used to index the pin array   |
|rawpin     |int    |yes        | The sysfs pin                 |
|chipID     |int    |yes        | The parent ID                 |

## SPI

**THIS INDEX'S THE SPI ARRAY**
This is used to setup the SPI functionality. The total number of json objects and the index
cannot exceed the spi_count. Each set of keys should be enclosed in a json object.

|Key        |Type   |Required   |Description                                 |
|-----------|-------|-----------|--------------------------------------------|
|index      |int    |yes        | The index into the SPI array               |
|chipID     |int    |yes        | The parent ID                              |
|clock      |int    |no         | The clock used for transmissions           |
|miso       |int    |no         | Pin used for incoming data from the slave  |
|mosi       |int    |no         | Pin used for outgoing data from the master |
|chipselect |int    |no         | Pin used to select the slave device        |
|default    |boolean|no         | Sets the default SPI device                |

### UART

**THIS INDEX'S THE UART ARRAY**
This is used to setup the UART functionality. The total number of json objects and the index
cannot exceed the uart_count. Each set of keys should be enclosed in a json object.

|Key        |Type   |Required   |Description                              |
|-----------|-------|-----------|-----------------------------------------|
|index      |int    |yes        | Used to index the UART array            |
|chipID     |int    |yes        | Parent ID                               |
|rawpin     |int    |yes        | Sysfs pin                               |
|rx         |int    |no         | Read pin                                |
|tx         |int    |no         | Transmit pin                            |
|path       |string |yes        | Used to talk to a connected UART device |
|default    |boolean|no         | Sets the default UART device            |
