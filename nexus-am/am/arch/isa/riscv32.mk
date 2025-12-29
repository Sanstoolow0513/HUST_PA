CROSS_COMPILE := riscv-none-embed-
COMMON_FLAGS  := -fno-pic -march=rv32ima_zicsr -mabi=ilp32
#这里有修改，在报告中需要体现
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv
