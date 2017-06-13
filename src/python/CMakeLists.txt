set_source_files_properties (mraapython.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (mraapython.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")

if (PYTHON2_LIBRARY)
  add_subdirectory (python2)
endif ()
if (PYTHON3_LIBRARY)
  add_subdirectory (python3)
endif ()
