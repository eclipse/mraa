iio				{#iio}
===

IIO is the kernel's framework for supporting sensors. You may need to recompile
a kernel and add, this is from kernel 4.2:
CONFIG_IIO_DUMMY_EVGEN=m
CONFIG_IIO_SIMPLE_DUMMY=m
CONFIG_IIO_SIMPLE_DUMMY_EVENTS=y
CONFIG_IIO_SIMPLE_DUMMY_BUFFER=y

## 'RAW' access

Mraa supports raw access

###Channels

###Attributes

###Events

###Triggers

## Testing without hardware

Create a dummy iio device
$ modprobe iio_dummy

Create a dummy iio event generator
$modprobe iio_dummy_evgen
