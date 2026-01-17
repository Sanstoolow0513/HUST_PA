#include <am.h>
#include <riscv32.h>

void __am_get_cur_as(_Context *c);
void __am_switch(_Context *c);

static _Context* (*user_handler)(_Event, _Context*) = NULL;

_Context* __am_irq_handle(_Context *c) {
    __am_get_cur_as(c);
  _Context *next = c;
  if (user_handler) {
    _Event ev = {0};
    switch (c->scause) {
      case 11:
        if ((int32_t)c->gpr[17] == -1) {
          ev.event = _EVENT_YIELD;
        } else {
          ev.event = _EVENT_SYSCALL;
        }
        break;
      case 0x80000005:  // 时钟中断（IRQ_TIMER）
        ev.event = _EVENT_IRQ_TIMER;
        break;
      default: 
        ev.event = _EVENT_ERROR; 
        break;
    }
    next = user_handler(ev, c);
    if (next == NULL) {
      next = c;
    }
  }
  __am_switch(next);
  return next;
}


extern void __am_asm_trap(void);

int _cte_init(_Context*(*handler)(_Event, _Context*)) {
  // initialize exception entry
  asm volatile("csrw stvec, %0" : : "r"(__am_asm_trap));

  // register event handler
  user_handler = handler;

  _intr_write(1);

  return 0;
}

_Context *_kcontext(_Area stack, void (*entry)(void *), void *arg) {
  //将栈顶指针定位到 stack.end，并向下预留 _Context 的大小
  _Context *c = (_Context*)stack.end - 1; 
  // 填充上下文
  c->sepc = (uintptr_t)entry;     // 入口地址
  c->sstatus = 0x1800;            // 状态寄存器: MPP=11 (Machine Mode), MPIE=1 (Enable Interrupt)
  c->gpr[10] = (uintptr_t)arg;    // a0 传递的参数
  c->as = NULL;                   // 内核线程使用内核地址空间
  return c;
}

void _yield() {
  asm volatile("li a7, -1; ecall");
}

int _intr_read() {
  uint32_t sstatus;
  asm volatile("csrr %0, sstatus" : "=r"(sstatus));
  return (sstatus & 0x2) != 0; 
}

void _intr_write(int enable) {
  if (enable) {
    asm volatile("csrsi sstatus, 0x2");  // 设置 SIE
  } else {
    asm volatile("csrci sstatus, 0x2");  // 清除 SIE
  }
}
