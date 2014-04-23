include_directories(
  ${PROJECT_SOURCE_DIR}/api
  ${PROJECT_SOURCE_DIR}/include
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

macro(maa_CREATE_INSTALL_PKGCONFIG generated_file install_location)
  configure_file(${generated_file}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} @ONLY)
  install(FILES ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} DESTINATION ${install_location})
endmacro(maa_CREATE_INSTALL_PKGCONFIG)
maa_create_install_pkgconfig(maa.pc lib${LIB_SUFFIX}/pkgconfig)

install (TARGETS maa DESTINATION lib)

add_subdirectory (python)
add_subdirectory (javascript)
