message (INFO " - Adding ARM platforms")
set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/arm/arm.c
  PARENT_SCOPE
)
