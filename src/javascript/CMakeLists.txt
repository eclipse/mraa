find_package (Nodejs)

include_directories (
  ${NODE_INCLUDE_DIRS}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

macro(C2HEX RET VAL)
   if(${VAL} LESS 10)
     set(${RET} "${VAL}")
   elseif(${VAL} EQUAL 10)
     set(${RET} "A")
   elseif(${VAL} EQUAL 11)
     set(${RET} "B")
   elseif(${VAL} EQUAL 12)
     set(${RET} "C")
   elseif(${VAL} EQUAL 13)
     set(${RET} "D")
   elseif(${VAL} EQUAL 14)
     set(${RET} "E")
   elseif(${VAL} EQUAL 15)
     set(${RET} "F")
   else()
     message(WARNING " - wrong value in C2HEX: val=${VAL} -> use F !!!")
     set(${RET} "F")
   endif()
endmacro(C2HEX)

macro(CONVERT_DEC2HEX RET VAL)
  string(LENGTH VAL len)
  set(${RET} "00")
  if(${len} LESS 1 OR ${len} GREATER 3)
    message (WARNING " - there is a wrong input value length for CONVERT_DEC2HEX: '${VAL}' len: ${len} -> return ZERO !!!")
  else()
    string(REGEX MATCH "[0-9]+" TMP ${VAL})
    if(TMP GREATER 255)
       set (TMP 255)
       message (WARNING " - a value is above 255 in CONVERT_DEC2HEX: '${VAL}' -> use 0xFF ")
    endif()
    math(EXPR CLO "${TMP}&15")
    math(EXPR CHI "${TMP}>>4")
    C2HEX(CHIH ${CHI})
    C2HEX(CLOH ${CLO})
    set(${RET} "${CHIH}${CLOH}")
  endif()
endmacro(CONVERT_DEC2HEX)

message (INFO " - v8-version.h search path: ${NODE_INCLUDE_DIRS}")

foreach(VERPATH ${NODE_INCLUDE_DIRS})
   message(INFO " -   check ${VERPATH}")
   if(EXISTS "${VERPATH}/v8-version.h")
      file(READ "${VERPATH}/v8-version.h" V8_VERSION_FILE)
      string(REGEX MATCH "\#define V8_MAJOR_VERSION [0-9]+" V8_MAJOR_VERSION_STR ${V8_VERSION_FILE})
      string(REGEX MATCH "\#define V8_MINOR_VERSION [0-9]+" V8_MINOR_VERSION_STR ${V8_VERSION_FILE})
      string(REGEX MATCH "\#define V8_BUILD_NUMBER [0-9]+" V8_BUILD_NUMBER_STR ${V8_VERSION_FILE})
      string(REGEX REPLACE "\#define V8_MAJOR_VERSION " "" V8_MAJOR_VERSION ${V8_MAJOR_VERSION_STR})
      string(REGEX REPLACE "\#define V8_MINOR_VERSION " "" V8_MINOR_VERSION ${V8_MINOR_VERSION_STR})
      string(REGEX REPLACE "\#define V8_BUILD_NUMBER " "" V8_BUILD_NUMBER ${V8_BUILD_NUMBER_STR})
      message (INFO " -   Node API: v8-version.h has a version like ${V8_MAJOR_VERSION}.${V8_MINOR_VERSION}.${V8_BUILD_NUMBER}")
      break()
   endif()
endforeach()


# convert V8 version to hex string
if(DEFINED V8_MAJOR_VERSION AND DEFINED V8_MINOR_VERSION AND DEFINED V8_BUILD_NUMBER)
   message (INFO " - Use V8 API version ${V8_MAJOR_VERSION}.${V8_MINOR_VERSION}.${V8_BUILD_NUMBER}")
   # use a value provided by V8 API (only for version 4.3.0 or above)
   CONVERT_DEC2HEX(V8_VERSION_MAJOR_HEX ${V8_MAJOR_VERSION})
   CONVERT_DEC2HEX(V8_VERSION_MINOR_HEX ${V8_MINOR_VERSION})
   CONVERT_DEC2HEX(V8_VERSION_PATCH_HEX ${V8_BUILD_NUMBER})
else()
   message (INFO " -  Use a command line version ${V8_VERSION_MAJOR}.${V8_VERSION_MINOR}.${V8_VERSION_PATCH}")
   # use a value provided as a command line parameter
   CONVERT_DEC2HEX(V8_VERSION_MAJOR_HEX ${V8_VERSION_MAJOR})
   CONVERT_DEC2HEX(V8_VERSION_MINOR_HEX ${V8_VERSION_MINOR})
   CONVERT_DEC2HEX(V8_VERSION_PATCH_HEX ${V8_VERSION_PATCH})
endif()
set (V8_VERSION_HEX 0x${V8_VERSION_MAJOR_HEX}${V8_VERSION_MINOR_HEX}${V8_VERSION_PATCH_HEX})

message (INFO " -    where V8_VERSION_HEX = ${V8_VERSION_HEX}, V8_VERSION_MAJOR=${V8_VERSION_MAJOR}")

# SWIG treats SWIG_FLAGS as a list and not a string so semicolon seperation is
# required.

set_property (SOURCE mraajs.i PROPERTY SWIG_FLAGS "-node"
              "-I${CMAKE_BINARY_DIR}/src" "-DV8_VERSION=${V8_VERSION_HEX}")
set_source_files_properties (mraajs.i PROPERTIES CPLUSPLUS ON)

swig_add_module (mraajs javascript mraajs.i ${mraa_LIB_SRCS})
swig_link_libraries (mraajs ${NODE_LIBRARIES} ${mraa_LIBS})

set_target_properties (mraajs PROPERTIES
  COMPILE_FLAGS "${CMAKE_CXX_FLAGS} -DBUILDING_NODE_EXTENSION -DSWIGJAVASCRIPT=${SWIG_FOUND}"
  PREFIX ""
  OUTPUT_NAME mraa
  SUFFIX ".node"
)

if (${V8_VERSION_MAJOR} GREATER 3)
  message (INFO " - Using V8 version > 3 so requiring C++11 compiler")
  # Node 0.12.x V8 engine major version is '3'.
  # Node 2.1.0  V8 engine major version is '4'.
  set_property (TARGET mraajs PROPERTY CXX_STANDARD 11)
  set_property (TARGET mraajs PROPERTY CXX_STANDARD_REQUIRED ON)
endif ()

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
