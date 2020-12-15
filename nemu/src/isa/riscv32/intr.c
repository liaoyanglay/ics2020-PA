#include <cpu/exec.h>
#include "local-include/rtl.h"

void raise_intr(DecodeExecState *s, uint32_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * That is, use ``NO'' to index the IDT.
   */

  cpu.sepc = epc;
  cpu.scause = NO;
  rtl_jr(s, &cpu.stvec);
}

void query_intr(DecodeExecState *s) {
}
