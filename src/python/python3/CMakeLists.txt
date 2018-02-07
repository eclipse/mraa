set_source_files_properties (mraa3.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (mraa3.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

message (STATUS "PYTHON3 attempting to build!")

if (CMAKE_VERSION VERSION_LESS "3.8")
  swig_add_module (python3-mraa python mraa3.i ../mraapy.c)
else ()
  swig_add_library (python3-mraa LANGUAGE python SOURCES mraa3.i ../mraapy.c)
endif ()
swig_link_libraries (python3-mraa ${PYTHON3_LIBRARIES} mraa)

target_include_directories(${SWIG_MODULE_python3-mraa_REAL_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../.."
  "${PYTHON3_INCLUDE_DIR}"
)

set_target_properties (${SWIG_MODULE_python3-mraa_REAL_NAME} PROPERTIES
  OUTPUT_NAME _mraa
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPYTHON=${SWIG_FOUND} -DSWIGPYTHON3=${SWIG_FOUND}"
)

install (TARGETS ${SWIG_MODULE_python3-mraa_REAL_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON3_PACKAGES_PATH}
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mraa.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON3_PACKAGES_PATH}
)
