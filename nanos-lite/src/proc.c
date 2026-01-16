#include "proc.h"

void context_uload(PCB *pcb, const char *filename);

#define MAX_NR_PROC 4

static PCB pcb[MAX_NR_PROC] __attribute__((used)) = {};
static PCB pcb_boot = {};
PCB *current = NULL;

void switch_boot_pcb() {
  current = &pcb_boot;
}

void hello_fun(void *arg) {
  int j = 1;
  while (1) {
    Log("Hello World from Nanos-lite for the %dth time! (%s)", j, (char *)arg);
    j ++;
    _yield();
  }
}

_Context* schedule(_Context *prev) {
  // 保存当前上下文
  current->cp = prev;
  
  // 简单的轮转调度：在 pcb[0] 和 pcb[1] 之间切换
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);

  // 返回新进程上下文
  return current->cp;
}

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // PA4.2 测试：加载用户进程
  // 为了体现多道程序与虚存切换（PA4.2非抢占），我们需要两个都会主动 yield 的程序
  context_uload(&pcb[0], "/bin/dummy");
  context_uload(&pcb[1], "/bin/dummy");
  // context_uload(&pcb[2], "/bin/text");
  // context_uload(&pcb[3], "/bin/events");

  
  // 可选：双进程测试
  // context_uload(&pcb[0], "/bin/hello");
  // context_kload(&pcb[1], hello_fun);
}