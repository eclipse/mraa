message (INFO " - Adding x86 platforms")
set (mraa_LIB_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
  ${PROJECT_SOURCE_DIR}/src/x86/x86.c
  ${PROJECT_SOURCE_DIR}/src/x86/intel_galileo_rev_d.c
  ${PROJECT_SOURCE_DIR}/src/x86/intel_galileo_rev_g.c
  ${PROJECT_SOURCE_DIR}/src/x86/intel_edison_fab_c.c
  ${PROJECT_SOURCE_DIR}/src/x86/intel_de3815.c
  ${PROJECT_SOURCE_DIR}/src/x86/intel_minnow_max.c
  PARENT_SCOPE
)
