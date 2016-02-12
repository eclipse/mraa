message (INFO " - Adding firmata backend support")
set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
     ${PROJECT_SOURCE_DIR}/src/firmata/firmata.c PARENT_SCOPE)
