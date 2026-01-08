#include "cpu/exec.h"

make_EHelper(jal){
    //pc+4
    rtl_addi(&s0, &cpu.pc, 4);
    rtl_sr(id_dest->reg, &s0, 4);

    //jal
    rtl_add(&decinfo.jmp_pc, &cpu.pc, &id_src->val);
    decinfo.is_jmp = 1;
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

make_EHelper(branch) {
    bool taken = false;
    switch (decinfo.isa.instr.funct3) {
        case 0x0:  // beq
        taken = (id_src->val == id_src2->val);
        print_asm_template3(beq);
        break;
        case 0x1:  // bne
        taken = (id_src->val != id_src2->val);
        print_asm_template3(bne);
        break;
        case 0x4:  // blt
        taken = ((int32_t)id_src->val < (int32_t)id_src2->val);
        print_asm_template3(blt);
        break;
        case 0x5:  // bge
        taken = ((int32_t)id_src->val >= (int32_t)id_src2->val);
        print_asm_template3(bge);
        break;
        case 0x6:  // bltu
        taken = (id_src->val < id_src2->val);
        print_asm_template3(bltu);
        break;
        case 0x7:  // bgeu
        taken = (id_src->val >= id_src2->val);
        print_asm_template3(bgeu);
        break;
        default:
        assert(0);
    }

    if (taken) {
        rtl_add(&decinfo.jmp_pc, &cpu.pc, &id_dest->val);
        decinfo.is_jmp = 1;
    }
}
