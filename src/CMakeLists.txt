include_directories(
  ${PROJECT_SOURCE_DIR}/api
  ${PROJECT_SOURCE_DIR}/include
)

set (maa_LIB_HEADERS
  ${PROJECT_SOURCE_DIR}/api/maa.h
  ${PROJECT_SOURCE_DIR}/api/i2cslave.h
  ${PROJECT_SOURCE_DIR}/api/i2c.h
  ${PROJECT_SOURCE_DIR}/api/pwm.h
  ${PROJECT_SOURCE_DIR}/api/gpio.h
  ${PROJECT_SOURCE_DIR}/include/smbus.hpp
  ${PROJECT_SOURCE_DIR}/include/smbus.h
)

set (maa_LIB_KERNEL
  ${PROJECT_SOURCE_DIR}/include/linux/compiler.h
  ${PROJECT_SOURCE_DIR}/include/linux/i2c-dev.h
  ${PROJECT_SOURCE_DIR}/include/linux/posix_types.h
  ${PROJECT_SOURCE_DIR}/include/linux/types.h
)

set (maa_LIB_SRCS
  ${PROJECT_SOURCE_DIR}/src/maa.cxx
  ${PROJECT_SOURCE_DIR}/src/i2c/i2c.cxx
  ${PROJECT_SOURCE_DIR}/src/i2c/i2cslave.cxx
  ${PROJECT_SOURCE_DIR}/src/i2c/smbus.c
  ${PROJECT_SOURCE_DIR}/src/gpio/gpio.c
  ${PROJECT_SOURCE_DIR}/src/pwm/pwm.cxx
)

add_library (maa SHARED ${maa_LIB_SRCS})

set_target_properties(                                                                                                                                         
   maa
   PROPERTIES
   SOVERSION ${maa_VERSION_MAJOR}
   VERSION ${maa_VERSION_STRING}
)
install (FILES ${maa_LIB_HEADERS} DESTINATION include/maa)
install (FILES ${maa_LIB_KERNEL} DESTINATION include/maa/linux)

macro(maa_CREATE_INSTALL_PKGCONFIG generated_file install_location)
  configure_file(${generated_file}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} DESTINATION ${install_location})
endmacro(maa_CREATE_INSTALL_PKGCONFIG)
maa_create_install_pkgconfig(maa.pc lib${LIB_SUFFIX}/pkgconfig)

install (TARGETS maa DESTINATION lib)

add_subdirectory (python)
add_subdirectory (javascript)
