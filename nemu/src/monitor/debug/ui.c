#include <isa.h>
#include <memory/vaddr.h>
#include "expr.h"
#include "watchpoint.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void cpu_exec(uint64_t);
int is_batch_mode();

/* We use the `readline' library to provide more flexibility to read from stdin. */
static char* rl_gets() {
  static char *line_read = NULL;

  if (line_read) {
    free(line_read);
    line_read = NULL;
  }

  line_read = readline("(nemu) ");

  if (line_read && *line_read) {
    add_history(line_read);
  }

  return line_read;
}

static int cmd_help(char *args);

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args) {
  if (args == NULL) {
    cpu_exec(1);
    return 0;
  }

  uint64_t count = 0;
  if (sscanf(args, "%lu", &count) != 1) {
    printf("Usage: si [N]\n");
  }
  cpu_exec(count);
  return 0;
}

static int cmd_info(char *args) {
  if (args == NULL) {
    printf("Usage: info r or info w\n");
    return 0;
  }
  if (*args == 'r') {
    isa_reg_display();
  } else if (*args == 'w') {

  } else {
    printf("Usage: info r or info w\n");
  }
  return 0;
}

static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Usage: p expr\n");
    return 0;
  }
  bool success = false;
  word_t res = expr(args, &success);
  if (success) {
    printf("result: %u\n", res);
  }
  return 0;
}

static int cmd_x(char *args) {
  uint32_t num = 0;
  vaddr_t addr = 0;
  if (sscanf(args, "%u %x", &num, &addr) != 2) {
    printf("Usage: x N expr\n");
  }

  for (uint32_t i = 0; i < num; i++) {
    if (i % 4 == 0) {
      printf("\n(");
      printf(FMT_WORD, addr + i * (uint32_t) sizeof(word_t));
      printf(")");
    }
    printf("   ");
    printf(FMT_WORD, vaddr_read(addr + i * sizeof(word_t), sizeof(word_t)));
  }
  printf("\n");
  return 0;
}

static int cmd_w(char *args) {
  return 0;
}

static int cmd_d(char *args) {
  return 0;
}

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "c", "Continue the execution of the program", cmd_c },
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */
  { "si", "Single step execution", cmd_si },
  { "info", "Display informations about register status or watch point", cmd_info },
  { "p", "Expression evaluation", cmd_p },
  { "x", "Scan memory", cmd_x },
  { "w", "Set the watch point", cmd_w },
  { "d", "Remove the watch point", cmd_d },
};

#define NR_CMD (sizeof(cmd_table) / sizeof(cmd_table[0]))

static int cmd_help(char *args) {
  /* extract the first argument */
  char *arg = strtok(NULL, " ");
  int i;

  if (arg == NULL) {
    /* no argument given */
    for (i = 0; i < NR_CMD; i ++) {
      printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
    }
  }
  else {
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(arg, cmd_table[i].name) == 0) {
        printf("%s - %s\n", cmd_table[i].name, cmd_table[i].description);
        return 0;
      }
    }
    printf("Unknown command '%s'\n", arg);
  }
  return 0;
}

void ui_mainloop() {
  if (is_batch_mode()) {
    cmd_c(NULL);
    return;
  }

  for (char *str; (str = rl_gets()) != NULL; ) {
    char *str_end = str + strlen(str);

    /* extract the first token as the command */
    char *cmd = strtok(str, " ");
    if (cmd == NULL) { continue; }

    /* treat the remaining string as the arguments,
     * which may need further parsing
     */
    char *args = cmd + strlen(cmd) + 1;
    if (args >= str_end) {
      args = NULL;
    }

#ifdef HAS_IOE
    extern void sdl_clear_event_queue();
    sdl_clear_event_queue();
#endif

    int i;
    for (i = 0; i < NR_CMD; i ++) {
      if (strcmp(cmd, cmd_table[i].name) == 0) {
        if (cmd_table[i].handler(args) < 0) { return; }
        break;
      }
    }

    if (i == NR_CMD) { printf("Unknown command '%s'\n", cmd); }
  }
}
