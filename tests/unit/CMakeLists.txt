# For now, Google Test is NOT required */
find_package(GTest)

# If not found, print a status message and return
if(NOT GTEST_FOUND)
    message(STATUS "Install Google Test to enable additional unit testing")
    return ()
endif()

# Unit tests - C common header methods
add_executable(api_common_h_unit api/api_common_h_unit.cxx)
target_link_libraries(api_common_h_unit GTest::GTest GTest::Main mraa)
target_include_directories(api_common_h_unit
    PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/api" "${CMAKE_SOURCE_DIR}/api/mraa")
gtest_add_tests(api_common_h_unit "" AUTO)

# Unit tests - C++ common header methods
add_executable(api_common_hpp_unit api/api_common_hpp_unit.cxx)
target_link_libraries(api_common_hpp_unit GTest::GTest GTest::Main mraa)
target_include_directories(api_common_hpp_unit PRIVATE "${CMAKE_SOURCE_DIR}/api")
gtest_add_tests(api_common_hpp_unit "" AUTO)

# Add a custom target for unit tests
add_custom_target(tests-unit ALL
    DEPENDS
        api_common_h_unit
        api_common_hpp_unit
    COMMENT "mraa unit test collection")
