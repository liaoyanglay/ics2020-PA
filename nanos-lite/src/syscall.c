#include <common.h>
#include <fs.h>
#include <proc.h>
#include <sys/time.h>
#include "syscall.h"

int sys_gettimeofday(struct timeval *tv, struct timezone *tz) {
  if (tv != NULL) {
    uint64_t us = io_read(AM_TIMER_UPTIME).us;
    tv->tv_sec = us / 1000000;
    tv->tv_usec = us % 1000000;
  }
  if (tz != NULL) {
    tz->tz_minuteswest = 0;
    tz->tz_dsttime = 0;
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
    case SYS_exit: sys_execve("/bin/menu", NULL, NULL); break;
    case SYS_yield: yield(); break;
    case SYS_brk: c->GPRx = 0; break;
    case SYS_write: c->GPRx = fs_write(a[1], (void *) a[2], a[3]); break;
    case SYS_read: c->GPRx = fs_read(a[1], (void *) a[2], a[3]); break;
    case SYS_open: c->GPRx = fs_open((char *) a[1], a[2], a[3]); break;
    case SYS_close: c->GPRx = fs_close(a[1]); break;
    case SYS_lseek: c->GPRx = fs_lseek(a[1], a[2], a[3]); break;
    case SYS_gettimeofday: c->GPRx = sys_gettimeofday((struct timeval *)a[1], (struct timezone *)a[2]); break;
    case SYS_execve: c->GPRx = sys_execve((const char *) a[1], (char **) a[2], (char **) a[3]); break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
