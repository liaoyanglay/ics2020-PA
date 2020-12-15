#define INDEX_A7 17

static inline rtlreg_t *csr_l(uint32_t csr) {
  csr = csr & 0xfff;
  switch (csr) {
    case 0x100: return &cpu.sstatus;
    case 0x105: return &cpu.stvec;
    case 0x141: return &cpu.sepc;
    case 0x142: return &cpu.scause;
    default: panic("Unsuported CSR\n");
  }
}

static inline def_EHelper(ecall) {
  assert(s->isa.instr.i.simm11_0 == 0);
  void raise_intr(DecodeExecState *, uint32_t, vaddr_t);
  raise_intr(s, reg_l(INDEX_A7), cpu.pc);
}

static inline def_EHelper(sret) {
  rtl_jr(s, &cpu.sepc);
}

static inline def_EHelper(csrrw) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  rtl_mv(s, s0, dsrc1);
  if (ddest != &reg_l(0)) {
    rtl_mv(s, ddest, csr);
  }
  rtl_mv(s, csr, s0);
}

static inline def_EHelper(csrrs) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  rtl_mv(s, s0, dsrc1);
  rtl_mv(s, ddest, csr);
  if (dsrc1 != &reg_l(0)) {
    int32_t mask = 1 << 31;
    rtl_mv(s, s1, csr);
    if (*s0 > 0) {
      *s1 = *s1 | (mask >> (*s0 - 1));
    }
    rtl_mv(s, csr, s1);
  }
}

static inline def_EHelper(csrrc) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  rtl_mv(s, s0, dsrc1);
  rtl_mv(s, ddest, csr);
  if (dsrc1 != &reg_l(0)) {
    uint32_t mask = -1;
    rtl_mv(s, s1, csr);
    *s1 = *s1 & (mask >> *s0);
    rtl_mv(s, csr, s1);
  }
}

static inline def_EHelper(csrrwi) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  if (ddest != &reg_l(0)) {
    rtl_mv(s, ddest, csr);
  }
  rtl_li(s, csr, id_src1->imm);
}

static inline def_EHelper(csrrsi) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  rtl_mv(s, ddest, csr);
  if (id_src1->imm != 0) {
    int32_t mask = 1 << 31;
    rtl_mv(s, s1, csr);
    *s1 = *s1 | (mask >> (id_src1->imm - 1));
    rtl_mv(s, csr, s1);
  }
}

static inline def_EHelper(csrrci) {
  rtlreg_t *csr = csr_l(id_src2->imm);
  rtl_mv(s, ddest, csr);
  if (id_src1->imm != 0) {
    uint32_t mask = -1;
    rtl_mv(s, s1, csr);
    *s1 = *s1 & (mask >> id_src1->imm);
    rtl_mv(s, csr, s1);
  }
}
