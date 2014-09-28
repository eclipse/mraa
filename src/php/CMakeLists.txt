FIND_PACKAGE (PHPLibs)
INCLUDE_DIRECTORIES(${PHP_INCLUDE_DIRS})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PHP_INCLUDE_DIRS}
)

set_source_files_properties (php-mraa.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (php-mraa.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")
swig_add_module (php-mraa php php-mraa.i ${mraa_LIB_SRCS})
swig_link_libraries (php-mraa ${PHP_LIBRARIES})

set_target_properties (${SWIG_MODULE_php-mraa_REAL_NAME} PROPERTIES
  OUTPUT_NAME _mraa
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPHP=${SWIG_FOUND}"
)
