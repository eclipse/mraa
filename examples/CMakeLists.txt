add_executable (i2c_HMC5883L i2c_HMC5883L.c)
add_executable (hellomaa hellomaa.c)
add_executable (cycle-pwm3 cycle-pwm3.c)
add_executable (blink-io8 blink-io8.c)

include_directories(${PROJECT_SOURCE_DIR}/api ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (hellomaa maa)
target_link_libraries (i2c_HMC5883L maa m)
target_link_libraries (cycle-pwm3 maa)
target_link_libraries (blink-io8 maa)
