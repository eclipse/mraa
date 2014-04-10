add_executable (readi2c readi2c.cpp)
add_executable (hellomaa hellomaa.cpp)

include_directories(${PROJECT_SOURCE_DIR}/api ${PROJECT_SOURCE_DIR}/include)

target_link_libraries (hellomaa maa)
target_link_libraries (readi2c maa)
