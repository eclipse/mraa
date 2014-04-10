include_directories(
  ${PROJECT_SOURCE_DIR}/api
  ${PROJECT_SOURCE_DIR}/include
)

set (maa_LIB_SRCS
  ${PROJECT_SOURCE_DIR}/src/maa.cxx
  ${PROJECT_SOURCE_DIR}/src/i2c/i2c.cxx
  ${PROJECT_SOURCE_DIR}/src/i2c/smbus.c
  ${PROJECT_SOURCE_DIR}/src/gpio/gpio.c
)

add_library (maa STATIC ${maa_LIB_SRCS})

add_subdirectory(python)
add_subdirectory(javascript)
