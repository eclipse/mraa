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

add_subdirectory (docs)
