static inline def_EHelper(jal) {
  s->is_jmp = true;
  rtl_addi(s, &s->jmp_pc, &cpu.pc, id_src1->imm);
  rtl_addi(s, ddest, &cpu.pc, 4);
  print_asm_template2(jal);
}

static inline def_EHelper(jalr) {
  s->is_jmp = true;
  rtl_addi(s, &s->jmp_pc, dsrc1, id_src2->imm);
  rtl_andi(s, &s->jmp_pc, &s->jmp_pc, -2);  // set lsb to zero
  rtl_addi(s, ddest, &cpu.pc, 4);
  print_asm_template3(jalr);
}

static inline def_EHelper(beq) {

}

static inline def_EHelper(bne) {

}

static inline def_EHelper(blt) {

}

static inline def_EHelper(bge) {

}

static inline def_EHelper(bltu) {

}

static inline def_EHelper(bgeu) {

}