#include <isa.h>
#include "local-include/reg.h"

const char *regsl[] = {
  "$0", "ra", "sp", "gp", "tp", "t0", "t1", "t2",
  "s0", "s1", "a0", "a1", "a2", "a3", "a4", "a5",
  "a6", "a7", "s2", "s3", "s4", "s5", "s6", "s7",
  "s8", "s9", "s10", "s11", "t3", "t4", "t5", "t6"
};

#define NR_REG 32

void isa_reg_display() {
  for (int i = 0; i < NR_REG; i++) {
    printf("%-6s  0x%08x   %u\n", regsl[i], reg_l(i), reg_l(i));
  }
}

word_t isa_reg_str2val(const char *s, bool *success) {
  if (strcmp("0", s) == 0) {
    *success = true;
    return reg_l(0);
  }
  if (strcmp("pc", s) == 0) {
    *success = true;
    return cpu.pc;
  }
  for (int i = 1; i < NR_REG; i++) {
    if (strcmp(regsl[i], s) == 0) {
      *success = true;
      return reg_l(i);
    }
  }
  *success = false;
  return 0;
}
