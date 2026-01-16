#include "memory.h"

static void *pf = NULL;

void* new_page(size_t nr_page) {
  void *p = pf;
  pf += PGSIZE * nr_page;
  assert(pf < (void *)_heap.end);
  return p;
}

void free_page(void *p) {
  panic("not implement yet");
}

/* The brk() system call handler. */
int mm_brk(uintptr_t brk, intptr_t increment) {
#ifdef HAS_VME
  uintptr_t new_brk = brk + increment;
  
  // 如果新的 brk 超过 max_brk，需要映射新页面
  if (new_brk > current->max_brk) {
    // 按页对齐
    uintptr_t old_end = PGROUNDUP(current->max_brk);
    uintptr_t new_end = PGROUNDUP(new_brk);
    
    // 分配并映射新页面
    for (uintptr_t va = old_end; va < new_end; va += PGSIZE) {
      void *pa = new_page(1);
      _map(&current->as, (void *)va, pa, 0);
    }
    
    current->max_brk = new_brk;
  }
#endif
  return 0;
}

void init_mm() {
  pf = (void *)PGROUNDUP((uintptr_t)_heap.start);
  Log("free physical pages starting from %p", pf);

  _vme_init(new_page, free_page);
}
