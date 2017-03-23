# detect the board where we are running the test
function(mraa_PLATFORM_DETECT)
    set(platform_list
      "GalileoGen2"            INTEL_GALILEO_GEN2
      "SIMATIC IOT2000"        INTEL_GALILEO_GEN2
      "BODEGA BAY"             INTEL_EDISON_FAB_C
      "SALT BAY"               INTEL_EDISON_FAB_C
      "DE3815"                 INTEL_DE3815
      "NUC5i5MYBE"             INTEL_NUC5
      "NOTEBOOK"               INTEL_MINNOWBOARD_MAX
      "MinnowBoard MAX"        INTEL_MINNOWBOARD_MAX
      "Galileo"                INTEL_GALILEO_GEN1
      "MinnowBoard Compatible" INTEL_MINNOWBOARD_MAX
      "MinnowBoard Turbot"     INTEL_MINNOWBOARD_MAX
      "Braswell Cherry Hill"   INTEL_CHERRYHILLS
      "UP-CHT01"               UP
      "RVP"                    INTEL_JOULE_EXPANSION
      "SDS"                    INTEL_JOULE_EXPANSION
    )

    # message(STATUS "INFO - ${platform_list}")

    file(STRINGS "/sys/class/dmi/id/board_name"    mraa_BOARD_NAME)
    file(STRINGS "/sys/class/dmi/id/board_version" mraa_BOARD_VERSION)
    message(STATUS "INFO - Board Name: ${mraa_BOARD_NAME} Board Version: ${mraa_BOARD_VERSION}")

    list(LENGTH platform_list len_plat)
    math(EXPR len_div_2 "(${len_plat}-1)/2")      # divide by 2 to get name and board_name

    #set(mraa_DETECTED_PLATFORM "" PARENT_SCOPE)

    foreach(val RANGE ${len_div_2})
      math(EXPR val_name "${val} * 2")
      math(EXPR val_plat "${val_name} + 1")

      #message(STATUS "INFO - try val_plat val_name: ${val_plat} '${val_name}'")
      list(GET platform_list  ${val_name} name)
      list(GET platform_list  ${val_plat} plat)
      #message(STATUS "INFO - try plat name: ${plat} ${name}")
      if(name STREQUAL mraa_BOARD_NAME)
         message(STATUS "INFO - Detected platform ${plat}")
         set(mraa_DETECTED_PLATFORM ${plat} PARENT_SCOPE)
         break()
      endif()
    endforeach()
endfunction()

function(mraa_ADD_PLATFORM_TEST)
    # detect the platform
    mraa_PLATFORM_DETECT()

    if(mraa_DETECTED_PLATFORM)
        string(TOLOWER ${mraa_DETECTED_PLATFORM} mraa_DETECTED_PLATFORM_lower)
        message(STATUS "INFO - Try to find platform test directory: ${CMAKE_CURRENT_SOURCE_DIR}/${mraa_DETECTED_PLATFORM_lower}")

        if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${mraa_DETECTED_PLATFORM_lower}/")
            message(STATUS "INFO - Found platform test directory; include it: ${CMAKE_CURRENT_SOURCE_DIR}/${mraa_DETECTED_PLATFORM_lower}")
            add_subdirectory(${mraa_DETECTED_PLATFORM_lower})
        endif()
    else()
      message(STATUS "WARN - Impossible to detect platform from dmi info")
    endif()
endfunction()

################################################################################

if (BUILDSWIGJAVA)
    add_test (NAME check_clean COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_clean.py
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR})

    add_test (NAME check_samplenames COMMAND ${PYTHON_EXECUTABLE}
        ${CMAKE_CURRENT_SOURCE_DIR}/check_samplenames.py
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR})
endif()

if (BUILDSWIGPYTHON)
    if (PYTHON2INTERP_FOUND)
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python2")
    elseif (PYTHON3INTERP_FOUND)
        set (PYTHON_DEFAULT_PYTHONPATH "${CMAKE_BINARY_DIR}/src/python/python3")
    endif ()
    if (MOCKPLAT)
        add_subdirectory (mock)
    else()
        add_test (NAME py_general COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/general_checks.py)
        set_tests_properties(py_general PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

        add_test (NAME py_platform COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/platform_checks.py)
        set_tests_properties(py_platform PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")

        add_test (NAME py_gpio COMMAND ${PYTHON_DEFAULT_EXECUTABLE} ${CMAKE_CURRENT_SOURCE_DIR}/gpio_checks.py)
        set_tests_properties(py_gpio PROPERTIES ENVIRONMENT "PYTHONPATH=${PYTHON_DEFAULT_PYTHONPATH}")
    endif()
endif()

# add platform specific tests
mraa_ADD_PLATFORM_TEST()
