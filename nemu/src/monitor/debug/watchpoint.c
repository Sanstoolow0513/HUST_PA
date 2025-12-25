#include "monitor/watchpoint.h"
#include "monitor/expr.h"

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
WP* get_head(){
  return head;
}

WP *new_wp(char *expr_str){
  if(free_ == NULL){
    printf("No free watchpoint pool\n");
    return NULL;
  }

  WP *wp = free_;
  free_ = free_->next;
  wp->next = NULL;
  strncpy(wp->expr,expr_str,127);

  bool success = true;
  if(!success){
    printf("Invalid expression: %s\n", wp->expr);
    wp->next = free_;
    free_ = wp;
    return NULL;
  }

  if(head == NULL){
    head = wp;
  }else {
    WP *p = head;
    while (p->next != NULL) {
      p = p->next;
    }
    p->next = wp;
  }

  return wp;
}


void free_wp(int NO){
  WP *p,*prev;

  prev = NULL;
  for (p = head; p != NULL; prev = p, p = p->next) {
    if (p->NO == NO) {
      break;
    }
  }
  if (p == NULL) {
    printf("Watchpoint %d not found\n", NO);
    return;
  }

  if (prev == NULL) {
    head = p->next;
  } else {
    prev->next = p->next;
  }

  p->next = free_;
  free_ = p;
  
  printf("Watchpoint %d deleted\n", NO);
}

bool check_watchpoints(){
  
  WP *p;
  bool changed = false;
  
  for (p = head; p != NULL; p = p->next) {
    bool success = true;
    uint32_t new_val = expr(p->expr, &success);
    
    if (!success) {
      printf("Error evaluating watchpoint %d: %s\n", p->NO, p->expr);
      continue;
    }
    
    if (new_val != p->old_val) {
      printf("\n");
      printf("Hardware watchpoint %d: %s\n", p->NO, p->expr);
      printf("Old value = 0x%08x\n", p->old_val);
      printf("New value = 0x%08x\n", new_val);
      printf("\n");
      
      p->old_val = new_val;  // 更新旧值
      changed = true;
    }
  }
  
  return changed;

}


