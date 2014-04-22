add_executable (i2c_HMC5883L i2c_HMC5883L.cpp)
add_executable (hellomaa hellomaa.cpp)
add_executable (cycle-pwm3 cycle-pwm3.cpp)

include_directories(${PROJECT_SOURCE_DIR}/api ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (hellomaa maa)
target_link_libraries (i2c_HMC5883L maa)
target_link_libraries (cycle-pwm3 maa)
