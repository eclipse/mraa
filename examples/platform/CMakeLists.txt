add_executable (gpio_edison gpio_edison.c)
target_link_libraries (gpio_edison mraa)

add_executable (up2-leds up2-leds.cpp)
target_link_libraries (up2-leds mraa)

include_directories(${PROJECT_SOURCE_DIR}/api)
include_directories(${PROJECT_SOURCE_DIR}/api/mraa)

if (FIRMATA)
  add_executable (firmata_curie_imu firmata_curie_imu.c)
  add_executable (i2c_firmata i2c_firmata.c)
  target_link_libraries (firmata_curie_imu mraa)
  target_link_libraries (i2c_firmata mraa)
endif()
