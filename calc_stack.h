#ifndef _CALC_STACK__H_
#define _CALC_STACK__H_

#define MAX_STACK_SIZE 512

typedef char stack_type;

enum STACK_ERRORS
{
    STACK_SUCCESS,
    STACK_UNDERFLOW,
    STACK_OVERFLOW,
    STACK_FAIL
};

typedef struct token_stack
{
    stack_type * elements[MAX_STACK_SIZE];
    size_t max_size_life;
    size_t stack_count;
} token_stack;

int stack_initialize(token_stack *st);
int stack_pop(token_stack *st);
int stack_push(token_stack *st, stack_type * elem);
int stack_top(token_stack *st, stack_type ** elem);
int stack_peek(token_stack *st, stack_type ** elem);
int stack_empty(token_stack *st);
int stack_full(token_stack *st);
int stack_clear(token_stack *st);
token_stack * stack_reverse(token_stack * st);
size_t stack_size(token_stack *st);

#endif  // _CALC_STACK__H_


/*
  initialize
     - create stack object and set to 0

  pop
     - If stack not empty, top element is removed; otherwise error
  returned (underflow)

  push
     - If stack is not full, element pushed onto top of stack;
  otherwise, error is returned (overflow)

  top
     - If stack is not empty, return (or value-result) top element of
  the stack; otherwise, error is returned (fail)

  empty
     - True is returned if zero elements in stack

  full
     - True is returned if MAX_STACK_SIZE elements in stack

  size
     - stack_count is returned
*/

  
