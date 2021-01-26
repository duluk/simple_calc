/*
 * This was the first implementation of the infix-to-postfix
 * converter as detailed in the Wikpedia article.  The function
 * has been updated in simple_calc.c since.
*/

#include <stdio.h>
#include <stdlib.h>  // for malloc/free
#include <string.h>  // for strlen
#include <ctype.h>   // isspace/isdigit

#include "calc_stack.h"

#define MAXBUF 512

int
is_operator(char test)
{
    int is_oper = 0;
    
    switch(test)
    {
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
	case '^':
            is_oper = 1;
            break;
        default:
            is_oper = 0;
    }

    return is_oper;
}

int
precedence(char op)
{
    int prec = 0;

    switch (op)
    {
        // This may be used later...
        /* case '(': */
        /*     prec = 5; */
        /*     break; */
        case '*':
        case '/':
        case '%':
            prec = 4;
            break;
        case '+':
        case '-':
            prec = 3;
            break;
        default:
            prec = 1;
            break;
    }

    return prec;
}

// This reverses it before printing it. Also adds space between
// tokens. (the whole reversing thing is messed up...not sure how
// my use of stack are wrong though)
const char *
make_expression(token_stack input)
{
    char * expr   = malloc(MAXBUF);
    char space[]  = " ";
    stack_type  * item = NULL;
    token_stack * ordered = NULL;

    memset(expr, '\0', MAXBUF);
    
    ordered = stack_reverse(&input);
    
    while (!stack_empty(ordered))
    {
        if ((stack_peek(ordered, &item)) != STACK_FAIL)
        {
            if (item != NULL)
            {
                // This is sort of ridiculous
                int l = strlen(expr);
                strncpy(expr+l, item, strlen(item));
                strncpy(expr+l+strlen(item), space, 1);
                *(expr+l+strlen(item)+1) = '\0';
            }
        }
        if ((stack_pop(ordered)) != STACK_SUCCESS)
        {
            fprintf(stderr, "Error popping off top of stack in make_expression.\n");
            free(expr); // I mean why not...or why? :)
            free(ordered);
            exit(EXIT_FAILURE);
        }
    }

    free(ordered);  // Not using this anymore; malloc'd by stack_reverse
    return expr;
}

int
left_associative(char op)
{
    // I'm leaving this even though I don't handle any operators that
    // are not left associative. Just return true.
    return 1;
}


// Closely following shunting-yard algorithm as detailed in Wikipedia
token_stack
parse_infix(char * infix)
{
    stack_type
        * top_op = NULL,
        * c = NULL,
        * p = NULL;

    token_stack
        output    = {0},
        operators = {0};

    // No way all these parens are needed
    if (((stack_initialize(&output)) != 0) ||
        ((stack_initialize(&operators)) != 0))
    {
        // By "extremely weird" I mean it's just assigning 0 to two ints
        fprintf(stderr, "Something extremely weird happened. Aborting.\n");
        exit(EXIT_FAILURE);
    }
    
    p = infix;
    while (*p != '\0')
    {
        if (isspace(*p))
        {
            ++p;
            continue;
        }

        // If it's a digit, get the whole number
        if (isdigit(*p))
        {
            c = p++; // p assigned to c first
            while (isdigit(*p))
                ++p;

            // I'm not sure this malloc is necessary. Is there a way
            // to track all of this without allocating any memory?
            // Like managing my own buffer. Seems a little complicated.
            stack_type * num = NULL;
            num = malloc(sizeof(p-c)+1);
            strncpy(num, c, sizeof(p-c));
            *(num+(p-c)) = '\0';
            stack_push(&output, num);
        }
        else if (is_operator(*p))
        {
            // I had this call in the while condition, but I'm not convinced
            // it was working as expected. This is better and more explicit.
            enum STACK_ERRORS ret = STACK_FAIL;
            ret = stack_peek(&operators, &top_op);
            while (
                (ret == STACK_SUCCESS) &&
                ((precedence(*top_op) > precedence(*p)) ||
                    ((precedence(*top_op) == precedence(*p)) && left_associative(*p))) &&
                (*top_op != '('))
            {
                stack_push(&output, top_op);
                stack_pop(&operators);
                ret = stack_peek(&operators, &top_op);
            }

            stack_type * op = malloc(2); 
            *op = *p;
            *(op+1) = '\0';
            stack_push(&operators, op);
            ++p;
        }
        else if (*p == '(')
        {
            // Mark our parenthetized expression. We'll use it for popping
            // off the operators when we hit the ')'.
            stack_type * op = malloc(2);
            *op = *p;
            *(op+1) = '\0';
            stack_push(&operators, op);
            ++p;
        }
        else if (*p == ')')
        {
            enum STACK_ERRORS ret = STACK_FAIL;
            ret = stack_peek(&operators, &top_op);
            while ((ret == STACK_SUCCESS) && (*top_op != '('))
            {
                stack_push(&output, top_op);
                stack_pop(&operators);
                ret = stack_peek(&operators, &top_op);
            }
            if (*top_op == '(')
            {
                stack_pop(&operators);
            } 
            else
            {
                printf("Invalid input. Mismatched parenthesis.\n");
                break;
            }
            ++p;
        }
    }

    // Now pop anything remaining off the operators stack and add it to
    // the output stack
    char * op = NULL;  // previous op out of scope
    while (!stack_empty(&operators))
    {
        stack_peek(&operators, &op);
        if ((*op == '(') || (*op == ')'))
        {
            // There are mismatch parenthesis
            printf("Invalid input. Mismatched parenthesis.\n");
            break;
        }
        stack_push(&output, op);
        stack_pop(&operators);
    }

    stack_clear(&operators);

    return output;
}

void
test_input(const char * input)
{
    token_stack output = {0};

    stack_initialize(&output);

    output = parse_infix(input);
    printf("Input:  %s\n", input);
    printf("Output: %s\n\n", make_expression(output));

    stack_clear(&output);
}
        
int
main(void)
{
    test_input("12+15/3*9");
    test_input("15/3*9-1");
    test_input("1-15/3*9");
    test_input("3*(1+2)");
    test_input("(1+3)*5");
    test_input("(1+3+4)*5");
    test_input("5*(1+3+4)/5");
    test_input("5*(1+2*3)/7");
    test_input("5*(1*2+3)/5");

    // This test is for precedence within parenthesis
    test_input("(1+3/2*4)*4/2*(355-12*2*6)");

}
