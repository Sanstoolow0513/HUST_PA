#include "common.h"
#include "syscall.h"
#include "fs.h"
#include "proc.h"

int mm_brk(uintptr_t brk, intptr_t increment);
void context_uload(PCB *pcb, const char *filename);

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);
void naive_uload(void *pcb, const char *filename);

_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;  // syscall number
  a[1] = c->GPR2;  // arg0
  a[2] = c->GPR3;  // arg1
  a[3] = c->GPR4;  // arg2
  switch (a[0]) {
    case SYS_exit:
      // naive_uload(NULL, "/bin/init");  // 批处理的逻辑 PA3
      // _halt(a[1]);  // Original behavior
      context_uload(current, "/bin/init"); // PA4.2
      break;
    case SYS_yield:
      _yield();
      c->GPRx = 0;
      break;
    case SYS_open:
      c->GPRx = fs_open((const char *)a[1], a[2], a[3]);
      break;
    case SYS_read:
      c->GPRx = fs_read(a[1], (void *)a[2], a[3]);
      break;
    case SYS_write:
      c->GPRx = fs_write(a[1], (const void *)a[2], a[3]);
      break;
    case SYS_close:
      c->GPRx = fs_close(a[1]);
      break;
    case SYS_lseek:
      c->GPRx = fs_lseek(a[1], a[2], a[3]);
      break;
    case SYS_brk:
      c->GPRx = mm_brk(a[1], 0);
      break;
    case SYS_execve:
      context_uload(current, (const char *)a[1]); 
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}