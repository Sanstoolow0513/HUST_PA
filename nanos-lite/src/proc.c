#include "proc.h"

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
  //保存当前上下文
  current->cp = prev;
  //选择下一个进程/线程 (pcb[0] <-> pcb[1])
  current = (current == &pcb[0] ? &pcb[1] : &pcb[0]);
  //返回新进程上下文
  return current->cp;
}

void init_proc() {
  switch_boot_pcb();

  Log("Initializing processes...");

  // load program here PA3
  // naive_uload(NULL, "/bin/text");
  // naive_uload(NULL, NULL);


  // 内核进程hello world pcb[0]
  _Area stack;
  stack.start = pcb[0].stack;
  stack.end = pcb[0].stack + sizeof(pcb[0].stack);
  pcb[0].cp = _kcontext(stack, hello_fun, "Thread-0");
  
  // 内核进程hello world pcb[1]
  stack.start = pcb[1].stack;
  stack.end = pcb[1].stack + sizeof(pcb[1].stack);
  pcb[1].cp = _kcontext(stack, hello_fun, "Thread-1");
}