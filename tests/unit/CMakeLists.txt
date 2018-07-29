# Use the IN_LIST operator if available (for gtest)
if(POLICY CMP0057)
    cmake_policy(SET CMP0057 NEW)
endif()

# For now, Google Test is NOT required */
find_package(GTest)

# If not found, print a status message and return
if(NOT GTEST_FOUND)
    message(STATUS "Install Google Test to enable additional unit testing")
    return ()
endif()

# Unit tests - C common header methods
add_executable(test_unit_common_h api/api_common_h_unit.cxx)
target_link_libraries(test_unit_common_h ${GTEST_BOTH_LIBRARIES} mraa)
target_include_directories(test_unit_common_h
    PRIVATE "${CMAKE_SOURCE_DIR}" "${CMAKE_SOURCE_DIR}/api" "${CMAKE_SOURCE_DIR}/api/mraa")
gtest_add_tests(test_unit_common_h "" api/api_common_h_unit.cxx)
list(APPEND GTEST_UNIT_TEST_TARGETS test_unit_common_h)

# Unit tests - C++ common header methods
add_executable(test_unit_common_hpp api/api_common_hpp_unit.cxx)
target_link_libraries(test_unit_common_hpp ${GTEST_BOTH_LIBRARIES} mraa)
target_include_directories(test_unit_common_hpp PRIVATE "${CMAKE_SOURCE_DIR}/api")
gtest_add_tests(test_unit_common_hpp "" api/api_common_hpp_unit.cxx)
list(APPEND GTEST_UNIT_TEST_TARGETS test_unit_common_hpp)

if (FTDI4222 AND USBPLAT)
    # Unit tests - Test platform extenders (as much as possible)
    add_executable(test_unit_ftdi4222 platform_extender/platform_extender.cxx)
    target_link_libraries(test_unit_ftdi4222 ${GTEST_BOTH_LIBRARIES} mraa-platform-ft4222 dl)
    target_include_directories(test_unit_ftdi4222 PRIVATE "${PROJECT_SOURCE_DIR}/api"
        "${PROJECT_SOURCE_DIR}/api/mraa"
        "${PROJECT_SOURCE_DIR}/include")
    gtest_add_tests(test_unit_ftdi4222 "" platform_extender/platform_extender.cxx)
    list(APPEND GTEST_UNIT_TEST_TARGETS test_unit_ftdi4222)
endif ()

# Unit tests - test C initio header methods on MOCK platform only
if (DETECTED_ARCH STREQUAL "MOCK")
    add_executable(test_unit_ioinit_h api/mraa_initio_h_unit.cxx)
    target_link_libraries(test_unit_ioinit_h ${GTEST_BOTH_LIBRARIES} mraa)
    target_include_directories(test_unit_ioinit_h PRIVATE "${CMAKE_SOURCE_DIR}/api")
    gtest_add_tests(test_unit_ioinit_h "" api/mraa_initio_h_unit.cxx)
    list(APPEND GTEST_UNIT_TEST_TARGETS test_unit_ioinit_h)

    add_executable(test_unit_ioinit_hpp api/mraa_initio_hpp_unit.cxx)
    target_link_libraries(test_unit_ioinit_hpp ${GTEST_BOTH_LIBRARIES} mraa)
    target_include_directories(test_unit_ioinit_hpp PRIVATE "${CMAKE_SOURCE_DIR}/api")
    gtest_add_tests(test_unit_ioinit_hpp "" api/mraa_initio_hpp_unit.cxx)
    list(APPEND GTEST_UNIT_TEST_TARGETS test_unit_ioinit_hpp)

    # The initio C++ header requires c++11
    use_cxx_11(test_unit_ioinit_hpp)
endif()

# Add a target for all unit tests
add_custom_target(test_unit_all ALL DEPENDS ${GTEST_UNIT_TEST_TARGETS})
