#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>

enum {
  TK_NOTYPE = 256, TK_EQ,
  TK_PLUS,      // +
  TK_MINUS,     // -
  TK_MULT,      // *
  TK_DIV,       // /
  TK_LPAREN,    // (
  TK_RPAREN,    // )
  TK_DEC_NUM,   // 十进制
  TK_HEX_NUM,   // 十六进制
  TK_REG,       // $sp, $a0
  TK_DEREF,     // *
  TK_NEQ,       // !=
  TK_AND        // &&

};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */
  {" +", TK_NOTYPE},    // spaces
  {"\\+", '+'},         // plus
  {"-", TK_MINUS},      // minus
  {"\\*", '*'},         // multiply/dereference
  {"/", '/'},           // divide
  {"==", TK_EQ},        // equal
  {"!=", TK_NEQ},       // not equal
  {"&&", TK_AND},       // logical and
  {"\\(", '('},         // left paren
  {"\\)", ')'},         // right paren
  {"0x[0-9a-fA-F]+", TK_HEX_NUM},  // hexadecimal number
  {"[0-9]+", TK_DEC_NUM},          // decimal number
  {"\\$[a-zA-Z0-9]+", TK_REG},     // register $sp, $a0等
};

#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX] = {};

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
  int i;
  char error_msg[128];
  int ret;

  for (i = 0; i < NR_REGEX; i ++) {
    ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
    if (ret != 0) {
      regerror(ret, &re[i], error_msg, 128);
      panic("regex compilation failed: %s\n%s", error_msg, rules[i].regex);
    }
  }
}

typedef struct token {
  int type;
  char str[32];
} Token;

static Token tokens[32] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
            i, rules[i].regex, position, substr_len, substr_len, substr_start);
        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            // 空格，什么都不做，只是跳过
            break;
          case '+': case '-': case '*': case '/':
          case '(': case ')': case '!':
            // 单字符运算符和括号
            tokens[nr_token].type = rules[i].token_type;
            nr_token++;
            break;
          case TK_DEC_NUM:
          case TK_HEX_NUM:
          case TK_EQ: case TK_NEQ:
          case TK_AND: case TK_MINUS: case TK_REG:
            // 需要保存字符串的token
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
            tokens[nr_token].str[substr_len] = '\0';  // 确保字符串结束
            nr_token++;
            break;
          default:
            panic("Unknown token type: %d\n", rules[i].token_type);
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  return true;
}

uint32_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */
  TODO();

  return 0;
}
