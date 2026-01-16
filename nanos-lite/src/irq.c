#include "common.h"

_Context* schedule(_Context *prev);
_Context* do_syscall(_Context *c);

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_YIELD:
      Log("Get _EVENT_YIELD, will return to user program...");
      return schedule(c);//传入上下文c返回新的上下文
      break;
    case _EVENT_SYSCALL:
      _Context *next = do_syscall(c);
      if (next != NULL) {
        return next;  // SYS_exit/execve 返回新进程上下文
      }
      return schedule(c);
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
