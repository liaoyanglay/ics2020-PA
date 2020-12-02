#include <cpu/exec.h>
#include "../local-include/decode.h"
#include "all-instr.h"

static inline void set_width(DecodeExecState *s, int width) {
  if (width != 0) s->width = width;
}

static inline def_EHelper(load) {
  switch (s->isa.instr.i.funct3) {
    EXW  (0, lds, 1)
    EXW  (1, lds, 2)
    EXW  (2, ld, 4)
    EXW  (4, ld, 1)
    EXW  (5, ld, 2)
    default: exec_inv(s);
  }
}

static inline def_EHelper(store) {
  switch (s->isa.instr.s.funct3) {
    EXW  (0, st, 1)
    EXW  (1, st, 2)
    EXW  (2, st, 4)
    default: exec_inv(s);
  }
}

static inline def_EHelper(sri) {
  switch (s->isa.instr.r.funct7) {
    EX (0b0000000, srli)
    EX (0b0100000, srai)
    default: exec_inv(s);
  }
}

static inline def_EHelper(op_imm) {
  switch (s->isa.instr.i.funct3) {
    EX (0, addi)
    EX (1, slli)
    EX (2, slti)
    EX (3, sltiu)
    EX (4, xori)
    EX (5, sri)
    EX (6, ori)
    EX (7, andi)
    default: exec_inv(s);
  }
}

static inline def_EHelper(op) {
  switch ((s->isa.instr.r.funct7 << 3) | s->isa.instr.r.funct3) {
    EX ((0b0000000 << 3) | 0, add)
    EX ((0b0100000 << 3) | 0, sub)
    EX ((0b0000000 << 3) | 1, sll)
    EX ((0b0000000 << 3) | 2, slt)
    EX ((0b0000000 << 3) | 3, sltu)
    EX ((0b0000000 << 3) | 4, xor)
    EX ((0b0000000 << 3) | 5, srl)
    EX ((0b0100000 << 3) | 5, sra)
    EX ((0b0000000 << 3) | 6, or)
    EX ((0b0000000 << 3) | 7, and)
    EX ((0b0000001 << 3) | 0, mul)
    EX ((0b0000001 << 3) | 1, mulh)
    EX ((0b0000001 << 3) | 2, mulhsu)
    EX ((0b0000001 << 3) | 3, mulhu)
    EX ((0b0000001 << 3) | 4, div)
    EX ((0b0000001 << 3) | 5, divu)
    EX ((0b0000001 << 3) | 6, rem)
    EX ((0b0000001 << 3) | 7, remu)
    default: exec_inv(s);
  }
}

static inline def_EHelper(branch) {
  switch (s->isa.instr.b.funct3) {
    EX (0, beq)
    EX (1, bne)
    EX (4, blt)
    EX (5, bge)
    EX (6, bltu)
    EX (7, bgeu)
    default: exec_inv(s);
  }
}

static inline def_EHelper(system) {
  switch (s->isa.instr.i.simm11_0) {
    // EX (0, ecall)
    // EX (1, ebreak)
    default: exec_inv(s);
  }
}

static inline void fetch_decode_exec(DecodeExecState *s) {
  s->isa.instr.val = instr_fetch(&s->seq_pc, 4);
  assert(s->isa.instr.i.opcode1_0 == 0x3);
  switch (s->isa.instr.i.opcode6_2) {
    IDEX (0b00000, I, load)
    IDEX (0b01000, S, store)
    IDEX (0b01101, U, lui)
    IDEX (0b00101, U, auipc)
    IDEX (0b00100, I, op_imm)
    IDEX (0b01100, R, op)
    IDEX (0b11000, B, branch)
    IDEX (0b11001, I, jalr)
    IDEX (0b11011, J, jal)
    IDEX (0b11100, I, system)
    EX   (0b11010, nemu_trap)
    default: exec_inv(s);
  }
}

static inline void reset_zero() {
  reg_l(0) = 0;
}

vaddr_t isa_exec_once() {
  DecodeExecState s;
  s.is_jmp = 0;
  s.seq_pc = cpu.pc;

  fetch_decode_exec(&s);
  update_pc(&s);

  reset_zero();

  return s.seq_pc;
}
