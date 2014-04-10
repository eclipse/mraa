FIND_PATH(NODE_ROOT_DIR "node/node.h")

set(NODE_INCLUDE_DIRS
  ${NODE_ROOT_DIR}/src 
  ${NODE_ROOT_DIR}/node
  ${NODE_ROOT_DIR}/deps/v8/include
  ${NODE_ROOT_DIR}/deps/uv/include
)

include_directories(
  ${NODE_INCLUDE_DIRS}                                                                                                                                                        
  /usr/include/node
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

SET_SOURCE_FILES_PROPERTIES(maajs.i PROPERTIES CPLUSPLUS ON)                                                                                                 
SET_SOURCE_FILES_PROPERTIES(maajs.i PROPERTIES SWIG_FLAGS "-node")

SWIG_ADD_MODULE(maajs javascript ${maa_LIB_SRCS})
SWIG_LINK_LIBRARIES(maajs ${NODE_LIBRARIES})

set_target_properties(maajs PROPERTIES
  PREFIX ""
  SUFFIX ".node"                                                                                                                                                                  
)
