#include "monitor/monitor.h"
#include "monitor/expr.h"
#include "monitor/watchpoint.h"
#include "nemu.h"

#include <stdlib.h>
#include <readline/readline.h>
#include <readline/history.h>

void isa_reg_display(void);

void cpu_exec(uint64_t);

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

static int cmd_c(char *args) {
  cpu_exec(-1);
  return 0;
}

static int cmd_q(char *args) {
  return -1;
}

static int cmd_si(char *args){
  uint64_t n = 1;

  if (args != NULL) {
    n = atoi(args);  //字符串转整数
  }
  cpu_exec(n);
  return 0;
}

static int cmd_info(char *args){
  char *arg = strtok(NULL, " ");

  if (arg == NULL) {
    printf("Usage: info r | info w\n");
    return 0;
  }
  
  if (strcmp(arg, "r") == 0) {
    isa_reg_display();
  }
  else if (strcmp(arg, "w") == 0) {
  extern WP* get_head();
  WP *head = get_head();
  
  if (head == NULL) {
    printf("No watchpoints\n");
  } else {
    printf("%-5s %-20s %-15s\n", "NO", "Expr", "Value");
    printf("---- -------------------- ---------------\n");
    WP *p;
    for (p = head; p != NULL; p = p->next) {
      printf("%-5d %-20s 0x%08x\n", p->NO, p->expr, p->old_val);
    }
  }
}
  return 0;
}
static int cmd_p(char *args) {
  if (args == NULL) {
    printf("Usage: p EXPR\n");
    return 0;
  }
  
  bool success = true;
  uint32_t result = expr(args, &success);
  
  if (success) {
    printf("%u (0x%08x)\n", result, result);
  } else {
    printf("Expression evaluation failed\n");
  }
  
  return 0;
}

static int cmd_d(char *args) {
  if (args == NULL) {
    printf("Usage: d N\n");
    return 0;
  }
  
  int NO = atoi(args);
  free_wp(NO);
  
  return 0;
}

static int cmd_w(char *args){
  if (args == NULL) {
    printf("Usage: w EXPR\n");
    return 0;
  }
  
  WP *wp = new_wp(args);
  if (wp != NULL) {
    printf("Set watchpoint %d: %s\n", wp->NO, wp->expr);
    printf("Old value = 0x%08x\n", wp->old_val);
  }
  
  return 0;
}

static int cmd_x(char *args) {
  if (args == NULL) {
    printf("Usage: x N EXPR\n");
    return 0;
  }
  
  char *n_str = strtok(args, " ");
  if (n_str == NULL) {
    printf("Missing N\n");
    return 0;
  }
  int n = atoi(n_str);
  
  char *expr_str = strtok(NULL, " ");
  if (expr_str == NULL) {
    printf("Missing address expression\n");
    return 0;
  }
  
  uint32_t addr;
  sscanf(expr_str, "%x", &addr);  // %x 表示读取十六进制
  
  int i;
  for (i = 0; i < n; i++) {
    uint32_t data = vaddr_read(addr + i * 4, 4);
    printf("0x%08x: 0x%08x\n", addr + i * 4, data);
  }
  
  return 0;
}

static int cmd_help(char *args);

static struct {
  char *name;
  char *description;
  int (*handler) (char *);
} cmd_table [] = {
  { "help", "Display informations about all supported commands", cmd_help },
  { "info", "Print information (r: registers, w: watchpoints)",cmd_info },
  { "c", "Continue the execution of the program", cmd_c },
  { "w", "Set watchpoint: w EXPR", cmd_w },
{ "d", "Delete watchpoint: d N", cmd_d },
  { "p", "Evaluate expression", cmd_p},
  { "si", "Single step execution", cmd_si},
  { "x", "Exmaine memory", cmd_x},
  { "q", "Exit NEMU", cmd_q },

  /* TODO: Add more commands */

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

void ui_mainloop(int is_batch_mode) {
  if (is_batch_mode) {
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
    extern void sdl_clear_event_queue(void);
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
