add_executable (AioA0 AioA0.cpp)
add_executable (blink-io-cpp Blink-IO.cpp)

include_directories(${PROJECT_SOURCE_DIR}/api)

target_link_libraries (AioA0 maa stdc++)
target_link_libraries (blink-io-cpp maa stdc++)
