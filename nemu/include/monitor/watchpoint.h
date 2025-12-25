#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"
typedef struct watchpoint {
  int NO;
  struct watchpoint *next;

  /* TODO: Add more members if necessary */
  char expr[128];
  uint32_t old_val;    //旧值
  uint32_t new_val;    //新值

} WP;
WP* new_wp(char *expr_str);
void free_wp(int NO);
bool check_watchpoints(void);
WP* get_head(void);
#endif
