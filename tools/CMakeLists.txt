add_executable (mraa-gpio mraa-gpio.c)
add_executable (mraa-i2c mraa-i2c.c)
add_executable (mraa-uart mraa-uart.c)

include_directories (${PROJECT_SOURCE_DIR}/api)
# FIXME Hack to access mraa internal types used by mraa-i2c
include_directories (${PROJECT_SOURCE_DIR}/include)
include_directories (${PROJECT_SOURCE_DIR}/api/mraa)

target_link_libraries (mraa-gpio mraa)
target_link_libraries (mraa-i2c mraa)
target_link_libraries (mraa-uart mraa)

if (INSTALLTOOLS)
  install (TARGETS mraa-gpio DESTINATION bin)
  install (TARGETS mraa-i2c DESTINATION bin)
  install (TARGETS mraa-uart DESTINATION bin)
endif()
