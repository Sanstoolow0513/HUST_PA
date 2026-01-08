#ifndef __ARCH_H__
#define __ARCH_H__

struct _Context {
  uintptr_t gpr[32];  // gpr[0]=x0, gpr[1]=x1, ..., gpr[31]=x31
  uintptr_t sp_original;
  uintptr_t scause;
  uintptr_t sstatus;
  uintptr_t sepc;
  struct _AddressSpace *as;
};


#define GPR1 gpr[17]   // a7寄存器（系统调用号）
#define GPR2 gpr[10]   // a0寄存器（第一个参数）
#define GPR3 gpr[11]   // a1寄存器（第二个参数）
#define GPR4 gpr[12]   // a2寄存器（第三个参数）
#define GPRx gpr[10]   // 返回值寄存器（a0）

#endif
