static inline def_EHelper(lui) {
  rtl_li(s, ddest, id_src1->imm);
  print_asm_template2(lui);
}

static inline def_EHelper(auipc) {
  rtl_addi(s, ddest, &cpu.pc, id_src1->imm);
  print_asm_template2(auipc);
}

static inline def_EHelper(addi) {
  rtl_addi(s, ddest, dsrc1, id_src2->imm);
  print_asm_template3(addi);
}

static inline def_EHelper(slli) {

}

static inline def_EHelper(slti) {

}

static inline def_EHelper(sltiu) {

}

static inline def_EHelper(xori) {

}

static inline def_EHelper(srli) {

}

static inline def_EHelper(srai) {

}

static inline def_EHelper(ori) {

}

static inline def_EHelper(andi) {

}

static inline def_EHelper(add) {

}

static inline def_EHelper(sub) {

}

static inline def_EHelper(sll) {

}

static inline def_EHelper(slt) {

}

static inline def_EHelper(sltu) {

}

static inline def_EHelper(xor) {

}

static inline def_EHelper(srl) {

}

static inline def_EHelper(sra) {

}

static inline def_EHelper(or) {

}

static inline def_EHelper(and) {

}
