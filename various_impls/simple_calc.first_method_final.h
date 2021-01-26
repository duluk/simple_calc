#ifndef _SIMPLE_CALC__H_
#define _SIMPLE_CALC__H_

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
long calculate(char *lnum, enum operators oper, char *rnum);
long simple_evaluate(const char * expression);
void assert_calc_func(const char * expr, long expected, long (*func)(const char *));
void run_tests(void);

    
#endif  // _SIMPLE_CALC__H_
