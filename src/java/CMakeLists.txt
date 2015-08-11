FIND_PACKAGE (JNI REQUIRED)

include_directories (
  ${JAVA_INCLUDE_PATH}
  ${JAVA_INCLUDE_PATH2}
  ${CMAKE_CURRENT_SOURCE_DIR}/..
)

# SWIG treats SWIG_FLAGS as a list and not a string so semicolon seperation is required
set_source_files_properties (mraajava.i PROPERTIES SWIG_FLAGS ";-package;mraa;-I${CMAKE_BINARY_DIR}/src")
set_source_files_properties (mraajava.i PROPERTIES CPLUSPLUS ON)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fpermissive")

set (JAVAC $ENV{JAVA_HOME}/bin/javac)
set (JAR $ENV{JAVA_HOME}/bin/jar)

swig_add_module (mraajava java mraajava.i ${mraa_LIB_SRCS})
swig_link_libraries (mraajava ${JAVA_LIBRARIES} ${mraa_LIBS})

add_custom_command (TARGET mraajava
  POST_BUILD
  COMMAND cmake -E echo "Compiling java.."
  COMMAND cmake -E make_directory ${CMAKE_CURRENT_BINARY_DIR}/mraa
  COMMAND ${JAVAC} *.java -d ${CMAKE_CURRENT_BINARY_DIR}
  COMMAND cmake -E echo "Creating jar"
  COMMAND ${JAR} cvf mraa.jar mraa
)

if (DOXYGEN_FOUND)
  foreach (_file ${DOCFILES})
    add_dependencies (${SWIG_MODULE_mraajava_REAL_NAME} ${_file}doc_i)
  endforeach ()
endif ()

install (FILES ${CMAKE_CURRENT_BINARY_DIR}/libmraajava.so
		DESTINATION lib/java
)
install (FILES ${CMAKE_CURRENT_BINARY_DIR}/mraa.jar
		DESTINATION lib/java
)
