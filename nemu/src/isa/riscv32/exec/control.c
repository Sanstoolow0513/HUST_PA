#include "cpu/exec.h"

make_EHelper(jal){
    //pc+4
    rtl_addi(&s0, &cpu.pc, 4);
    rtl_sr(id_dest->reg, &s0, 4);

    //jal
    rtl_add(&decinfo.jmp_pc, &cpu.pc, &id_src->val);
    decinfo.is_jmp = 1;
    printf("JAL DEBUG: cpu.pc=0x%x, src->val=0x%x, jmp_pc=0x%x, *pc=0x%x\n",
            (uint32_t)cpu.pc, id_src->val, (uint32_t)decinfo.jmp_pc, *pc);
    print_asm_template2(jal);
}

make_EHelper(jalr) {
    // jalr: pc = rs1 + imm, rd = pc+4
    rtl_addi(&s0, &cpu.pc, 4);
    rtl_sr(id_dest->reg, &s0, 4);
    rtl_add(&decinfo.jmp_pc, &id_src->val, &id_src2->val);
    decinfo.is_jmp = 1;

    print_asm_template3(jalr);
  }
