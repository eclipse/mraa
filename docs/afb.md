-DBUILDARCH="AFB"
./afb-daemon --binding="/home/brendan/git/af-mraa-binding/build/src/af-mraa-binding.so" --verbose --port=1521 --token=x --ldpaths=. --sessiondir=/tmp --rootdir=/home/brendan/agl --alias=icons:/usr/share/icons 

./afb-client-demo -H ws://localhost:1521/api\?token=x mraa dev-init "[ 'mraa_i2c_init', 0 ]" 
