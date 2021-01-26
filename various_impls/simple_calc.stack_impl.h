#ifndef _SIMPLE_CALC__H_
#define _SIMPLE_CALC__H_

#include "calc_stack.h"

typedef long long calc_t;

enum operators {
    NUL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW
};

int  is_operator(char test);
char operator_to_str(enum operators op);
enum operators get_operator(char op);
int  precedence(char op);
token_stack  parse_infix(char * input);
char * make_expression(token_stack input);
void display_expression(char *op1, enum operators oper, char *op2);
calc_t calculate(char *lnum, enum operators oper, char *rnum);
calc_t simple_evaluate(const char * expression);
calc_t postfix_evaluate(token_stack * input);
void assert_calc(const char * expr, calc_t expected);
void run_tests(void);

    
#endif  // _SIMPLE_CALC__H_
