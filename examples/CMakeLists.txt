add_executable (readi2c readi2c.cpp)
add_executable (hellomaa hellomaa.cpp)

target_link_libraries (hellomaa maa)
target_link_libraries (readi2c maa)
