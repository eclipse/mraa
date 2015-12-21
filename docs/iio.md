iio             {#iio}
===
IIO is the kernel's framework for supporting sensors.
## Using dummy_iio driver
For platforms without IIO hardware the iio_dummy driver can be used. You can
add the driver by either rebuilding the kernel with IIO component enabled or
just building the IIO modules and load them.  You must add the following config
fragment. Known to work for kernel 3.19 and later.
<pre><code>
CONFIG_IIO_DUMMY_EVGEN=m
CONFIG_IIO_SIMPLE_DUMMY=m
CONFIG_IIO_SIMPLE_DUMMY_EVENTS=y
CONFIG_IIO_SIMPLE_DUMMY_BUFFER=y
</code></pre>
### Add driver to kernel
Boot with new kernel, then load modules
<pre><code>
$ modprobe iio_dummy
$ modprobe iio_dummy_evgen
</code></pre>
### Load kernel modules
Depending our your kernel config, some of these modules may already be loaded.
<pre><code>
$ insmod drivers/iio/industrialio.ko
$ insmod drivers/iio/kfifo_buf.ko
$ insmod drivers/staging/iio/iio_dummy_evgen.ko
$ insmod drivers/staging/iio/iio_dummy.ko
</code></pre>
## 'RAW' access

Mraa supports raw access

###Channels

###Attributes

###Events

###Triggers

Activate the trigger in /sys/class/iio
$ echo 1 > trigger0/trigger_now


