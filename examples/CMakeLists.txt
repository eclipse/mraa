add_subdirectory (c)
add_subdirectory (platform)

if (NOT ANDROID_TOOLCHAIN)
  add_subdirectory (c++)
endif ()

install (DIRECTORY ${PROJECT_SOURCE_DIR}/examples/ DESTINATION ${CMAKE_INSTALL_DATADIR}/mraa/examples)
