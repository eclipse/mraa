find_path (NODE_ROOT_DIR "node/node.h")

set (NODE_INCLUDE_DIRS
  ${NODE_ROOT_DIR}/src
  ${NODE_ROOT_DIR}/node
  ${NODE_ROOT_DIR}/deps/v8/include
  ${NODE_ROOT_DIR}/deps/uv/include
)

include_directories (
  ${NODE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# SWIG treats SWIG_FLAGS as a list and not a string so semicolon seperation is required
set_source_files_properties (mraajs.i PROPERTIES SWIG_FLAGS "-node;-I${CMAKE_BINARY_DIR}/src")
set_source_files_properties (mraajs.i PROPERTIES CPLUSPLUS ON)

swig_add_module (mraajs javascript mraajs.i ${mraa_LIB_SRCS})
swig_link_libraries (mraajs ${NODE_LIBRARIES})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCFILES})
    add_dependencies (${SWIG_MODULE_mraajs_REAL_NAME} ${_file}doc_i)
  endforeach ()
endif ()

set_target_properties (mraajs PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_NODE_EXTENSION"
  PREFIX ""
  OUTPUT_NAME mraa
  SUFFIX ".node"
)

install (FILES ${CMAKE_CURRENT_SOURCE_DIR}/package.json
         ${CMAKE_CURRENT_BINARY_DIR}/mraa.node
         DESTINATION lib/node_modules/mraa)
