if (FIRMATA)
  message (STATUS "INFO - Adding firmata backend support")
  set (mraa_firmata_LIBS
    ${PROJECT_SOURCE_DIR}/src/firmata/firmata.c
    ${PROJECT_SOURCE_DIR}/src/firmata/firmata_mraa.c
  )
  if (FIRMATABLE)
    message (STATUS "INFO - Adding firmata ble backend support")
    set (mraa_firmata_LIBS ${mraa_firmata_LIBS}
      ${PROJECT_SOURCE_DIR}/src/firmata/firmata_ble.c
    )
  endif ()
  set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO} ${mraa_firmata_LIBS}
    PARENT_SCOPE
  )  
endif ()
