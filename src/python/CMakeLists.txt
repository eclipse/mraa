FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PYTHON_INCLUDE_DIRS}
)

swig_add_module (pymaa python pymaa.i ${maa_LIB_SRCS})
swig_link_libraries (pymaa ${PYTHON_LIBRARIES})

set (CMAKE_C_FLAGS -DSWIG=${SWIG_FOUND})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCFILES})
    add_dependencies (${SWIG_MODULE_pymaa_REAL_NAME} ${_file}doc_i)
  endforeach ()

  add_custom_target (pydoc
    pydoc -w ${CMAKE_CURRENT_BINARY_DIR}/pymaa.py ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with pydoc" VERBATIM
  )
endif ()

add_subdirectory (docs)
