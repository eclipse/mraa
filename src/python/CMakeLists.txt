FIND_PACKAGE(PythonLibs)
INCLUDE_DIRECTORIES(${PYTHON_INCLUDE_PATH})

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${PYTHON_INCLUDE_DIRS}
)

SET_SOURCE_FILES_PROPERTIES(pymaa.i PROPERTIES CPLUSPLUS ON)                                                                                                 

SWIG_ADD_MODULE(pymaa python pymaa.i ../maa.cxx ../i2c/i2c.cxx)
SWIG_LINK_LIBRARIES(pymaa ${PYTHON_LIBRARIES})
