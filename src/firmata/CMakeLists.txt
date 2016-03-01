if (FIRMATA)
  message (STATUS "INFO - Adding firmata backend support")
  set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_PLAT_SRCS_NOAUTO}
    ${PROJECT_SOURCE_DIR}/src/firmata/firmata.c
    ${PROJECT_SOURCE_DIR}/src/firmata/servo.c
    ${PROJECT_SOURCE_DIR}/src/firmata/firmata_mraa.c
    PARENT_SCOPE
  )
  message (${mraa_LIB_PLAT_SRCS_NOAUTO})
endif ()
