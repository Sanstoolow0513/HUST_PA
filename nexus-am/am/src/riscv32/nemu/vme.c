#include <am.h>
#include <riscv32.h>
#include <nemu.h>

#define PG_ALIGN __attribute((aligned(PGSIZE)))

static PDE kpdirs[NR_PDE] PG_ALIGN = {};
static PTE kptabs[(PMEM_SIZE + MMIO_SIZE) / PGSIZE] PG_ALIGN = {};
static void* (*pgalloc_usr)(size_t) = NULL;
static void (*pgfree_usr)(void*) = NULL;
static int vme_enable = 0;

static _Area segments[] = {      // Kernel memory mappings
  {.start = (void*)0x80000000u, .end = (void*)(0x80000000u + PMEM_SIZE)},
  {.start = (void*)MMIO_BASE,   .end = (void*)(MMIO_BASE + MMIO_SIZE)}
};

#define NR_KSEG_MAP (sizeof(segments) / sizeof(segments[0]))

static inline void set_satp(void *pdir) {
  asm volatile("csrw satp, %0" : : "r"(0x80000000 | ((uintptr_t)pdir >> 12)));
}

int _vme_init(void* (*pgalloc_f)(size_t), void (*pgfree_f)(void*)) {
  pgalloc_usr = pgalloc_f;
  pgfree_usr = pgfree_f;

  // make all PDEs invalid
  int i;
  for (i = 0; i < NR_PDE; i ++) {
    kpdirs[i] = 0;
  }

  PTE *ptab = kptabs;
  for (i = 0; i < NR_KSEG_MAP; i ++) {
    uint32_t pdir_idx = (uintptr_t)segments[i].start / (PGSIZE * NR_PTE);
    uint32_t pdir_idx_end = (uintptr_t)segments[i].end / (PGSIZE * NR_PTE);
    for (; pdir_idx < pdir_idx_end; pdir_idx ++) {
      // fill PDE
      kpdirs[pdir_idx] = ((uintptr_t)ptab >> PGSHFT << 10) | PTE_V;

      // fill PTE
      PTE pte = (PGADDR(pdir_idx, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      PTE pte_end = (PGADDR(pdir_idx + 1, 0, 0) >> PGSHFT << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
      for (; pte < pte_end; pte += (1 << 10)) {
        *ptab = pte;
        ptab ++;
      }
    }
  }

  set_satp(kpdirs);
  vme_enable = 1;

  return 0;
}

int _protect(_AddressSpace *as) {
  PDE *updir = (PDE*)(pgalloc_usr(1));
  as->ptr = updir;
  // map kernel space
  for (int i = 0; i < NR_PDE; i ++) {
    updir[i] = kpdirs[i];
  }

  return 0;
}

void _unprotect(_AddressSpace *as) {
}

static _AddressSpace *cur_as = NULL;
void __am_get_cur_as(_Context *c) {
  c->as = cur_as;
}

void __am_switch(_Context *c) {
  if (vme_enable && c->as != NULL) {
    // printf("[VME] __am_switch: as=%p, pdir=%p\n", c->as, c->as->ptr);
    set_satp(c->as->ptr);
    cur_as = c->as;
  } else {
    // printf("[VME] __am_switch: SKIP (vme_enable=%d, as=%p)\n", vme_enable, c->as);
  }
}

int _map(_AddressSpace *as, void *va, void *pa, int prot) {
  // 页目录基址
  PDE *pdir = (PDE *)as->ptr;
  // 计算页目录索引和页表索引
  uint32_t pdx = PDX(va);
  uint32_t ptx = PTX(va);
  // 检查页目录项是否有效
  if (!(pdir[pdx] & PTE_V)) {
    PTE *new_ptab = (PTE *)pgalloc_usr(1);
    for (int i = 0; i < NR_PTE; i++) {
      new_ptab[i] = 0;
    }
    // 设置页目录项：页表物理地址 >> 12 << 10 | PTE_V
    pdir[pdx] = ((uintptr_t)new_ptab >> 12 << 10) | PTE_V;
  }
  PTE *ptab = (PTE *)PTE_ADDR(pdir[pdx]);
  ptab[ptx] = ((uintptr_t)pa >> 12 << 10) | PTE_V | PTE_R | PTE_W | PTE_X;
  
  return 0;
}

_Context *_ucontext(_AddressSpace *as, _Area ustack, _Area kstack, void *entry, void *args) {
  // 在内核栈顶创建上下文
  _Context *c = (_Context *)kstack.end - 1;
  // 初始化
  c->sepc = (uintptr_t)entry;
  c->sstatus = 0;    
  c->gpr[10] = (uintptr_t)args;  // a0 = args
  c->gpr[2] = (uintptr_t)ustack.end;  // sp 指向用户栈顶
  // 设置地址空间指针
  c->as = as;
  return c;
}
