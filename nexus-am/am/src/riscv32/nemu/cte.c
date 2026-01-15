#include <am.h>
#include <riscv32.h>

static _Context* (*user_handler)(_Event, _Context*) = NULL;

_Context* __am_irq_handle(_Context *c) {
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
    switch (c->scause) {
      case 11:  // ecall
        if ((int32_t)c->gpr[17] == -1) {  // a7 == -1 means yield
          ev.event = _EVENT_YIELD;
        } else {
          ev.event = _EVENT_SYSCALL;
        }
        break;
      default: ev.event = _EVENT_ERROR; break;
    }

    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }

  return next;
}


extern void __am_asm_trap(void);

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  //将栈顶指针定位到 stack.end，并向下预留 _Context 的大小
  _Context *c = (_Context*)stack.end - 1; 
  // 填充上下文
  c->sepc = (uintptr_t)entry;     // 入口地址
  c->sstatus = 0x1800;            // 状态寄存器: MPP=11 (Machine Mode), MPIE=1 (Enable Interrupt)
  c->gpr[10] = (uintptr_t)arg;    // a0 传递的参数
  return c;
}

void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  return 0;
}

void _intr_write(int enable) {
}
