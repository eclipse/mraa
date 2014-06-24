find_package (GTest REQUIRED)

set(PROJECT_TEST_NAME "${PROJECT_NAME}_test")

include_directories(
  ${GTEST_INCLUDE_DIRS}
  ${PROJECT_SOURCE_DIR}/api
  ${PROJECT_SOURCE_DIR}/include
)

add_executable(${PROJECT_TEST_NAME} "mraa_test.cxx")

target_link_libraries(${PROJECT_TEST_NAME} ${PROJECT_NAME_STR} ${GTEST_BOTH_LIBRARIES} mraa pthread)

add_test(Basic ${PROJECT_TEST_NAME})
