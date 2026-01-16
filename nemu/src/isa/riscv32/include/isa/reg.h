#ifndef __RISCV32_REG_H__
#define __RISCV32_REG_H__

#include "common.h"

#define PC_START (0x80000000u + IMAGE_START)

typedef struct {
  struct {
    rtlreg_t _32;
  } gpr[32];

  vaddr_t pc;
  vaddr_t stvec;   // 异常入口地址
  vaddr_t sepc;    // 异常返回地址
  uint32_t sstatus; // 处理器状态
  uint32_t scause;  // 异常原因
  uint32_t satp;    // 页表基址寄存器 (分页)
} CPU_state;

static inline int check_reg_index(int index) {
  assert(index >= 0 && index < 32);
  return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)

static inline const char* reg_name(int index, int width) {
  extern const char* regsl[];
  assert(index >= 0 && index < 32);
  return regsl[index];
}

#endif
