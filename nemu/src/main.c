#include <stdio.h>
#include <stdlib.h>
#include "monitor/debug/expr.h"

void init_monitor(int, char *[]);
void engine_start();
int is_exit_status_bad();

int main(int argc, char *argv[]) {
  /* Initialize the monitor. */
  init_monitor(argc, argv);

  /* Expr test */
  // word_t result = 0;
  // char s[65536] = {};
  // FILE *fp = fopen("./input", "r");
  // assert(fp != NULL);
  // int i = 0;
  // bool success = false;
  // bool ac = true;
  // while (fscanf(fp, "%u %s", &result, s) != EOF) {
  //   word_t ans = expr(s, &success);
  //   i++;
  //   if (result != expr(s, &success)) {
  //     ac = false;
  //     printf("Case %u failed, wa: %u, expr: %s\n", i, ans, s);
  //   }
  // }
  // fclose(fp);
  // if (ac) printf("All test cases access!\n");
  // return 0;

  /* Start engine. */
  engine_start();

  return is_exit_status_bad();
}
