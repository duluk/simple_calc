#include <stdio.h>
#include <string.h>  // For memcpy
#include <stdlib.h>  // For malloc

#include "calc_stack.h"

#define chomp(str) if ((str)[strlen((str))-1] == '\n') (str)[strlen((str))-1] = '\0'
#define TEST_SIZE 5

const int MAX_BUF = 1024;

int
main(int argc, char **argv)
{
    char input[MAX_BUF];
    char * input_copy = NULL;
    token_stack s;
    
    if ((stack_initialize(&s)) != 0)
    {
        fprintf(stderr, "Something extremely weird happened. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    
    for (int i = 0; i < TEST_SIZE; ++i)
    {
        printf("Expression %d: ", i+1);
        fgets(input, MAX_BUF, stdin);
        chomp(input);

        input_copy = malloc(strlen(input)+1);
        strncpy(input_copy, input, strlen(input));
        if ((stack_push(&s, input_copy)) != STACK_SUCCESS)
        {
            fprintf(stderr, "Error pushing %s onto the stack.\n", input_copy);
        }

        // Do I need to clear input? memset?
    }

    printf("stack_size = %ld\n", stack_size(&s));
    
    stack_type * item;
    while (!stack_empty(&s))
    {
        if ((stack_top(&s, &item)) != STACK_FAIL)
        {
            if (item != NULL)
            {
                printf("%s\n", item);
            }
            else
            {
                fprintf(stderr, "item from stack_top is NULL; "
                    "however, s.elements[s.stack_count-1] = %s.\n",
                    s.elements[s.stack_count-1]);
                exit(EXIT_FAILURE);
            }
            if ((stack_pop(&s)) != STACK_SUCCESS)
            {
                fprintf(stderr, "Error removing element from stack.\n");
                // Is this an abort-worthy error?
            }
        }
    }

    return 0;
}
