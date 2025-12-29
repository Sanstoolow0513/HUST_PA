#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t epc) {
  // 1. 将当前PC保存到sepc
  cpu.sepc = epc;
  // 2. 设置scause为异常号
  cpu.scause = NO;
  // 3. 从stvec寄存器中读取异常入口地址
  // 由于简化实现，需要实现一个全局变量来存储stvec
  // 或者直接跳转到已知的异常入口地址
  
  // 4. 跳转到异常入口地址
  // 需要实现stvec的读取和跳转逻辑
}

bool isa_query_intr(void) {
  
  return false;
}
