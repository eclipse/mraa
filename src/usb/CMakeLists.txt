message (INFO " - Adding USB platforms")
set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/usb/usb.c
  ${PROJECT_SOURCE_DIR}/src/usb/ftdi_ft4222.c
  PARENT_SCOPE
)

