message (INFO " - Adding ARM platforms")
set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/arm/arm.c
  ${PROJECT_SOURCE_DIR}/src/arm/raspberry_pi.c
  ${PROJECT_SOURCE_DIR}/src/arm/beaglebone.c
  ${PROJECT_SOURCE_DIR}/src/arm/banana.c
  PARENT_SCOPE
)
