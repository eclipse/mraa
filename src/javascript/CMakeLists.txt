find_package (Nodejs)

include_directories (
  ${NODE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# SWIG treats SWIG_FLAGS as a list and not a string so semicolon seperation is
# required. This hardcodes V8_VERSION to be <10 but I assume that's not going
# to be a problem for a little while!
#set_source_files_properties (mraajs.i PROPERTIES SWIG_FLAGS "-node;-DV8_VERSION=0x0${V8_VERSION_HEX};-I${CMAKE_BINARY_DIR}/src")
#set_source_files_properties (mraajs.i PROPERTIES SWIG_FLAGS "-node;-I${CMAKE_BINARY_DIR}/src")
set_property (SOURCE mraajs.i PROPERTY SWIG_FLAGS "-node"
              "-I${CMAKE_BINARY_DIR}/src" "-DV8_VERSION=0x0${V8_VERSION_MAJOR}${V8_VERSION_MINOR}${V8_VERSION_PATCH}")
set_source_files_properties (mraajs.i PROPERTIES CPLUSPLUS ON)

swig_add_module (mraajs javascript mraajs.i ${mraa_LIB_SRCS})
swig_link_libraries (mraajs ${NODE_LIBRARIES} ${mraa_LIBS})

set_target_properties (mraajs PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_NODE_EXTENSION -DSWIGJAVASCRIPT=${SWIG_FOUND}"
  PREFIX ""
  OUTPUT_NAME mraa
  SUFFIX ".node"
)
macro (mraa_CREATE_INSTALL_PACKAGE_JSON generated_file install_location)
  configure_file (${generated_file}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} @ONLY)
  install (FILES ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} DESTINATION ${install_location})
endmacro (mraa_CREATE_INSTALL_PACKAGE_JSON)
mraa_create_install_package_json (package.json lib/node_modules/mraa)

macro (mraa_CREATE_BINDING_GYP generated_file)
  set (mraa_LIB_SRCS_GYP "")
  set (mraa_NPM_SRCS ${mraa_LIB_SRCS_NOAUTO}
                     ${mraa_LIB_ARM_SRCS_NOAUTO}
                     ${mraa_LIB_X86_SRCS_NOAUTO}
  )
  foreach (srcfile ${mraa_NPM_SRCS})
    file (RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${srcfile})
    set (mraa_LIB_SRCS_GYP "'${rel}',\n${mraa_LIB_SRCS_GYP}")
  endforeach (srcfile)
  foreach (includedir ${mraa_LIB_INCLUDE_DIRS})
    file (RELATIVE_PATH rel ${CMAKE_SOURCE_DIR} ${includedir})
    set (mraa_LIB_INCLUDE_DIRS_GYP "'${rel}',\n${mraa_LIB_INCLUDE_DIRS_GYP}")
  endforeach (includedir)
  configure_file (${generated_file}.cmake ${CMAKE_CURRENT_BINARY_DIR}/${generated_file} @ONLY)
endmacro (mraa_CREATE_BINDING_GYP)
mraa_create_binding_gyp (binding.gyp)

add_custom_target (npmpkg)
add_custom_command (TARGET npmpkg POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/mraajsJAVASCRIPT_wrap.cxx ${CMAKE_SOURCE_DIR}/src
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_BINARY_DIR}/src/version.c ${CMAKE_SOURCE_DIR}/src
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/package.json ${CMAKE_SOURCE_DIR}
        COMMAND sed -i "'s/mraa.node/build\\/Release\\/mraa.node/'"
        ${CMAKE_SOURCE_DIR}/package.json
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_CURRENT_BINARY_DIR}/binding.gyp ${CMAKE_SOURCE_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/docs/npm.md ${CMAKE_SOURCE_DIR}/READMEFIRST)
add_dependencies (npmpkg mraajs)

if (BUILDDOC)
  find_package(Yuidoc)
  if (YUIDOC_FOUND)
    add_custom_target(jsdoc ALL
      COMMAND ${CMAKE_SOURCE_DIR}/doxygen2jsdoc/docgen.js -m mraa i xml/ -o jsdoc -c ${CMAKE_CURRENT_SOURCE_DIR}/doxygen2jsdoc_custom.json --strict -h
      COMMAND ${YUIDOC_EXECUTABLE} -C --no-sort --helpers ${CMAKE_SOURCE_DIR}/doxygen2jsdoc/generators/yuidoc/tmpl/mraa/helper.js --themedir ${CMAKE_SOURCE_DIR}/doxygen2jsdoc/generators/yuidoc/tmpl/mraa -o html/node jsdoc/yuidoc/mraa
      COMMAND ${CMAKE_SOURCE_DIR}/doxygen2jsdoc/tolower.js -i html/node
      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
      DEPENDS doc
      COMMENT "Generating API documentation with Yuidoc" VERBATIM
  )
  else ()
    message (INFO " - Failed to find Yuidoc. node.js API doc will not be generated")
  endif ()
endif ()

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mraa.node
         DESTINATION lib/node_modules/mraa)
