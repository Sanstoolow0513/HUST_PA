#include "nemu.h"
#define PGSIZE 4096
#define PTE_V 0x01

static inline int paging_enabled() {
  return (cpu.satp & 0x80000000) != 0;
}

static paddr_t page_translate(vaddr_t vaddr) {
  // 调试：当访问用户空间地址时输出 satp 状态
  if (vaddr >= 0x40000000 && vaddr < 0x80000000) {
    printf("[MMU] page_translate: vaddr=0x%08x, satp=0x%08x, paging=%d\n", 
           vaddr, cpu.satp, paging_enabled());
  }
  
  if (!paging_enabled()) {
    return vaddr;  // 未启用分页，直接返回
  }
  
  // 调试输出（仅用户空间地址）
  // if (vaddr >= 0x40000000 && vaddr < 0x80000000) {
  //   printf("page_translate: vaddr=0x%08x, satp=0x%08x\n", vaddr, cpu.satp);
  // }
  
  paddr_t pdir_base = (cpu.satp & 0x3FFFFF) << 12;
  // 一级页表索引（va[31:22]）
  uint32_t vpn1 = (vaddr >> 22) & 0x3FF;
  // 二级页表索引（va[21:12]）
  uint32_t vpn0 = (vaddr >> 12) & 0x3FF;
  // 页内偏移（va[11:0]）
  uint32_t offset = vaddr & 0xFFF;
  // 读取页目录项
  paddr_t pde_addr = pdir_base + vpn1 * 4;
  uint32_t pde = paddr_read(pde_addr, 4);
  assert(pde & PTE_V);  // 检查有效位
  
  // 页表基址（pde[31:10] << 12）
  paddr_t ptab_base = (pde >> 10) << 12;
  
  // 读取页表项
  paddr_t pte_addr = ptab_base + vpn0 * 4;
  uint32_t pte = paddr_read(pte_addr, 4);
  assert(pte & PTE_V);  // 检查有效位
  
  // 物理页号（pte[31:10] << 12）+ 偏移
  paddr_t paddr = ((pte >> 10) << 12) | offset;
  return paddr;
}
uint32_t isa_vaddr_read(vaddr_t addr, int len) {
  if ((addr & 0xFFF) + len > PGSIZE) {
    uint32_t result = 0;
    for (int i = 0; i < len; i++) {
      paddr_t paddr = page_translate(addr + i);
      result |= paddr_read(paddr, 1) << (i * 8);
    }
    return result;
  }
  paddr_t paddr = page_translate(addr);
  return paddr_read(paddr, len);
}
void isa_vaddr_write(vaddr_t addr, uint32_t data, int len) {
  if ((addr & 0xFFF) + len > PGSIZE) {
    for (int i = 0; i < len; i++) {
      paddr_t paddr = page_translate(addr + i);
      paddr_write(paddr, (data >> (i * 8)) & 0xFF, 1);
    }
    return;
  }
  paddr_t paddr = page_translate(addr);
  paddr_write(paddr, data, len);
}