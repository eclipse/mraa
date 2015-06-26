set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/usb/usb.c
)

if (FTDI4222)
   message (INFO " - FTDI4222")
  set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
    ${PROJECT_SOURCE_DIR}/src/usb/ftdi_ft4222.c
    PARENT_SCOPE
  )
endif ()



