#include "cpu/exec.h"

make_EHelper(lui) {
  rtl_sr(id_dest->reg, &id_src->val, 4);

  print_asm_template2(lui);
}

make_EHelper(alu_i){
  switch (decinfo.isa.instr.funct3){
    case 0x0:  // addi
      rtl_addi(&s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(addi);
      break;

    case 0x1:  // slli
      {
      rtlreg_t shamt_val = id_src2->imm & 0x1F;
      rtl_shl(&s0, &id_src->val, &shamt_val);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(slli);
      }
      break;

    case 0x2:  // slti 
      rtl_setrelopi(RELOP_LT, &s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(slti);
      break;

    case 0x3:  // sltiu 
      rtl_setrelopi(RELOP_LTU, &s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(sltiu);
      break;

    case 0x4:  // xori
      rtl_xori(&s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(xori);
      break;
    
    case 0x5:  // srli or srai
      {
      rtlreg_t shamt_val = id_src2->imm & 0x1F;
      if (decinfo.isa.instr.funct7 == 0x00) {  // srli
        rtl_shr(&s0, &id_src->val, &shamt_val);
        print_asm_template3(srli);
      } else if (decinfo.isa.instr.funct7 == 0x20) {  // srai
        rtl_sar(&s0, &id_src->val, &shamt_val);
        print_asm_template3(srai);
      } else {
        assert(0);
      }
      rtl_sr(id_dest->reg, &s0, 4);
      }
      break;
    case 0x6:  // ori
      rtl_ori(&s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(ori);
      break;

    case 0x7:  // andi
      rtl_andi(&s0, &id_src->val, id_src2->imm);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(andi);
      break;

    default:
      printf("alu_i: unknown funct3=0x%x\n", decinfo.isa.instr.funct3);
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
    case 0x0:  // add/sub/mul
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
          printf("alu_r: unknown funct7=0x%x (funct3=0x0)\n", decinfo.isa.instr.funct7);
          assert(0);
      }
      break;

    case 0x1:  // sll (shift left logical)
      rtl_shl(&s0, &id_src->val, &id_src2->val);
      rtl_sr(id_dest->reg, &s0, 4);
      print_asm_template3(sll);
      break;

    case 0x4:  // xor/div
      switch (decinfo.isa.instr.funct7) {
        case 0x00:  // xor
          rtl_xor(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(xor);
          break;
        case 0x01:  // div
          rtl_idiv_q(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(div);
          break;
        default:
          printf("alu_r: unknown funct7=0x%x (funct3=0x4)\n", decinfo.isa.instr.funct7);
          assert(0);
      }
      break;

    case 0x5:  // srl/sra/divu
      switch (decinfo.isa.instr.funct7) {
        case 0x00:  // srl
          rtl_shr(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(srl);
          break;
        case 0x20:  // sra
          rtl_sar(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(sra);
          break;
        case 0x01:  // divu
          rtl_div_q(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(divu);
          break;
        default:
          printf("alu_r: unknown funct7=0x%x (funct3=0x5)\n", decinfo.isa.instr.funct7);
          assert(0);
      }
      break;

    case 0x6:  // or/rem
      switch (decinfo.isa.instr.funct7) {
        case 0x00:  // or
          rtl_or(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(or);
          break;
        case 0x01:  // rem
          rtl_idiv_r(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(rem);
          break;
        default:
          printf("alu_r: unknown funct7=0x%x (funct3=0x6)\n", decinfo.isa.instr.funct7);
          assert(0);
      }
      break;

    case 0x7:  // and/remu
      switch (decinfo.isa.instr.funct7) {
        case 0x00:  // and
          rtl_and(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(and);
          break;
        case 0x01:  // remu
          rtl_div_r(&s0, &id_src->val, &id_src2->val);
          rtl_sr(id_dest->reg, &s0, 4);
          print_asm_template3(remu);
          break;
        default:
          printf("alu_r: unknown funct7=0x%x (funct3=0x7)\n", decinfo.isa.instr.funct7);
          assert(0);
      }
      break;


    default:
      printf("alu_r: unknown funct3=0x%x, funct7=0x%x\n", decinfo.isa.instr.funct3, decinfo.isa.instr.funct7);
      assert(0);
  }
}