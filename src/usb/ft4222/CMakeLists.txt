if (FTDI4222)
    set(target_name mraa-platform-ft4222)
    add_library(${target_name} SHARED ftdi_ft4222.cxx ftdi_ft4222.hpp)
    target_link_libraries(${target_name} ${LIBFT4222_LIBRARIES})
    install(TARGETS ${target_name} DESTINATION ${LIB_INSTALL_DIR})

    set_target_properties(
       ${target_name}
       PROPERTIES
       SOVERSION ${mraa_VERSION_MAJOR}
       VERSION ${mraa_VERSION_STRING}
    )
    if (MSYS)
      # Under MSYS we have to put our generated DLL into bin, otherwise it's not picked up
      install(TARGETS ${target_name} DESTINATION ${CMAKE_INSTALL_BINDIR})
    else ()
      install(TARGETS ${target_name} DESTINATION ${LIB_INSTALL_DIR})
    endif ()
endif()
