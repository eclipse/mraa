Mraa with AFB                         {#afb}
=============

libmraa needs to be built with the BUILDARCH AFB on the client side. The server
side needs to be a normal mraa built for your architecture.

Buildarch is stated like this in the cmake build options:
-DBUILDARCH="AFB"

## Adding the af-mraa-binding to the afb-daemon

./afb-daemon --binding="/home/brendan/git/af-mraa-binding/build/src/af-mraa-binding.so" --verbose --port=1521 --token=x --ldpaths=. --sessiondir=/tmp --rootdir=/home/brendan/agl --alias=icons:/usr/share/icons 

## Manually calling a mraa command

./afb-client-demo -H ws://localhost:1521/api\?token=x mraa dev-init "[ 'mraa_i2c_init', 0 ]" 


## Calling mraa directly
