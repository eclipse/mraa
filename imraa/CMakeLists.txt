add_executable (imraa imraa.c)

include_directories (${PROJECT_SOURCE_DIR}/api)
include_directories (${PROJECT_SOURCE_DIR}/include)
include_directories (${PROJECT_SOURCE_DIR}/api/mraa)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -DIMRAA=1")

find_package (JSON-C REQUIRED)
include_directories (${JSON-C_INCLUDE_DIR})
find_package (UDEV REQUIRED)
include_directories (${UDEV_INCLUDE_DIR})
target_link_libraries (imraa mraa ${JSON-C_LIBRARIES} ${UDEV_LIBRARIES})

set_property (TARGET imraa PROPERTY C_STANDARD 99)

install (TARGETS imraa DESTINATION bin)
