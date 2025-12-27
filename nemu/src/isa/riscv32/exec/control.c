#include "cpu/exec.h"

make_EHelper(jal){
    //pc+4
    rtl_addi(&s0, pc, 4);
    rtl_sr(id_dest->reg, &s0, 4);

    //jal
    rtl_add(&decinfo.jmp_pc, pc, &id_src->val);
    decinfo.is_jmp = 1;
    
    print_asm_template2(jal);
}
