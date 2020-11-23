#include <isa.h>
#include "expr.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_DEC_INT, TK_HEX_INT, TK_REG
};

static struct rule {
  char *regex;
  int token_type;
} rules[] = {

  /* TODO: Add more rules.
   * Pay attention to the precedence level of different rules.
   */

  {" +", TK_NOTYPE},      // spaces
  {"\\(", '('},
  {"\\)", ')'},
  {"\\*", '*'},           // multiply
  {"/", '/'},             // divide
  {"\\+", '+'},           // plus
  {"\\-", '-'},           // minus
  {"==", TK_EQ},          // equal
  {"0x[a-fA-F0-9]+", TK_HEX_INT},  // hex
  {"[0-9]+", TK_DEC_INT}, // decimal
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

#define TK_SIZE 64

static Token tokens[TK_SIZE] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  memset(tokens, '\0', sizeof(tokens));

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        char *substr_start = e + position;
        int substr_len = pmatch.rm_eo;

        // Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s",
        //     i, rules[i].regex, position, substr_len, substr_len, substr_start);

        position += substr_len;

        /* TODO: Now a new token is recognized with rules[i]. Add codes
         * to record the token in the array `tokens'. For certain types
         * of tokens, some extra actions should be performed.
         */

        switch (rules[i].token_type) {
          case TK_NOTYPE:
            break;
          default:
            if (nr_token >= TK_SIZE) {
              printf("expression too long\n");
              return false;
            }
            tokens[nr_token].type = rules[i].token_type;
            size_t length = substr_len < sizeof(tokens[0].str) - 1 ? substr_len : sizeof(tokens[0].str) - 1;
            strncpy(tokens[nr_token].str, substr_start, length);
            nr_token++;
            break;
        }

        break;
      }
    }

    if (i == NR_REGEX) {
      printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
      return false;
    }
  }

  if (nr_token == 0) {
    printf("no expression\n");
    return false;
  }
  return true;
}

bool check_parentheses(int p, int q, bool *is_match) {
  *is_match = tokens[p].type == '(' && tokens[q].type == ')';
  int left = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      left++;
    } else if (tokens[i].type == ')') {
      left--;
      if (left < 0) {
        printf("Bad parentheses\n");
        return false;
      }
      if (i < q && left == 0) {
        *is_match = false;
      }
    }
  }

  if (left > 0) {
    printf("Bad parentheses\n");
    return false;
  }
  return true;
}

int find_main_op(int p, int q) {
  int op = -1;
  int left = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      left++;
    } else if (tokens[i].type == ')') {
      left--;
    }

    if (left > 0) continue;

    switch (tokens[i].type) {
      case '+':
      case '-':
        op = i;
        break;
      case '*':
      case '/':
        if (op < 0 || (tokens[op].type != '+' && tokens[op].type != '-')) {
          op = i;
        }
        break;
      default:
        break;
    }
  }
  return op;
}

bool eval(int p, int q, word_t *res) {
  if (p > q) {
    printf("Bad expression\n");
    return false;
  }
  if (p == q) {
    /* Single token.
     * For now this token should be a number.
     * Return the value of the number.
     */
    switch (tokens[p].type) {
      case TK_DEC_INT:
        *res = strtol(tokens[p].str, NULL, 10);
        break;
      case TK_HEX_INT:
        *res = strtol(tokens[p].str, NULL, 16);
        break;
      default:
        printf("Bad expression\n");
        return false;
    }
    return true;
  }

  bool is_match = false;
  if (!check_parentheses(p, q, &is_match)) return false;
  if (is_match) {
    /* The expression is surrounded by a matched pair of parentheses.
     * If that is the case, just throw away the parentheses.
     */
    return eval(p + 1, q - 1, res);
  }

  // op is the position of 主运算符 in the token expression
  int op = find_main_op(p, q);
  if (op < p || op >= q) {
    printf("Bad expression\n");
    return false;
  }

  word_t val1 = 0;
  if (!eval(p, op - 1, &val1)) return false;
  word_t val2 = 0;
  if (!eval(op + 1, q, &val2)) return false;

  switch (tokens[op].type) {
    case '+':
      *res = val1 + val2;
      break;
    case '-':
      *res = val1 - val2;
      break;
    case '*':
      *res = val1 * val2;
      break;
    case '/':
      if (val2 == 0) {
        printf("Div by zero\n");
        return false;
      }
      *res = val1 / val2;
      break;
    default:
      printf("Bad expression\n");
      return false;
  }

  return true;
}

word_t expr(char *e, bool *success) {
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  word_t res = 0;
  if (!eval(0, nr_token - 1, &res)) {
    *success = false;
    return 0;
  }
  *success = true;
  return res;
}
