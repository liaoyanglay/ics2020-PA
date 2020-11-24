#include <isa.h>
#include <memory/vaddr.h>
#include "expr.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <regex.h>
#include <stdlib.h>

enum {
  TK_NOTYPE = 256, TK_EQ,

  /* TODO: Add more token types */
  TK_DEC_INT, TK_HEX_INT, TK_REG, TK_DEREF, TK_NEGATIVE,
  TK_NE, TK_AND,
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
  {"\\*", '*'},           // multiply or deref
  {"/", '/'},             // divide
  {"\\+", '+'},           // plus
  {"\\-", '-'},           // minus or neg
  {"==", TK_EQ},          // equal
  {"!=", TK_NE},          // not equal
  {"&&", TK_AND},         // and
  {"\\$[a-zA-Z0-9]+", TK_REG},    // reg
  {"0[xX][a-fA-F0-9]+", TK_HEX_INT}, // hex
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

#define TK_SIZE 256

static Token tokens[TK_SIZE] __attribute__((used)) = {};
static int nr_token __attribute__((used))  = 0;

static bool make_token(const char *e) {
  int position = 0;
  int i;
  regmatch_t pmatch;

  nr_token = 0;
  memset(tokens, '\0', sizeof(tokens));

  while (e[position] != '\0') {
    /* Try all rules one by one. */
    for (i = 0; i < NR_REGEX; i ++) {
      if (regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
        const char *substr_start = e + position;
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
            if (substr_len >= sizeof(tokens[0].str)) {
              printf("token too long: %s\n", substr_start);
              return false;
            }
            tokens[nr_token].type = rules[i].token_type;
            strncpy(tokens[nr_token].str, substr_start, substr_len);
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
  int op_deref = -1;
  int op_mul = -1;
  int op_plus = -1;
  int op_eq = -1;
  int op_and = -1;
  int left = 0;
  for (int i = p; i <= q; i++) {
    if (tokens[i].type == '(') {
      left++;
    } else if (tokens[i].type == ')') {
      left--;
    }

    if (left > 0) continue;

    switch (tokens[i].type) {
      case TK_DEREF:
      case TK_NEGATIVE:
        if (op_deref < 0) op_deref = i;
        break;
      case '+':
      case '-':
        op_plus = i;
        break;
      case '*':
      case '/':
        op_mul = i;
        break;
      case TK_EQ:
      case TK_NE:
        op_eq = i;
        break;
      case TK_AND:
        op_and = i;
        break;
      default:
        break;
    }
  }

  // Sort by precedence level from low to high.
  if (op_and >= 0) return op_and;
  if (op_eq >= 0) return op_eq;
  if (op_plus >= 0) return op_plus;
  if (op_mul >= 0) return op_mul;
  if (op_deref >= 0) return op_deref;

  return -1;
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
    bool success = false;
    switch (tokens[p].type) {
      case TK_DEC_INT:
        *res = strtol(tokens[p].str, NULL, 10);
        break;
      case TK_HEX_INT:
        *res = strtol(tokens[p].str, NULL, 16);
        break;
      case TK_REG:
        *res = isa_reg_str2val(tokens[p].str + 1, &success);
        if (!success) {
          printf("Unknown reg name: %s\n", tokens[p].str);
          return false;
        }
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
  if (tokens[op].type != TK_DEREF && tokens[op].type != TK_NEGATIVE) {
    if (!eval(p, op - 1, &val1)) return false;
  }
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
    case TK_DEREF:
      if (val2 < 0x80000000) {
        printf("Deref illegal address: 0x%08x\n", val2);
        return false;
      }
      *res = vaddr_read(val2, sizeof(word_t));
      break;
    case TK_NEGATIVE:
      *res = -val2;
      break;
    case TK_EQ:
      *res = val1 == val2;
      break;
    case TK_NE:
      *res = val1 != val2;
      break;
    case TK_AND:
      *res = val1 && val2;
      break;
    default:
      printf("Bad expressionr\n");
      return false;
  }

  return true;
}

word_t expr(const char *e, bool *success) {
  assert(e != NULL);
  if (!make_token(e)) {
    *success = false;
    return 0;
  }

  /* TODO: Insert codes to evaluate the expression. */

  for (int i = 0; i < nr_token; i ++) {
    if (tokens[i].type == '*' && (i == 0 || (tokens[i - 1].type != TK_DEC_INT &&
                                             tokens[i - 1].type != TK_HEX_INT &&
                                             tokens[i - 1].type != TK_REG &&
                                             tokens[i - 1].type != ')'))) {
      tokens[i].type = TK_DEREF;
    }
    if (tokens[i].type == '-' && (i == 0 || (tokens[i - 1].type != TK_DEC_INT &&
                                             tokens[i - 1].type != TK_HEX_INT &&
                                             tokens[i - 1].type != TK_REG &&
                                             tokens[i - 1].type != ')'))) {
      tokens[i].type = TK_NEGATIVE;
    }
  }

  word_t res = 0;
  if (!eval(0, nr_token - 1, &res)) {
    *success = false;
    return 0;
  }
  *success = true;
  return res;
}
