#include <common.h>
#include "syscall.h"

int sys_write(int fd, void *buf, size_t count) {
  if (fd == 1 || fd == 2) {
    for (char *ch = (char *) buf; ch < (char *) buf + count; ch++) {
      putch(*ch);
    }
    return count;
  }
  return 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  a[1] = c->GPR2;
  a[2] = c->GPR3;
  a[3] = c->GPR4;

  switch (a[0]) {
    case SYS_exit: halt(a[1]); panic("Should not reach here");
    case SYS_yield: yield(); break;
    case SYS_brk: c->GPRx = 0; break;
    case SYS_write: c->GPRx = sys_write(a[1], (void *) a[2], a[3]); break;
    case SYS_read:
    case SYS_open:
    case SYS_close:
    case SYS_lseek:
    case SYS_execve:
    case SYS_gettimeofday:
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
