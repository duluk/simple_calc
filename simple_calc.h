#ifndef _SIMPLE_CALC__H_
#define _SIMPLE_CALC__H_

#include "calc_stack.h"

#define MAX_VAR 26

typedef double calc_t;

typedef struct Variable
{
    // Currently allowing only one letter variables
    char   name;
    calc_t value;
} Variable;

typedef enum Commands
{
    NONE,
    LET,
    SHOW
} Commands;

typedef enum return_code
{
    SUCCESS,
    FAILED,
    VALID_COMMAND,
    INVALID_COMMAND
} return_code;

enum operators
{
    NUL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD,
    POW
};

int  is_operator(char test);
int  left_associative(char op);
char operator_to_str(enum operators op);
enum operators get_operator(char op);
int  precedence(char op);
int  parse_infix(const char * input, token_stack ** postfix_stack, calc_t * var_list);
char * stack_to_str(token_stack input);
void display_expression(char *op1, enum operators oper, char *op2);
calc_t calculate(char *lnum, enum operators oper, char *rnum);
calc_t simple_evaluate(const char * expression);
calc_t postfix_evaluate(token_stack * input);
void assert_calc(const char * expr, calc_t expected, calc_t * vars);
void run_tests(void);

Commands command_found(char * cmd);
int validate_variable(const char * v);
int validate_value(const char * v);
int hash_var(char v);
int lookup_var(char x, calc_t * vars, calc_t * n);
Variable * parse_let(char * input);
return_code parse_command(char * input_str, Variable ** var_list, Commands cmd);
const char * enum_cmd_str(Commands * c);
    
#endif  // _SIMPLE_CALC__H_
