message (INFO " - Adding AFB platform")
set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
    ${mraa_LIB_AFB_SRCS_NOAUTO} PARENT_SCOPE)
