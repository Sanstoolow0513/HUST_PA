CROSS_COMPILE := riscv-none-embed-
COMMON_FLAGS  := -fno-pic --march=rv32ima_zicsr  -mabi=ilp32
CFLAGS        += $(COMMON_FLAGS) -static
ASFLAGS       += $(COMMON_FLAGS) -O0
LDFLAGS       += -melf32lriscv
