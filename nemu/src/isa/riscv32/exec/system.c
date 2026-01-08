#include "cpu/exec.h"
#include "rtl/rtl.h"

make_EHelper(ecall) {
  raise_intr(11, cpu.pc);
  print_asm("ecall");
}

make_EHelper(csrrw) {
  uint32_t csr_num = id_src->val;
  uint32_t rs1_val = reg_l(id_src2->reg);
  
  uint32_t csr_val = 0;
  switch(csr_num) {
    case 0x100:  // sstatus
      csr_val = cpu.sstatus;
      break;
    case 0x102:  // sepc
      csr_val = cpu.sepc;
      break;
    case 0x104:  // stvec
    case 0x105:  // stvec (alternative encoding)
      csr_val = cpu.stvec;
      break;
    case 0x142:  // scause
      csr_val = cpu.scause;
      break;
    default:
      printf("Unknown CSR: 0x%x\n", csr_num);
      assert(0);
  }

  if (id_dest->reg != 0) {
    rtl_sr(id_dest->reg, &csr_val, 4);
  }
  
  switch(csr_num) {
    case 0x100:  // sstatus
      cpu.sstatus = rs1_val;
      break;
    case 0x102:  // sepc
      cpu.sepc = rs1_val;
      break;
    case 0x104:  // stvec
    case 0x105:  // stvec (alternative)
      cpu.stvec = rs1_val;
      break;
    case 0x142:  // scause
      cpu.scause = rs1_val;
      break;
    default:
      assert(0);
  }
  
  print_asm("csrrw %s, 0x%x, %s", reg_name(id_dest->reg, 4), csr_num, reg_name(id_src2->reg, 4));
}

make_EHelper(csrrs) {
  uint32_t csr_num = id_src->val;
  uint32_t rs1_val = reg_l(id_src2->reg);
  
  uint32_t csr_val = 0;
  switch(csr_num) {
    case 0x100:  // sstatus
      csr_val = cpu.sstatus;
      break;
    case 0x102:  // sepc
      csr_val = cpu.sepc;
      break;
    case 0x104:  // stvec
    case 0x105:  // stvec (alternative)
      csr_val = cpu.stvec;
      break;
    case 0x142:  // scause
      csr_val = cpu.scause;
      break;
    default:
      printf("Unknown CSR: 0x%x\n", csr_num);
      assert(0);
  }
  
  if (id_dest->reg != 0) {
    rtl_sr(id_dest->reg, &csr_val, 4);
  }
  
  if (id_src2->reg != 0) {
    switch(csr_num) {
      case 0x100:  // sstatus
        cpu.sstatus = cpu.sstatus | rs1_val;
        break;
      case 0x102:  // sepc
        cpu.sepc = cpu.sepc | rs1_val;
        break;
      case 0x104:  // stvec
      case 0x105:  // stvec (alternative)
        cpu.stvec = cpu.stvec | rs1_val;
        break;
      case 0x142:  // scause
        cpu.scause = cpu.scause | rs1_val;
        break;
      default:
        assert(0);
    }
  }
  
  print_asm("csrrs %s, 0x%x, %s", reg_name(id_dest->reg, 4), csr_num, reg_name(id_src2->reg, 4));
}

make_EHelper(csrrc) {
  uint32_t csr_num = id_src->val;
  uint32_t rs1_val = reg_l(id_src2->reg);
  
  uint32_t csr_val = 0;
  switch(csr_num) {
    case 0x100:  // sstatus
      csr_val = cpu.sstatus;
      break;
    case 0x102:  // sepc
      csr_val = cpu.sepc;
      break;
    case 0x104:  // stvec
    case 0x105:  // stvec
      csr_val = cpu.stvec;
      break;
    case 0x142:  // scause
      csr_val = cpu.scause;
      break;
    default:
      printf("Unknown CSR: 0x%x\n", csr_num);
      assert(0);
  }
  
  if (id_dest->reg != 0) {
    rtl_sr(id_dest->reg, &csr_val, 4);
  }
  
  if (id_src2->reg != 0) {
    switch(csr_num) {
      case 0x100:  // sstatus
        cpu.sstatus = cpu.sstatus & ~rs1_val;
        break;
      case 0x102:  // sepc
        cpu.sepc = cpu.sepc & ~rs1_val;
        break;
      case 0x104:  // stvec
      case 0x105:  // stvec
        cpu.stvec = cpu.stvec & ~rs1_val;
        break;
      case 0x142:  // scause
        cpu.scause = cpu.scause & ~rs1_val;
        break;
      default:
        assert(0);
    }
  }
  
  print_asm("csrrc %s, 0x%x, %s", reg_name(id_dest->reg, 4), csr_num, reg_name(id_src2->reg, 4));
}

make_EHelper(sret) {
  decinfo.jmp_pc = cpu.sepc + 4;
  decinfo.is_jmp = 1;
  print_asm("sret");
}

make_EHelper(system) {
  switch (decinfo.isa.instr.funct3) {
    case 0x0:  // ecall/ebreak等
      if (decinfo.isa.instr.rd == 0 && 
          decinfo.isa.instr.rs1 == 0 &&
          decinfo.isa.instr.funct7 == 0) {
        exec_ecall(pc);
      } else {
        assert(0);
      }
      break;
    case 0x1:  // csrrw
      exec_csrrw(pc);
      break;
    case 0x2:  // csrrs
      exec_csrrs(pc);
      break;
    case 0x3:  // csrrc
      exec_csrrc(pc);
      break;
    case 0x5:  // 根据csr字段区分
      if (decinfo.isa.instr.csr == 0x102) {  // sret
        exec_sret(pc);
      } else {
        assert(0);
      }
      break;
    default:
      assert(0);
  }
}
