#include "cpu/exec.h"




make_EHelper(ecall){


    raise_intr(11, cpu.pc);

    decinfo.is_jmp = 1;
    decinfo.jmp_pc = cpu.stvec;  // 假设stvec在CPU_state中
    
    print_asm("ecall");
}
make_EHelper(sret){
    

}