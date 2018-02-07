set_source_files_properties (mraa2.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (mraa2.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

message (STATUS "PYTHON2 attempting to build!")

if (CMAKE_VERSION VERSION_LESS "3.8")
  swig_add_module (python2-mraa python mraa2.i ../mraapy.c)
else ()
  swig_add_library (python2-mraa LANGUAGE python SOURCES mraa2.i ../mraapy.c)
endif ()
swig_link_libraries (python2-mraa ${PYTHON2_LIBRARIES} mraa)

target_include_directories(${SWIG_MODULE_python2-mraa_REAL_NAME}
  PUBLIC
  "${CMAKE_CURRENT_SOURCE_DIR}/../.."
  "${PYTHON2_INCLUDE_DIR}"
)

if (DOXYGEN_FOUND AND PYTHON2_EXECUTABLE)
  foreach (_file ${DOCCLASSES})
	add_dependencies (${SWIG_MODULE_python2-mraa_REAL_NAME} ${_file}class_doc_i)
  endforeach ()
  add_dependencies (${SWIG_MODULE_python2-mraa_REAL_NAME} common_hpp_doc_i)
endif ()

set_target_properties (${SWIG_MODULE_python2-mraa_REAL_NAME} PROPERTIES
  OUTPUT_NAME _mraa
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPYTHON=${SWIG_FOUND} -DSWIGPYTHON2=${SWIG_FOUND}"
)

install (TARGETS ${SWIG_MODULE_python2-mraa_REAL_NAME}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON2_PACKAGES_PATH}
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mraa.py
  DESTINATION ${CMAKE_INSTALL_PREFIX}/${PYTHON2_PACKAGES_PATH}
)

add_subdirectory (docs)
