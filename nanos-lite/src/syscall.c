#include "common.h"
#include "syscall.h"
_Context* do_syscall(_Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;  // syscall number
  a[1] = c->GPR2;  // arg0
  a[2] = c->GPR3;  // arg1
  a[3] = c->GPR4;  // arg2
  switch (a[0]) {
    case SYS_exit:
      _halt(a[1]);
      break;
    case SYS_yield:
      _yield();
      c->GPRx = 0;
      break;
    case SYS_write:
      if (a[1] == 1 || a[1] == 2) {  // stdout or stderr
        char *buf = (char *)a[2];
        for (size_t i = 0; i < a[3]; i++) {
          _putc(buf[i]);
        }
        c->GPRx = a[3];
      } else {
        c->GPRx = -1;
      }
      break;
    case SYS_brk:
      c->GPRx = 0;  // always succeed
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
  return NULL;
}