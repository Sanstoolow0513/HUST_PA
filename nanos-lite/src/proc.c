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
  // PA4.2 测试：只运行 pcb[0]
  current = &pcb[0];
  // 返回新进程上下文
  return current->cp;
}

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // PA4.2 测试：加载用户进程
  context_uload(&pcb[0], "/bin/dummy");
  
  // 可选：双进程测试
  // context_uload(&pcb[0], "/bin/hello");
  // context_kload(&pcb[1], hello_fun);
}