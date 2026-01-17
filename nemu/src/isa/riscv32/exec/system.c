#include "cpu/exec.h"
#include "rtl/rtl.h"

void raise_intr(uint32_t NO, vaddr_t epc);

make_EHelper(system) {
  switch (decinfo.isa.instr.funct3) {
    case 0x0:  // ecall/ebreak/sret等
      if (decinfo.isa.instr.rd == 0 && 
          decinfo.isa.instr.rs1 == 0) {
        // 需要通过funct7区分
        if (decinfo.isa.instr.funct7 == 0) {
          // ecall
          raise_intr(11, cpu.pc);
          print_asm("ecall");
        } else if (decinfo.isa.instr.funct7 == 0x08) {
          // sret: 0x10200073, funct7=0x08 (实际bits[31:25]=0001000)
          decinfo.jmp_pc = cpu.sepc + 4;
          decinfo.is_jmp = 1;
          print_asm("sret");
        } else {
          assert(0);
        }
      } else {
        assert(0);
      }
      break;
    case 0x1: {  // csrrw
      uint32_t csr_num = decinfo.isa.instr.csr;
      uint32_t rs1_val = reg_l(decinfo.isa.instr.rs1);
      uint32_t rd = decinfo.isa.instr.rd;
      
      uint32_t csr_val = 0;
      switch(csr_num) {
        case 0x100: csr_val = cpu.sstatus; break;
        case 0x102: csr_val = cpu.sepc; break;
        case 0x104: 
        case 0x105: csr_val = cpu.stvec; break;
        case 0x141: csr_val = cpu.sepc; break;  
        case 0x142: csr_val = cpu.scause; break;
        case 0x180: csr_val = cpu.satp; break;
        default: printf("Unknown CSR: 0x%x\n", csr_num); assert(0);
      }
      
      if (rd != 0) {
        rtl_sr(rd, &csr_val, 4);
      }
      
      switch(csr_num) {
        case 0x100: cpu.sstatus = rs1_val; break;
        case 0x102: cpu.sepc = rs1_val; break;
        case 0x104: 
        case 0x105: cpu.stvec = rs1_val; break;
        case 0x141: cpu.sepc = rs1_val; break;  
        case 0x142: cpu.scause = rs1_val; break;
        case 0x180: cpu.satp = rs1_val; break;
        default: assert(0);
      }
      
      print_asm("csrrw %s, 0x%x, %s", reg_name(rd, 4), csr_num, reg_name(decinfo.isa.instr.rs1, 4));
      break;
    }
    
    case 0x2: {  // csrrs
      uint32_t csr_num = decinfo.isa.instr.csr;
      uint32_t rs1_val = reg_l(decinfo.isa.instr.rs1);
      uint32_t rd = decinfo.isa.instr.rd;
      
      uint32_t csr_val = 0;
      switch(csr_num) {
        case 0x100: csr_val = cpu.sstatus; break;
        case 0x102: csr_val = cpu.sepc; break;
        case 0x104: 
        case 0x105: csr_val = cpu.stvec; break;
        case 0x141: csr_val = cpu.sepc; break; 
        case 0x142: csr_val = cpu.scause; break;
        case 0x180: csr_val = cpu.satp; break;
        default: printf("Unknown CSR: 0x%x\n", csr_num); assert(0);
      }
      
      if (rd != 0) {
        rtl_sr(rd, &csr_val, 4);
      }
      
      if (decinfo.isa.instr.rs1 != 0) {
        switch(csr_num) {
          case 0x100: cpu.sstatus |= rs1_val; break;
          case 0x102: cpu.sepc |= rs1_val; break;
          case 0x104: 
          case 0x105: cpu.stvec |= rs1_val; break;
          case 0x141: cpu.sepc |= rs1_val; break;  
          case 0x142: cpu.scause |= rs1_val; break;
          case 0x180: cpu.satp |= rs1_val; break;
          default: assert(0);
        }
      }
      
      print_asm("csrrs %s, 0x%x, %s", reg_name(rd, 4), csr_num, reg_name(decinfo.isa.instr.rs1, 4));
      break;
    }
    
    case 0x3: {  // csrrc
      uint32_t csr_num = decinfo.isa.instr.csr;
      uint32_t rs1_val = reg_l(decinfo.isa.instr.rs1);
      uint32_t rd = decinfo.isa.instr.rd;
      
      uint32_t csr_val = 0;
      switch(csr_num) {
        case 0x100: csr_val = cpu.sstatus; break;
        case 0x102: csr_val = cpu.sepc; break;
        case 0x104: 
        case 0x105: csr_val = cpu.stvec; break;
        case 0x141: csr_val = cpu.sepc; break; 
        case 0x142: csr_val = cpu.scause; break;
        case 0x180: csr_val = cpu.satp; break;
        default: printf("Unknown CSR: 0x%x\n", csr_num); assert(0);
      }
      
      if (rd != 0) {
        rtl_sr(rd, &csr_val, 4);
      }
      
      if (decinfo.isa.instr.rs1 != 0) {
        switch(csr_num) {
          case 0x100: cpu.sstatus &= ~rs1_val; break;
          case 0x102: cpu.sepc &= ~rs1_val; break;
          case 0x104: 
          case 0x105: cpu.stvec &= ~rs1_val; break;
          case 0x141: cpu.sepc &= ~rs1_val; break;
          case 0x142: cpu.scause &= ~rs1_val; break;
          case 0x180: cpu.satp &= ~rs1_val; break;
          default: assert(0);
        }
      }
      
      print_asm("csrrc %s, 0x%x, %s", reg_name(rd, 4), csr_num, reg_name(decinfo.isa.instr.rs1, 4));
      break;
    }
    
    case 0x6: {  // csrrsi (CSR read and set bits immediate)
      uint32_t csr_num = decinfo.isa.instr.csr;
      uint32_t zimm = decinfo.isa.instr.rs1;  // 5-bit immediate in rs1 field
      uint32_t rd = decinfo.isa.instr.rd;
      
      uint32_t csr_val = 0;
      switch(csr_num) {
        case 0x100: csr_val = cpu.sstatus; break;
        case 0x102: csr_val = cpu.sepc; break;
        case 0x104: 
        case 0x105: csr_val = cpu.stvec; break;
        case 0x141: csr_val = cpu.sepc; break;  
        case 0x142: csr_val = cpu.scause; break;
        case 0x180: csr_val = cpu.satp; break;
        default: printf("Unknown CSR: 0x%x\n", csr_num); assert(0);
      }
      
      if (rd != 0) {
        rtl_sr(rd, &csr_val, 4);
      }
      
      if (zimm != 0) {  // Only update if immediate is non-zero
        switch(csr_num) {
          case 0x100: cpu.sstatus |= zimm; break;
          case 0x102: cpu.sepc |= zimm; break;
          case 0x104: 
          case 0x105: cpu.stvec |= zimm; break;
          case 0x141: cpu.sepc |= zimm; break;  
          case 0x142: cpu.scause |= zimm; break;
          case 0x180: cpu.satp |= zimm; break;
          default: assert(0);
        }
      }
      
      print_asm("csrrsi %s, 0x%x, %d", reg_name(rd, 4), csr_num, zimm);
      break;
    }
    
    case 0x7: {  // csrrci (CSR read and clear bits immediate)
      uint32_t csr_num = decinfo.isa.instr.csr;
      uint32_t zimm = decinfo.isa.instr.rs1;  // 5-bit immediate in rs1 field
      uint32_t rd = decinfo.isa.instr.rd;
      
      uint32_t csr_val = 0;
      switch(csr_num) {
        case 0x100: csr_val = cpu.sstatus; break;
        case 0x102: csr_val = cpu.sepc; break;
        case 0x104: 
        case 0x105: csr_val = cpu.stvec; break;
        case 0x141: csr_val = cpu.sepc; break; 
        case 0x142: csr_val = cpu.scause; break;
        case 0x180: csr_val = cpu.satp; break;
        default: printf("Unknown CSR: 0x%x\n", csr_num); assert(0);
      }
      
      if (rd != 0) {
        rtl_sr(rd, &csr_val, 4);
      }
      
      if (zimm != 0) {  // Only update if immediate is non-zero
        switch(csr_num) {
          case 0x100: cpu.sstatus &= ~zimm; break;
          case 0x102: cpu.sepc &= ~zimm; break;
          case 0x104: 
          case 0x105: cpu.stvec &= ~zimm; break;
          case 0x141: cpu.sepc &= ~zimm; break;
          case 0x142: cpu.scause &= ~zimm; break;
          case 0x180: cpu.satp &= ~zimm; break;
          default: assert(0);
        }
      }
      
      print_asm("csrrci %s, 0x%x, %d", reg_name(rd, 4), csr_num, zimm);
      break;
    }
    
    case 0x5:  
      assert(0); // funct3=5不应该出现
      break;
    default:
      assert(0);
  }
}

// 保留ecall单独的helper（如果需要）
make_EHelper(ecall) {
  raise_intr(11, cpu.pc);
  print_asm("ecall");
}
