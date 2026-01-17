#include "nemu.h"
#include "rtl/rtl.h"
#include "cpu/exec.h"
 
#define IRQ_TIMER 0x80000005

// sstatus 位定义
#define SSTATUS_SIE  (1 << 1)  // 全局中断使能
#define SSTATUS_SPIE (1 << 5)  // 保存的中断使能

 void raise_intr(uint32_t NO, vaddr_t epc) {
   cpu.sepc = epc;
   cpu.scause = NO;
 
  // 保存 SIE 到 SPIE，然后清除 SIE (关中断)
  if (cpu.sstatus & SSTATUS_SIE) {
    cpu.sstatus |= SSTATUS_SPIE;
  } else {
    cpu.sstatus &= ~SSTATUS_SPIE;
  }
  cpu.sstatus &= ~SSTATUS_SIE;

   vaddr_t tvec = cpu.stvec;
 
   decinfo.jmp_pc = tvec;
   decinfo.is_jmp = 1;
 }
 
 bool isa_query_intr(void) {
// -  return false;
  // 检查: INTR引脚高电平 && SIE开中断
  if (cpu.INTR && (cpu.sstatus & SSTATUS_SIE)) {
    cpu.INTR = false;
    raise_intr(IRQ_TIMER, cpu.pc);
    return true;
  }
  return false;
 }