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

make_EHelper(auipc) {
  // sp = PC + (imm << 12)
  rtl_add(&s0, pc, &id_src->val);
  rtl_sr(id_dest->reg, &s0, 4);

  print_asm_template2(auipc);
}

make_EHelper(alu_r) {
  switch (decinfo.isa.instr.funct3) {
    case 0x0:  // add/sub
      switch (decinfo.isa.instr.funct7) {
        case 0x00:  // add
          rtl_add(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(add);
          break;
        case 0x20:  // sub
          rtl_sub(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(sub);
          break;
        case 0x01:  // mul
          rtl_mul_lo(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(mul);
          break;
        default:
          assert(0);
      }
      break;
    default:
      assert(0);
  }
}
