#include "proc.h"
#include <elf.h>

#ifdef HAS_VME
#include <riscv32.h>
#endif

#ifdef __ISA_AM_NATIVE__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

static uintptr_t loader(PCB *pcb, const char *filename) {
  int fd = fs_open(filename, 0, 0);
  
  Elf_Ehdr ehdr;
  fs_read(fd, &ehdr, sizeof(Elf_Ehdr));
  
  assert(*(uint32_t *)ehdr.e_ident == 0x464c457f);
  
  Elf_Phdr phdr;
  for (int i = 0; i < ehdr.e_phnum; i++) {
    fs_lseek(fd, ehdr.e_phoff + i * ehdr.e_phentsize, 0);
    fs_read(fd, &phdr, sizeof(Elf_Phdr));
    
    if (phdr.p_type == PT_LOAD) {
#ifdef HAS_VME
      uintptr_t va_start = phdr.p_vaddr;
      uintptr_t va_end = phdr.p_vaddr + phdr.p_memsz;
      
      uintptr_t va_aligned = PGROUNDDOWN(va_start);
      uintptr_t va_end_aligned = PGROUNDUP(va_end);
      
      for (uintptr_t va = va_aligned; va < va_end_aligned; va += PGSIZE) {
        void *pa = new_page(1);
        _map(&pcb->as, (void *)va, pa, 0);
      }
      fs_lseek(fd, phdr.p_offset, 0);
      
      // 按页读取
      uintptr_t file_offset = phdr.p_offset;
      uintptr_t filesz_remaining = phdr.p_filesz;
      uintptr_t va = phdr.p_vaddr;
      
      while (filesz_remaining > 0) {
        uintptr_t page_offset = va & (PGSIZE - 1);
        uintptr_t bytes_in_page = PGSIZE - page_offset;
        if (bytes_in_page > filesz_remaining) {
          bytes_in_page = filesz_remaining;
        }
        
        PDE *pdir = (PDE *)pcb->as.ptr;
        uint32_t pdx = PDX(va);
        uint32_t ptx = PTX(va);
        PTE *ptab = (PTE *)PTE_ADDR(pdir[pdx]);
        void *pa = (void *)((ptab[ptx] >> 10 << 12) | page_offset);
        
        fs_lseek(fd, file_offset, 0);
        fs_read(fd, pa, bytes_in_page);
        
        file_offset += bytes_in_page;
        va += bytes_in_page;
        filesz_remaining -= bytes_in_page;
      }
      
      for (uintptr_t addr = phdr.p_vaddr + phdr.p_filesz; 
           addr < phdr.p_vaddr + phdr.p_memsz; addr++) {
        uintptr_t page_offset = addr & (PGSIZE - 1);
        PDE *pdir = (PDE *)pcb->as.ptr;
        uint32_t pdx = PDX(addr);
        uint32_t ptx = PTX(addr);
        PTE *ptab = (PTE *)PTE_ADDR(pdir[pdx]);
        uint8_t *pa = (uint8_t *)((ptab[ptx] >> 10 << 12) | page_offset);
        *pa = 0;
      }
#else
      fs_lseek(fd, phdr.p_offset, 0);
      fs_read(fd, (void *)phdr.p_vaddr, phdr.p_filesz);
      memset((void *)(phdr.p_vaddr + phdr.p_filesz), 0, phdr.p_memsz - phdr.p_filesz);
#endif
    }
  }
  
  fs_close(fd);
  return ehdr.e_entry;
}


void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %x", entry);
  ((void(*)())entry) ();
}

void context_kload(PCB *pcb, void *entry) {
  _Area stack;
  stack.start = pcb->stack;
  stack.end = stack.start + sizeof(pcb->stack);
  pcb->cp = _kcontext(stack, entry, NULL);
}

void context_uload(PCB *pcb, const char *filename) {
#ifdef HAS_VME
  // 创建地址空间
  _protect(&pcb->as);
  // 初始化堆区边界
  pcb->max_brk = 0;
  
  // 分配用户栈（8 页，放在 0x80000000 以下）
  uintptr_t ustack_top = 0x80000000u;  // 用户栈顶（虚拟地址）
  for (int i = 0; i < 8; i++) {
    void *pa = new_page(1);
    _map(&pcb->as, (void *)(ustack_top - (i + 1) * PGSIZE), pa, 0);
  }
#endif

  uintptr_t entry = loader(pcb, filename);
  
  _Area kstack;
  kstack.start = pcb->stack;
  kstack.end = kstack.start + sizeof(pcb->stack);

#ifdef HAS_VME
  _Area ustack;
  ustack.start = (void *)(ustack_top - 8 * PGSIZE);
  ustack.end = (void *)ustack_top;
  pcb->cp = _ucontext(&pcb->as, ustack, kstack, (void *)entry, NULL);
#else
  pcb->cp = _ucontext(&pcb->as, kstack, kstack, (void *)entry, NULL);
#endif
}
