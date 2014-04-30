find_path (NODE_ROOT_DIR "node/node.h")

set (NODE_INCLUDE_DIRS
  ${NODE_ROOT_DIR}/src 
  ${NODE_ROOT_DIR}/node
  ${NODE_ROOT_DIR}/deps/v8/include
  ${NODE_ROOT_DIR}/deps/uv/include
)

include_directories (
  ${NODE_INCLUDE_DIRS}                                                                                                                                                        
  /usr/include/node
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

set_source_files_properties (maajs.i PROPERTIES SWIG_FLAGS "-node")
set_source_files_properties (maajs.i PROPERTIES CPLUSPLUS ON)

swig_add_module (maajs javascript maajs.i ${maa_LIB_SRCS})
swig_link_libraries (maajs ${NODE_LIBRARIES})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCFILES})
    add_dependencies (${SWIG_MODULE_maajs_REAL_NAME} ${_file}doc_i)
  endforeach ()
endif ()

set_target_properties (maajs PROPERTIES
  PREFIX ""
  SUFFIX ".node"                                                                                                                                                                  
)
