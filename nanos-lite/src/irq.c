#include "common.h"

static _Context* do_event(_Event e, _Context* c) {
  switch (e.event) {
    case _EVENT_YIELD:
      Log("Get _EVENT_YIELD, will return to user program...");
      return schedule(c);//传入上下文c返回新的上下文
      break;
    case _EVENT_SYSCALL:
      do_syscall(c);
      break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  _cte_init(do_event);
}
