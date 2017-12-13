message (STATUS "INFO - Adding Peripheral Manager platform")
set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
     ${mraa_LIB_PERIPHERALMAN_SRCS_NOAUTO} PARENT_SCOPE)
