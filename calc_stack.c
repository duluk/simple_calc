/*
 * Can this be implemented as both a stack and a queue? So that
 * instead of reversing the stack for processing the operands and
 * operators in the correct order, we just deque instead. What
 * would it look like? Need to have a pointer to the top of the stack
 * and the head of the queue, and both need to be maintained through
 * all operations.
 */

#include <stdio.h>
#include <stdlib.h>  // for malloc/free
#include <string.h>  // for memset

#include "calc_stack.h"

int
stack_initialize(token_stack *st)
{
    /* st = (token_stack) 0; */
    
    memset(st, 0, sizeof *st);
    st->stack_count = 0;
    st->max_size_life = 0;

    return st->stack_count;
}

int
stack_pop(token_stack *st)
{
    if (st->stack_count != 0)
    {
        // Not free-ing here. Let caller do it. They allocated it
        // anyway. This free-ing is way too limiting. :) Presumably if
        // they have retriveed it they want to use it. If they don't,
        // they can free it themsevles.
        /* free(st->elements[st->stack_count-1]); */
        --st->stack_count; // -> has precendence over prefix decrement
    }
    else
        return STACK_UNDERFLOW;

    return STACK_SUCCESS;
}

int
stack_push(token_stack *st, stack_type * elem)
{
    if (st->stack_count < MAX_STACK_SIZE-1)
    {
        st->elements[st->stack_count] = elem;
        ++st->stack_count;
        ++st->max_size_life;
    }
    else
        return STACK_OVERFLOW;

    return STACK_SUCCESS;
}

// See passing_pointer_test.c for understanding/reminder of why elem
// must be pointer to a pointer in order to actually pass back the
// item to the calling function.
int
stack_top(token_stack *st, stack_type ** elem)
{
    if (st->stack_count != 0)
    {
        *elem = st->elements[st->stack_count-1];
    }
    else
        return STACK_FAIL;

    return STACK_SUCCESS;
}

// "alias" for stack_top until I replace it
int
stack_peek(token_stack *st, stack_type ** elem)
{
    if (st->stack_count != 0)
    {
        *elem = st->elements[st->stack_count-1];
    }
    else
        return STACK_FAIL;

    return STACK_SUCCESS;
}

token_stack *
stack_reverse(token_stack *st)
{
    // This works as long as reversed is a pointer. The "proper" way
    // to do it is: malloc(sizeof (struct token_stack))
    token_stack * reversed = malloc(sizeof *reversed);

    size_t i = st->stack_count;
    size_t j = 0;

    while (i > 0)
    {
        // i will point to element insertion point, so decremement first.
	// j is pointing to first element (0) at beginning, so decrement
	// after.
        reversed->elements[j++] = st->elements[--i];
    }
    reversed->stack_count = st->stack_count;

    return reversed;
}

int
stack_clear(token_stack *st)
{
    // Using max_size_life causes massive valgind errors. But it seems
    // that stack_count doesn't neccesarily represent what was allocated.
    /* int n = st->max_size_life; */
    int n = st->stack_count;

    for (int i = 0; i < n; ++i)
        if (st->elements[i] != NULL)
            free(st->elements[i]);

    st->stack_count = 0;
    
    return STACK_SUCCESS;
}

int
stack_empty(token_stack *st)
{
    return (st->stack_count == 0);
}

int
stack_full(token_stack *st)
{
    return (st->stack_count == MAX_STACK_SIZE);
}
        
size_t
stack_size(token_stack *st)
{
    return st->stack_count;
}
