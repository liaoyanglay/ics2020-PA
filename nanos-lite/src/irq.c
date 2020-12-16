#include <common.h>

void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
    case EVENT_YIELD: printf("yield!\n"); break;
    case EVENT_SYSCALL: do_syscall(c); break;
    case EVENT_PAGEFAULT:
    case EVENT_IRQ_TIMER:
    case EVENT_IRQ_IODEV:
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
