add_executable (i2c_HMC5883L i2c_HMC5883L.cxx)
add_executable (hellomaa hellomaa.cxx)
add_executable (cycle-pwm3 cycle-pwm3.cxx)

include_directories(${PROJECT_SOURCE_DIR}/api ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (hellomaa maa)
target_link_libraries (i2c_HMC5883L maa)
target_link_libraries (cycle-pwm3 maa)
