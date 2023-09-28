message (INFO " - Adding RISC-V platforms")
set (mraa_LIB_PLAT_SRCS_NOAUTO ${mraa_LIB_SRCS_NOAUTO}
     ${mraa_LIB_RISCV_SRCS_NOAUTO} PARENT_SCOPE)
