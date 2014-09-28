# - FindPHPLibs
# Find PHP interpreter includes and library
#
#  PHPLIBS_FOUND      - True if PHP libs found
#  PHP_VERSION_STRING - The version of PHP found (x.y.z)
#  PHP_LIBRARIES      - Libaries (standard variable)
#  PHP_INCLUDE_DIRS   - List of include directories
#  PHP_INCLUDE_DIR    - Main include directory prefix
#  PHP_EXTENSION_DIR  - Location of PHP extension DSO-s
#  PHP_EXECUTABLE     - PHP executable
#  PHP_INSTALL_PREFIX - PHP install prefix, as reported

INCLUDE(CMakeFindFrameworks)

IF( NOT PHP_CONFIG_EXECUTABLE )
FIND_PROGRAM(PHP_CONFIG_EXECUTABLE
  NAMES php5-config php-config
  )
ENDIF( NOT PHP_CONFIG_EXECUTABLE )

MACRO(GET_FROM_PHP_CONFIG args variable)
  EXECUTE_PROCESS(COMMAND ${PHP_CONFIG_EXECUTABLE} ${args}
    OUTPUT_VARIABLE ${variable})
 STRING(REPLACE "\n" "" ${variable} "${${variable}}")
ENDMACRO(GET_FROM_PHP_CONFIG cmd variable)

IF(PHP_CONFIG_EXECUTABLE)
  GET_FROM_PHP_CONFIG("--version"       PHP_VERSION_STRING)
  GET_FROM_PHP_CONFIG("--php-binary"    PHP_EXECUTABLE)
  GET_FROM_PHP_CONFIG("--include-dir"   PHP_INCLUDE_DIR)
  GET_FROM_PHP_CONFIG("--extension-dir" PHP_EXTENSION_DIR)
  GET_FROM_PHP_CONFIG("--includes"      PHP_INCLUDE_DIRS)
  GET_FROM_PHP_CONFIG("--prefix"        PHP_INSTALL_PREFIX)
  STRING(REPLACE "-I" "" PHP_INCLUDE_DIRS "${PHP_INCLUDE_DIRS}")
  STRING(REPLACE " " ";" PHP_INCLUDE_DIRS "${PHP_INCLUDE_DIRS}")
ENDIF(PHP_CONFIG_EXECUTABLE)

# FIXME: Maybe we need all this crap that php-config --libs puts out,
#        however after building a few swig bindings without them,
#        I seriously doubt it.
SET(PHP_LIBRARIES "")

INCLUDE(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(PHPLibs DEFAULT_MSG PHP_INCLUDE_DIRS)

MARK_AS_ADVANCED(PHP_LIBRARIES PHP_INCLUDE_DIRS)
