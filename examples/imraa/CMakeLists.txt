add_executable (arduino101uploader arduino101uploader.c)

include_directories(${PROJECT_SOURCE_DIR}/api)
# FIXME Hack to access mraa internal types used by mraa-i2c
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/api/mraa)

target_link_libraries (arduino101uploader mraa)

if (INSTALLIMRAA)
   install (TARGETS arduino101uploader DESTINATION bin)
#   set (SYSTEMD_CONFIGURATIONS_FILES_DIR "/lib/systemd/system" )
#   install(FILES ${PROJECT_BINARY_DIR}/systemd/dlt.service DESTINATION ${SYSTEMD_CONFIGURATIONS_FILES_DIR} )
endif()
