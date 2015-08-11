find_package (PythonLibs ${PYTHONBUILD_VERSION} REQUIRED)

INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PYTHON_INCLUDE_DIRS}
)

set_source_files_properties (mraa.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (mraa.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")
swig_add_module (python-mraa python mraa.i ${mraa_LIB_SRCS})
swig_link_libraries (python-mraa ${PYTHON_LIBRARIES} ${mraa_LIBS})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCCLASSES})
    add_dependencies (${SWIG_MODULE_python-mraa_REAL_NAME} ${_file}class_doc_i)
  endforeach ()
  add_dependencies (${SWIG_MODULE_python-mraa_REAL_NAME} common_hpp_doc_i)

  add_custom_target (pydoc
    pydoc -w ${CMAKE_CURRENT_BINARY_DIR}/mraa.py ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with pydoc" VERBATIM
  )
endif ()

set_target_properties (${SWIG_MODULE_python-mraa_REAL_NAME} PROPERTIES
  OUTPUT_NAME _mraa
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIGPYTHON=${SWIG_FOUND}"
)

execute_process (
   COMMAND ${PYTHON_EXECUTABLE} -c
       "import site, sys; sys.stdout.write(site.PREFIXES[-1])"
   OUTPUT_VARIABLE PYTHON_PREFIX
)
file (TO_CMAKE_PATH "${PYTHON_PREFIX}" PYTHON_PREFIX)
execute_process (
   COMMAND ${PYTHON_EXECUTABLE} -c
       "import site, sys; sys.stdout.write(site.getusersitepackages().replace(site.getuserbase(), site.PREFIXES[-1]))"
   OUTPUT_VARIABLE PYTHON_SITE_DIR
)
file (TO_CMAKE_PATH "${PYTHON_SITE_DIR}" PYTHON_SITE_DIR)
string (REGEX REPLACE "^${PYTHON_PREFIX}/" ""
   PYTHON_SITE_DIR "${PYTHON_SITE_DIR}")

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/_mraa.so
         ${CMAKE_CURRENT_BINARY_DIR}/mraa.py
         DESTINATION ${PYTHON_SITE_DIR})

add_subdirectory (docs)
