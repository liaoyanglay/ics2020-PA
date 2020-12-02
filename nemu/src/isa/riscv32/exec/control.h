static inline def_EHelper(jal) {
  rtl_addi(s, ddest, &cpu.pc, 4);
  rtl_addi(s, s0, &cpu.pc, id_src1->simm);
  rtl_jr(s, s0);
  print_asm_template2(jal);
}

static inline def_EHelper(jalr) {
  rtl_addi(s, ddest, &cpu.pc, 4);
  rtl_addi(s, s0, dsrc1, id_src2->simm);
  rtl_andi(s, s0, s0, -2);  // set lsb to zero
  rtl_jr(s, s0);
  print_asm_template3(jalr);
}

static inline def_EHelper(beq) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_EQ, dsrc1, dsrc2, *s0);
  print_asm_template3(beq);
}

static inline def_EHelper(bne) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_NE, dsrc1, dsrc2, *s0);
  print_asm_template3(bne);
}

static inline def_EHelper(blt) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_LT, dsrc1, dsrc2, *s0);
  print_asm_template3(blt);
}

static inline def_EHelper(bge) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_GE, dsrc1, dsrc2, *s0);
  print_asm_template3(bge);
}

static inline def_EHelper(bltu) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_LTU, dsrc1, dsrc2, *s0);
  print_asm_template3(bltu);
}

static inline def_EHelper(bgeu) {
  rtl_addi(s, s0, &cpu.pc, id_dest->simm);
  rtl_jrelop(s, RELOP_GEU, dsrc1, dsrc2, *s0);
  print_asm_template3(bgeu);
}