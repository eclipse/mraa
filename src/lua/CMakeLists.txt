find_package (Lua REQUIRED)

include_directories(
  ${CMAKE_CURRENT_SOURCE_DIR}/..
  ${LUA_INCLUDE_DIR}
)

set_source_files_properties (mraa.i PROPERTIES CPLUSPLUS ON)
set_source_files_properties (mraa.i PROPERTIES SWIG_FLAGS "-I${CMAKE_BINARY_DIR}/src")
swig_add_module (lua-mraa lua mraa.i)
swig_link_libraries (lua-mraa mraa ${LUA_LIBRARIES})

if (DOXYGEN_FOUND)
  foreach (_file ${DOCCLASSES})
    add_dependencies (${SWIG_MODULE_lua-mraa_REAL_NAME} ${_file}class_doc_i)
  endforeach ()
  add_dependencies (${SWIG_MODULE_lua-mraa_REAL_NAME} common_hpp_doc_i)

  add_custom_target (pydoc
    pydoc -w ${CMAKE_CURRENT_BINARY_DIR}/mraa.py ${CMAKE_CURRENT_BINARY_DIR}/
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    COMMENT "Generating API documentation with pydoc" VERBATIM
  )
endif ()

set_target_properties (${SWIG_MODULE_lua-mraa_REAL_NAME} PROPERTIES
  OUTPUT_NAME mraa
  COMPILE_FLAGS "${CMAKE_C_FLAGS} -DSWIG=${SWIG_FOUND}"
)

execute_process (
  COMMAND lua -e
   "string.gsub(package.path, '[^;]+', function(e)
      if e:find('/usr/local') == 1 then
        p = e:gsub('(?.lua)', '')
        io.write(p)
        os.exit(0)
       end
    end)
    os.exit(1)
   "
   OUTPUT_VARIABLE LUA_SITE_DIR
)

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mraa.so
         DESTINATION ${LUA_SITE_DIR})

message(STATUS "Lua site dir: ${LUA_SITE_DIR}")

add_subdirectory (docs)
