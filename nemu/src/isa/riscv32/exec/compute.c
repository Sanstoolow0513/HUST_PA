#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(alu_i){
  //funct3字段
  switch (decinfo.isa.instr.funct3){
    case 0x0://addi
      rtl_addi(&s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(addi);

      break;
    default:
      assert(0);
  }
}
