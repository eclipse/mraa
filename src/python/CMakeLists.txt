FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PYTHON_INCLUDE_DIRS}
)

if (DOXYGEN_FOUND)
  set (CMAKE_SWIG_FLAGS -DDOXYGEN=${DOXYGEN_FOUND})
endif ()

swig_add_module (pymaa python pymaa.i ${maa_LIB_SRCS})
swig_link_libraries (pymaa ${PYTHON_LIBRARIES})

if (DOXYGEN_FOUND)
  set (DOCFILES maa pwm i2c gpio)
  foreach (_file ${DOCFILES})
    add_custom_command (OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/${_file}_doc.i
      COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/doxy2swig.py -n
        ${CMAKE_BINARY_DIR}/xml/${_file}_8h.xml
        ${CMAKE_CURRENT_BINARY_DIR}/${_file}_doc.i
      DEPENDS ${CMAKE_BINARY_DIR}/xml/${_file}_8h.xml
    )

    add_custom_target (${_file}doc_i DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/${_file}_doc.i)
    add_dependencies (${SWIG_MODULE_pymaa_REAL_NAME} ${_file}doc_i)
  endforeach ()

  add_custom_target (pydoc
    pydoc -w ${CMAKE_CURRENT_BINARY_DIR}/pymaa ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with pydoc" VERBATIM
  )
endif ()
