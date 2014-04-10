find_package (GTest REQUIRED)

set(PROJECT_TEST_NAME "${PROJECT_NAME}_test")
set(COMMON_INCLUDES ${PROJECT_SOURCE_DIR}/src)

include_directories(${GTEST_INCLUDE_DIRS} ${COMMON_INCLUDES})

add_executable(${PROJECT_TEST_NAME} "maa_test.cxx")
target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME_STR} ${GTEST_BOTH_LIBRARIES} maa pthread)

add_test(Basic ${PROJECT_TEST_NAME})
