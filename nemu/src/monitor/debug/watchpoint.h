#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include <common.h>

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[1024];
  word_t last;
} WP;

WP* new_wp(const char *e);
void free_wp(int NO);
bool check_wp_hit();
void display_wp();

#endif
