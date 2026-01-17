#include "nemu.h"

void dev_raise_intr() {
  extern CPU_state cpu;
  cpu.INTR = true;
}
