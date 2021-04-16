#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  uint32_t new_val;
  uint32_t old_val;
  char expr[32];

} WP;
void init_wp_pool();
WP* new_wp();
void free_wp(WP *wp);
WP* set_watchpoint(char *e);
bool delete_watchpoint(int NO);
void list_watchpoint(void);
WP* scan_watchpoint(void);
#endif
