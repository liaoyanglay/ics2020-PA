#include "watchpoint.h"
#include "expr.h"

#define NR_WP 32

static WP wp_pool[NR_WP] = {};
static WP *head = NULL, *free_ = NULL;

void init_wp_pool() {
  int i;
  for (i = 0; i < NR_WP; i ++) {
    wp_pool[i].NO = i;
    wp_pool[i].next = &wp_pool[i + 1];
  }
  wp_pool[NR_WP - 1].next = NULL;

  head = NULL;
  free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */
WP* new_wp(const char *e) {
  WP *wp = free_;
  if (wp == NULL) {
    printf("No free watch point\n");
    return NULL;
  }
  if (strlen(e) >= sizeof(wp->expr)) {
    printf("watch point expr too long\n");
    return NULL;
  }
  bool success = false;
  wp->last = expr(e, &success);
  if (!success) {
    return NULL;
  }

  strcpy(wp->expr, e);
  free_ = free_->next;
  wp->next = head;
  head = wp;
  return wp;
}

void free_wp(int NO) {
  WP *wp = wp_pool + NO;
  if (head == NULL) {
    printf("No watchpoint\n");
    return;
  }
  if (wp == head) {
    head = head->next;
    wp->next = free_;
    free_ = wp;
  } else {
    for (WP *tmp = head; tmp->next != NULL; tmp = tmp->next) {
      if (tmp->next == wp) {
        tmp->next = wp->next;
        wp->next = free_;
        free_ = wp;
        return;
      }
    }
    printf("watchpoint %d not found\n", NO);
  }
}

bool check_wp_hit() {
  bool hit = false;
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    bool success = false;
    word_t value = expr(wp->expr, &success);
    assert(success);
    if (wp->last != value) {
      hit = true;
      printf("\nHit watchpoint %d: %s\n", wp->NO, wp->expr);
      printf("Old value = %u\n", wp->last);
      printf("New value = %u\n", value);
      wp->last = value;
    }
  }
  return hit;
}

void display_wp() {
  if (head == NULL) {
    printf("No watchpoint\n");
    return;
  }
  for (WP *wp = head; wp != NULL; wp = wp->next) {
    printf("watchpoint %d: %s\n", wp->NO, wp->expr);
  }
}
