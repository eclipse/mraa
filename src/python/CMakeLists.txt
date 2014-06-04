FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PYTHON_INCLUDE_DIRS}
)

set_source_files_properties (pymaa.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (pymaa.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")
swig_add_module (pymaa python pymaa.i ${maa_LIB_SRCS})
swig_link_libraries (pymaa ${PYTHON_LIBRARIES})

set (CMAKE_C_FLAGS -DSWIGPYTHON=${SWIG_FOUND})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCCLASSES})
    add_dependencies (${SWIG_MODULE_pymaa_REAL_NAME} ${_file}class_doc_i)
  endforeach ()
  add_dependencies (${SWIG_MODULE_pymaa_REAL_NAME} maadoc_i)

  add_custom_target (pydoc
    pydoc -w ${CMAKE_CURRENT_BINARY_DIR}/pymaa.py ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with pydoc" VERBATIM
  )
endif ()

# Essentially do seperate_arguments but with "." instead of " "
string (REPLACE "." ";" PYTHON_VERSION_LIST ${PYTHONLIBS_VERSION_STRING})
list (GET PYTHON_VERSION_LIST 0 PYTHON_VERSION_MAJOR)
list (GET PYTHON_VERSION_LIST 1 PYTHON_VERSION_MINOR)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/_pymaa.so
         ${CMAKE_CURRENT_BINARY_DIR}/pymaa.py
         DESTINATION lib/python${PYTHON_VERSION_MAJOR}.${PYTHON_VERSION_MINOR}/site-packages/)

add_subdirectory (docs)
