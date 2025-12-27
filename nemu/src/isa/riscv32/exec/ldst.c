#include "cpu/exec.h"

make_EHelper(ld) {
  rtl_lm(&s0, &id_src->addr, decinfo.width);
  if (decinfo.width == 1) {
    if (s0 & 0x80) {
      s0 |= 0xffffff00;
    }
  } else if (decinfo.width == 2) {
    if (s0 & 0x8000) {
      s0 |= 0xffff0000;
    }
  }
  rtl_sr(id_dest->reg, &s0, 4);
  switch (decinfo.width) {
    case 4: print_asm_template2(lw); break;
    case 2: print_asm_template2(lh); break;
    case 1: print_asm_template2(lb); break;
    default: assert(0);
  }
}

make_EHelper(st) {
  rtl_sm(&id_src->addr, &id_dest->val, decinfo.width);

  switch (decinfo.width) {
    case 4: print_asm_template2(sw); break;
    case 2: print_asm_template2(sh); break;
    case 1: print_asm_template2(sb); break;
    default: assert(0);
  }
}
