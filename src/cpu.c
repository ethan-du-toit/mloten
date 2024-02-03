#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
//#include <time.h>
#include <stdbool.h>
#include "utility.h"
#include "cpu.h"
#include "interrupts.h"

struct instruction {
  i32 opcode, imm, rs1, rs2, rd, funct7, funct3;
};

void cpu_tick(u32* restrict pc, u8* restrict ram, u32* restrict rf) { /* only called from ml - inline please! */
  /* This is the second half of interrupt processing beginning. */
  i32 mip, mie, mtvec, mcause;
  mip = l(0x441510);
  mie = l(0x441410);    
  /* Here's a nice little extension. It shouldn't matter to user code. */
  /* But you can use it if you like. */
  /* 7CO CSR holds an i32 (only ever 1 or 0 - bit wasteful, but hell.) */
  /*                                          ^^^^^^^^^^^^ haha its wasteful of bits. geddit */
  /* This is mep - Exception Pending? */
  /* (Since it's only ever 1 or 0 I can just use ram[0x440fc0]) */
  if(mip || ram[0x440fc0]) {
    /* INTERRUPT */
#ifdef DEBUG      
    printf("MIP %x %x\n", mip, mie);
#endif      
    if(ram[0x440fc0] || mip & mie) { /* THIS INTERRUPT IS ENABLED */
      s(0, 0x441510); /* Hahahaha get outta here you other interrupts */
      s(0, 0x440fc0);
      mtvec = l(0x441414);
      mcause = l(0x441508);
      mcause &= 0x3f; /*cause goes to 63. no more.*/
      if (mip) {
	s(*pc, 0x441504); /* store pc (instruction after) into mepc */
      } else {
	s((*pc)-4, 0x441504); /* store pc into mepc */
      }
      /* for exceptions epc contains the instruction that caused the exception */
      /* IDK why. its standard */
#ifdef DEBUG      
      printf("%x %x mtvec\n", mtvec, mtvec&~3);
#endif      
      if(mip && mtvec & 3) { /* exceptions go to 0 */
	/* default mode - vectored */
	*pc = (mtvec & ~3) + mcause*4;
      } else {
	*pc = (mtvec & ~3);
      }
#ifdef DEBUG      
      printf("%x\n", *pc);
#endif
    }
  }
#ifdef DEBUG    
  printf("PC: 0x%x\n", *pc);
  printf("Registers: \n");
    
  for (i32 i = 0; i < 32; i++) {
    printf("%d: %d (%x)\n", i, rf[i], rf[i]);
  }
#endif  
  u32 current;
  current = l(*pc);
  struct instruction inst;
  i32 opcode, imm, rs1, rs2, rd, funct7, funct3;
  opcode = current & 0x7f;
#ifdef DEBUG
#define DB_X() {printf("Cur: %x\n", current);				\
  printf("OP: %x\nIMM: %x\nRS1: %x\nRS2: %x\nRD: %x\nFUNCT7: %x\nFUNCT3: %x\n", \
	 opcode,  imm,     rs1,     rs2,     rd,     funct7,     funct3);}
#else
#define DB_X()
#endif
  switch (opcode) {
  case 0x23: { /* store format */
    /* printf("S\n"); */
    rs2 = (current & 0x1f00000)  >> 20;
    rs1 = (current & 0xf8000)    >> 15;
    funct3 = (current & 0x7000)  >> 12;
    u32 in_imm =
      ((current & 0xfe000000) >> 20) +
      ((current & 0xf80) >> 7);
    i32 sign = current & 0x80000000;
    imm = sign ? in_imm | 0xfffff000 : in_imm;
    DB_X();
    goto store;
  } break;
  case 0x63: { /* branch format */
    /* printf("B\n"); */
    rs2 = (current & 0x1f00000)  >> 20;
    rs1 = (current & 0xf8000)    >> 15;
    funct3 = (current & 0x7000)  >> 12;
    i32 sign = current & 0x80000000;
    u32 in_imm =
      ((current & 0x80) << 4) /* bit 11 */
      + ((current & 0x7e000000) >> 20) /* bits 10:5 */
      + ((current & 0xf00) >> 7) /* bits 4:1 - bit 0 always 0 */
      ;
    imm = sign ? in_imm | 0xfffff000 : in_imm;
    DB_X();    
    goto branch;
  } break;
  case 0x6f: { /* jump format */
    rd = ((current & 0xf80) >> 7); 
    /* begin */
    i32 sign = current & 0x80000000;
    u32 in_imm =
      (current & 0xff000) /* bits 19:12. Nice luck!*/
      + ((current & 0x100000) >> 9) /* bit 11 */
      + ((current & 0x7fc00000) >> 20)
      ;
    imm =  sign ? in_imm | 0xfff00000 : in_imm;
    DB_X();
    goto jump;
  } break;
  case 0x37:
  case 0x17: { /* U format */
    /* printf("U\n"); */
    rd = ((current & 0xf80) >> 7);
    imm = current & 0xfffff000; /* refreshingly simple */
    DB_X();    
    goto u;
  } break;
  case 0x33: { /* R format */
    /* printf("R\n"); */
    rs2 = (current & 0x1f00000)  >> 20;
    rs1 = (current & 0xf8000)    >> 15;
    funct3 = (current & 0x7000)  >> 12;
    rd = (current & 0xf80) >> 7;
    funct7 = (current & 0xfe000000) >> 25;
    DB_X();
    goto reg;
  } break;
  default: {/* I format */
    /* printf("I\n"); */
    rd = (current & 0xf80) >> 7;
    rs1 = (current & 0xf8000)    >> 15;
    funct3 = (current & 0x7000)  >> 12;
    u32 in_imm = (current & 0xfff00000) >> 20;
    i32 sign = current & 0x80000000;
    imm = sign ? in_imm | 0xfffff000 : in_imm;
    DB_X();
    goto imm;
  } break;
  }

 reg:
  switch (funct7) {
  case 0: {
    switch (funct3) {
    case 0:  {
      rf[rd] = rf[rs1] + rf[rs2]; /* ADD */ /* Verified */
    } break;
    case 1: {
      rf[rd] = rf[rs1] << (rf[rs2] & 0x1f); /* SLL */ 
    } break;
    case 2: {
      rf[rd] = ((i32) rf[rs1]) < ((i32) rf[rs2]); /* SLT */ /* Verified */
    } break;
    case 3: {
      rf[rd] = ((u32) rf[rs1]) < ((u32) rf[rs2]); /* SLTU */
    } break;
    case 4: {
      rf[rd] = rf[rs1] ^ rf[rs2]; /* XOR */ /* Verified */
    } break;
    case 5: {
      rf[rd] = ((u32) rf[rs1]) >> (rf[rs2] & 0x1f); /* SRL */
    } break;
    case 6: {
      rf[rd] = rf[rs1] | rf[rs2]; /* OR */ /* Verified */
    } break;
    case 7: {
      rf[rd] = rf[rs1] & rf[rs2]; /* AND */ /* Verified */
    } break;
    default:
      __builtin_unreachable();
    }
  } break;
  case 0x20: {
    switch (funct3) {
    case 0: {
      rf[rd] = rf[rs1] - rf[rs2]; /* SUB */ /* Verified */
    } break;
    case 5: {
      rf[rd] = ((i32) rf[rs1]) >>  (rf[rs2] & 0x1f); /* SRA */
    } break;
    default:
      __builtin_unreachable();
    }
  } break;
  case 1: { /* MULDIV */
    switch(funct3) {
    case 0: {
      rf[rd] = rf[rs1] * rf[rs2]; /* MUL */ /* Verified */
    } break;
    case 1: {
      rf[rd] = (((i64) rf[rs1]) * ((i64) rf[rs2])) >> 32; /* MULH */
    } break;
    case 2: {
      rf[rd] = (((i64) rf[rs1]) * ((u64) rf[rs2])) >> 32; /* MULHSU */
    } break;
    case 3: {
      rf[rd] = (((u64) rf[rs1]) * ((u64) rf[rs2])) >> 32; /* MULHU */
    } break;
    case 4: {
      rf[rd] = ((i32) rf[rs1]) / ((i32) rf[rs2]); /* DIV */ 
    } break;
    case 5: {
      rf[rd] = ((u32) rf[rs1]) / ((u32) rf[rs2]); /* DIVU */
    } break;
    case 6: {
      rf[rd] = rf[rs1] % rf[rs2]; /* REM */
    } break;
    case 7: {
      rf[rd] = ((u32) rf[rs1]) % ((u32) rf[rs2]); /* REMU */
    } break;
    default:
      __builtin_unreachable();
    }
  } break;
  default: {
    __builtin_unreachable();
  }}
  goto end;
 u:
  switch(opcode) {
  case 0x37: {
    rf[rd] = imm; /* LUI */
  } break;
  case 0x17: {
    printf("%x\n", imm);
    rf[rd] = imm + *pc; /* AUIPC */
  } break;
  }
  goto end;
 jump:
  rf[rd] = *pc + 4;        /* JAL */
  *pc = *pc - 4 + imm;     /* JAL */
#ifdef PJ
  printf("%x\n", *pc + 4);
#endif
  goto end;
 branch:
  switch (funct3) {
  case 0: {
    if ( rf[rs1] == rf[rs2]) { /* BEQ */
      *pc = *pc + imm - 4;      /* BEQ */
    }                         /* BEQ */
  } break;
  case 1: {
    if (rf[rs1] != rf[rs2]) { /* BNE */
      *pc = *pc + imm - 4;      /* BNE */
    }                         /* BNE */
  } break;
  case 4: {
    if (((i32) rf[rs1]) < ((i32) rf[rs2])) {  /* BLT - mmm! */
      *pc = *pc + imm - 4;      /* BLT */
    }                         /* BLT */
  } break;
  case 5: {
    if (((i32) rf[rs1]) >= ((i32) rf[rs2])) {  /* BGE */
      *pc = *pc + imm - 4;       /* BGE */
    } 
    /* BGE */        
  } break;
  case 6: {
    if (((u32) rf[rs1]) < ((u32) rf[rs2])) {  /* BLTU */
      *pc = *pc + imm - 4;                                        /* BLTU */
    }                                                 /* BLTU */
  } break;
  case 7: {
    if (((u32) rf[rs1]) >= ((u32) rf[rs2])) {  /* BGEU */
      *pc = *pc + imm - 4;                                         /* BGEU */
    }                                                           /* BGEU */
  } break;
  default:
    __builtin_unreachable();
  }
  goto end;
 store:
  switch (funct3) {
  case 0: {
    ram[rf[rs1] + imm] = (rf[rs2] & 0xff); /* SB */
  } break;
  case 1: {
    sh(rf[rs2], rf[rs1] + imm);            /* SH */
  } break;
  case 2: {
    s(rf[rs2], rf[rs1] + imm); /* SW */
  } break;
  default:
    __builtin_unreachable();
  }
  goto end;
 imm:
  switch (opcode) {
  case 0x67: {
    rf[rd] = *pc + 4;                              /* JALR */
    *pc = (((u32) rf[rs1] + (u32) imm) & ~1) - 4;    /* JALR */
  } break;    
  case 0x3: { /* load instructions */
    switch (funct3) {
    case 0: {
      rf[rd] = (i8) ram[rf[rs1] + imm]; /* LB */
    } break;
    case 1: {
      rf[rd] = (i16) lh((rf[rs1] + imm)); /* LH */
    } break;
    case 2: {
      rf[rd] = l((rf[rs1]+imm)); /* LW */
    } break;
    case 4: {
      rf[rd] = (u8) ram[rf[rs1] + imm]; /* LBU */
    } break;
    case 5: {
      rf[rd] = (u16) lh((rf[rs1] + imm)); /* LHU */
    } break;
    default:
      __builtin_unreachable();
    }
  } break;
  case 0xf: { /* fence instructions */
    ; /* FENCE / FENCE.I - implemented as NOPS as we have total ordering */
  } break;
  case 0x13: { /* arithmetic register-immediate operations */
    switch (funct3) {
    case 0: {
      rf[rd] = rf[rs1] + imm; /* ADDI */
    } break;
    case 2: {
      rf[rd] = (i32) rf[rs1] < imm; /* SLTI */
    } break;
    case 3: {
      rf[rd] = (u32) rf[rs1] < (u32) imm; /* SLTIU */
    } break;
    case 4: {
      rf[rd] = rf[rs1] ^ imm; /* XORI */
    } break;
    case 6: {
      rf[rd] = rf[rs1] | imm; /* ORI */
    } break;
    case 7: {
      rf[rd] = rf[rs1] & imm; /* XORI */
    } break;
    case 1: {
      rf[rd] = rf[rs1] << imm; /* SLLI */
    } break;
    case 5: {
      if (imm & 0x400) {
        i32 newimm = imm & ~0x400;
	rf[rd] = ((i32) rf[rs1]) >> newimm; /* SRAI */	
      } else {
         /* SRLI */
	rf[rd] = ((u32) rf[rs1]) >> imm;
      }
    } break;
    default:
      __builtin_unreachable();
    }
  } break;
  case 0x73:  { /* SYSTEM */
    /* Available - rs1, rd, imm, funct3 */
    switch (funct3) {
    case 1: {
      i32 addr = imm * 4 + 0x440800;    /* CSRRW */
      rf[rd] = l(addr);                 /* CSRRW */
      s(rf[rs1], addr);                /* CSRRW */
    } break;
    case 2: {
      i32 addr = imm * 4 + 0x440800;    /* CSRRS */
      rf[rd] = l(addr);                 /* CSRRS */
      s((rf[rd] | rf[rs1]), addr);     /* CSRRS */
    } break;
    case 3: {
      i32 addr = imm * 4 + 0x440800;    /* CSRRC */
      rf[rd] = l(addr);                 /* CSRRC */
      s((rf[rd] & ~rf[rs1]), addr);    /* CSRRC */
    } break;
    case 5: {                 /* For the following immediates, the immediate is stored in rs1 
			       * (not the register, the variable). Helpful! No 0-extendin nec. */
      i32 addr = imm * 4 + 0x440800;    /* CSRRWI */
      rf[rd] = l(addr);                 /* CSRRWI */
      s(rf[rs1], rs1);                 /* CSRRWI */
    } break;
    case 6: {
      i32 addr = imm * 4 + 0x440800;    /* CSRRSI */
      rf[rd] = l(addr);                 /* CSRRSI */
      s((rf[rd] | rs1), addr);         /* CSRRSI */
    } break;
    case 7: {
      i32 addr = imm * 4 + 0x440800;    /* CSRRCI */
      rf[rd] = l(addr);                 /* CSRRCI */
      s((rf[rd] & ~rs1), addr);        /* CSRRCI */	
    } break;
    case 0: {/* system x */
      switch(imm) {
      case 1: {
	if(queue_interrupt(0xb, 1, 1, ram) == -1) { /* ECALL */
	  /* There's an interrupt about to happen, next cycle. */
	  /* Every other instruction has already completed its work. */
	  /* Not me. I need them to jump back to me. */
	  *pc -= 4; /* ECALL */
	}
      } break;
      case 0: {
	if(queue_interrupt(0x3, 1, 1, ram) == -1) { /* EBREAK */
	  /* Blah blah blah. */
	  *pc -= 4; /* EBREAK */
	}
      } break;
      case 0x302: {
	/* Probably a mutex here too */
	i32 mstatus, mepc;
	mstatus = l(0x441400); /* MRET */
	if(mstatus & 0x80) {   /* MRET */
	  mstatus |= 0x8; /* set mie to mpie */
	}                      /* MRET */ 
	mstatus |= 0x80; /* set mpie to 1 */
	s(mstatus, 0x441400); /* MRET */
	mepc = l(0x441504);
	*pc = mepc - 4;
#ifdef DEBUG	
	printf("%x", *pc); fflush(stdout);
#endif	
      } break;
      case 0x105: {
	; /* WFI - is a noop */
      } break;
      default:
	__builtin_unreachable();
      }
    } break;
    default:
      __builtin_unreachable();
    } break;
  }
  default:
    __builtin_unreachable();
  }
  goto end;
 end:
  rf[0] = 0; /* reset x0 to 0 */
  u32 cycles, cyclesh;
  cycles = l(0x443400); /* mcycles */
  cyclesh = l(0x446200); /* mcyclesh */
  cycles++;
  if (cycles == 0) {
    cyclesh++;
  }
  u32 timecmp, timecmph;
  timecmp = l(0x443708);
  timecmph = l(0x44370c);    
  if (cycles >= timecmp && cyclesh >= timecmph) {
#ifdef DEBUG      
    printf("Alright alright alright\n");
#endif
    queue_interrupt(0x80000007, 0, 0x80, ram);
  }
	
  s(cycles, 0x443400); /* mcycles */
  s(cyclesh, 0x446200); /* mcyclesh */
  s(cycles, 0x443800); /* cycles */
  s(cyclesh, 0x443a00); /* cyclesh */
  s(cycles, 0x443700); /* mtime */
  s(cycles, 0x443704); /* mtimeh */
  s(cycles, 0x443804); /* time */    
  s(cyclesh, 0x443a04); /* timeh */
  s(cycles, 0x443408); /* minstret */
  s(cyclesh, 0x446208); /* minstreth */    
  s(cycles, 0x443808); /* instret */    
  s(cyclesh, 0x443a08); /* instreth */
#ifdef DEBUG
#ifndef NOSLEEP  
  sleep(1);
#endif  
#endif      
}
