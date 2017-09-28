message (INFO " - Adding MIPS platforms")
set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/mips/mips.c
  ${PROJECT_SOURCE_DIR}/src/mips/mediatek.c
  PARENT_SCOPE
)
