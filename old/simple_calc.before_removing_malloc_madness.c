// Getting this right and somewhat realistic (useful) will be big step
// toward understanding programming and C and structures/algorithms.

// While this now accounts for multi-digit numbers, I still think
// something more like this may be better:
// 1. Call strtol on the input. This will convert all characters
// to numbers up to the first invalid digit, which would be the operator.
// 2. Store the operator. Move the pointer forward.
// 3. Call strtol again.
// 4. Calculate the result. (not doing precendence yet...that's tuff)
// 5. Repeat this until newline is found.
//
// This will at least teach pointer manipulation, along with working
// with ** (array of pointers), because of second argument to strol.
//
// When I add precedence, then I will learn about data structures
// and algorithms for real because will probably want to store the
// input in some sort of tree/graph/something. Sorted somehow. Or not
// - really it's more of an algorithm thing. A stack/queue is
// involved, so that could be interesting. But see below -
// shunting-yard algorithm seems to work (on paper at least).

// TODO:
//
// 1. Allow entering '*2' as input and calculate based on last stored
//    result
// 2. Error recovery. Entering 3+# will cause it to constantly read
//    input.
// 3. Better use of data structures. Maybe rearrange the whole line of
//    input for better parsing, whether tree situation or even just
//    pre/post-fix notation.
//      expression = parse_input
//      evaluate(expression)
// Try using the shunting-yard algorithm for postfix notation:

 /* The input is processed one symbol at a time: if a variable or
    number is found, it is copied directly to the output. If the
    symbol is an operator, it is pushed onto the operator stack. If
    the operator's precedence is lower than that of the operators at
    the top of the stack or the precedents are equal and the operator
    is left associative, then that operator is popped off the stack
    and added to the output. Finally, any remaining operators are
    popped off the stack and added to the output. -Wikipedia
 */

// 4. Add variables. Oh yeah...
// 5. *DONE* Add power (^) operation


#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS/EXIT_FAILURE
#include <errno.h>  // for strtol()
#include <limits.h> // for LONG_(MIN|MAX)
#include <string.h> // for strlen()
#include <ctype.h>  // for isdigit()
#include <math.h>   // for pow()

#include <readline/readline.h>
#include <readline/history.h>

#include "mylib.h"
#include "simple_calc.h"
//#include "calc_stack.h"

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

long
calculate(char *lnum, enum operators oper, char *rnum)
{
    long left = 0,
         right = 0;
    long result = 0;

    // Assuming valid input. :) This will happen elsewhere in final
    // version. This function, if it remains, will receive actual numbers.
    left  = atoi(lnum);
    right = atoi(rnum);
    /* left = convert_str_to_long(lnum); */
    /* right = convert_str_to_long(rnum); */
    switch (oper)
    {
        case ADD:
            result = left + right;
            break;
        case SUB:
            result = left - right;
            break;
        case MUL:
            result = left * right;
            break;
        case DIV:
            if (right == 0)
            {
                fprintf(stderr, "Attempt to divide by zero. Returning zero.\n");
                result = 0;
            }
            else
            {
                result = left / right;
            }
            break;
        case MOD:
            result = left % right;
            break;
	case POW:
	    result = pow(left, right);
	    break;
        case NUL:
            // I'm tired of the compiler warning...
            break;
    }

    return result;
}

long
simple_evaluate(const char * expression)
{
    const char * p = NULL;

    int
        need_to_calculate = 0,
        have_operator = 0,
        first_done = 0,
        first_op = 1;

    long
        working_res = 0;

    char
        *left_num_str = NULL,
        *left_char = NULL,
        *right_num_str = NULL,
        *right_char = NULL;

    enum operators operator = NUL;

    // Setting it to null terminator just because it seems good to me
    left_num_str  = (char*)malloc(MAXBUF);
    memset(left_num_str, '\0', MAXBUF);
    left_char = left_num_str;

    right_num_str = (char*)malloc(MAXBUF);
    memset(right_num_str, '\0', MAXBUF);
    right_char = right_num_str;
    
    p = expression;
    while (*p != '\0')
    {
        if (isspace(*p))
        {
            ++p;
            continue;
        }
        
        // Maybe convert these to numbers as we go...like:
        // n = 0; first
        // n = n * 10 + (*p++ + '0')
        if (isdigit(*p) and !first_done)
        {
            // This should only be true for first number
            *left_char++ = *p++;
        }
        /* else if (isdigit(*p) and !need_to_calculate) // and have_operator) */
        else if (isdigit(*p) and have_operator)
        {
            *right_char++ = *p++;
            // *p is now the next character. If it's an operator then we've got
            // a string of operators and need to calculate the one we have. If
            // it's a null, then we're done with the input and need to calculate
            // the last expression.
            if ((*p == '\0') or (is_operator(*p))) 
            {
                // This seems like a hack
                need_to_calculate = 1;
            }
        }
        else if (is_operator(*p))
        {
            // We may need to check the next value for a double-char operator
            switch(*p++)
            {
                case '+':
                    operator = ADD;
                    break;
                case '-':
                    operator = SUB;
                    break;
                case '*':
                    operator = MUL;
                    break;
                case '/':
                    operator = DIV;
                    break;
                case '%':
                    operator = MOD;
                    break;
                case '^':
                    operator = POW;
                    break;
                default:
                    operator = NUL;
                    break;
            }
            if (operator != NUL)
            {
                if (!first_done)
                    first_done = 1;

                if (have_operator)
                {
                    // We have hit two operators with no calculation;
                    // therefore, we have a multi-operator request and
                    // we need to do it.
                    need_to_calculate = 1;
                }
                else
                {
                    have_operator = 1;
                    need_to_calculate = 0;
                }
            }
        }

        if (need_to_calculate)
        {
            if (!first_op)
            {
                // Set left operand to working_res
                convert_long_to_str(working_res, left_num_str);
            }
            else
            {
                first_op = 0;
            }

            // This is a mess. Just look at it.
            working_res = calculate(left_num_str, operator, right_num_str);
            have_operator = 0;
            need_to_calculate = 0;
            memset(left_num_str, '\0', MAXBUF);
            left_char = left_num_str;
            memset(right_num_str, '\0', MAXBUF);
            right_char = right_num_str;
        }
    }

    if (left_num_str)
        free(left_num_str);
    if (right_num_str)
        free(right_num_str);

    return working_res;
}

int
precedence(char op)
{
    int prec = 0;

    switch (op)
    {
        // This may not be needed
        case '(':
            prec = 5;
            break;
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
            
// This is broken. Puts it in reverse order. But I don't think I can
// use it anyway. A simple string doesn't separate numbers. So I will
// just handle the stack itself. Makes more sense anyway really. It is
// interesting to consider though - how to make it a string, correctly
// (that is, not reversed). The stack is by nature reversed.
char *
make_expression(token_stack input)
{
    char * expr = malloc(MAXBUF);
    stack_type * item = NULL;

    while (!stack_empty(&input))
    {
        if ((stack_top(&input, &item)) != STACK_FAIL)
        {
            if (item != NULL)
            {
                int l = strlen(expr);
                strncpy(expr+l, item, strlen(item)); // ???
                *(expr+l+strlen(item)) = '\0';
            }
        }
        if ((stack_pop(&input)) != STACK_SUCCESS)
        {
            fprintf(stderr, "Error popping off top of stack in make_expression.\n");
            exit(EXIT_FAILURE);
        }
    }
    return expr;
}

// The memory management in here is probably atrocious. How to know?
// (be a C programmer? :))
token_stack
parse_input(char * input)
 /* The input is processed one symbol at a time: if a variable or
    number is found, it is copied directly to the output. If the
    symbol is an operator, it is pushed onto the operator stack. If
    the operator's precedence is lower than that of the operators at
    the top of the stack or the precedents are equal and the operator
    is left associative, then that operator is popped off the stack
    and added to the output. Finally, any remaining operators are
    popped off the stack and added to the output. -Wikipedia
 */
{
    char * p = input;
    token_stack parsed, operators;

    // No way all these parens are needed
    if (((stack_initialize(&parsed)) != 0) ||
        ((stack_initialize(&operators)) != 0))
    {
        fprintf(stderr, "Something extremely weird happened. Aborting.\n");
        exit(EXIT_FAILURE);
    }

    
    char * c = NULL;
    char * num = NULL;
    char * top_op = NULL;
    while (*p != '\0')
    {
        if (isspace(*p))
        {
            ++p;
            continue;
        }
        
        if (isdigit(*p))
        {
            c = p++; // p assigned to c first
            while (isdigit(*p))
                ++p;
            // I'm not sure this malloc is necessary. Is there a way
            // to track all of this without allocating any memory?
            // Like managing my own buffer. Seems a little complicated.
            num = malloc(sizeof(p-c));
            strncpy(num, c, sizeof(p-c));
            *(num+(p-c)) = '\0';
            stack_push(&parsed, num);
        }

        if (is_operator(*p))
        {
            if ((stack_top(&operators, &top_op)) == STACK_SUCCESS)
            {
                if (precedence(*p) <= precedence(*top_op))
                {
                    while (precedence(*p) <= precedence(*top_op))
                    {
                        // Error checking?
                        // Has to be a malloc or something needed in here
                        char * op = malloc(2); // ridiculous to just allocate 2;
                                               // need to manage own buffer
                                               // or something
                        *op = *top_op;
                        *(op+1) = '\0';
                        stack_push(&parsed, op);
                        stack_pop(&operators);
                        /* stack_push(&parsed, top_op); */
                        stack_top(&operators, &top_op);
                    }
                }
            }
            // p is pointing to rest of the expression. Need just the
            // operator. For now this is one character.
            char * op = malloc(2); // ridiculous to just allocate 2;
                                   // need to manage own buffer or something
            *op = *p;
            *(op+1) = '\0';
            stack_push(&operators, op);
            ++p;
        }
    }

    // Now pop everything off the operators stack and add it to the
    // parsed stack
    char * op = NULL;  // previous op out of scope
    char * m = NULL;
    while (!stack_empty(&operators))
    {
        // There may be a problem here. stack_pop is going to free the
        // memory associated with op. But it has been assigned to parsed.
        // So we are going to allocate yet more memory.
        stack_top(&operators, &op);
        m = malloc(2);
        strncpy(m, op, 2);
        *(m+1) = '\0';
        stack_push(&parsed, m);
        stack_pop(&operators);
    }

    /* *output = make_expression(parsed); */
    
    return 0;
}

long
postfix_evaluate(token_stack * input)
    /*
      1. Reverse the stack (for now just do it)
      2. Walk the stack
      3. Retrieve top of stack
      4. If number, push to operand stack
      5. If operator, pop off last two operands and calculate
      6. Save the result
      7. Goto 3.
    */
{
    token_stack operands;
    char *
        operand1 = NULL,
        operand2 = NULL;

    enum operators operator = NUL;

    char * token = NULL;
    while (!stack_empty(input))
    {
        stack_top(input, &token);
        if (!is_operator(*token))  // Write is_number function?
        {
            // I think strlen works for strings because characters are
            // 1 byte. May not work for multi-byte unicode characters though.
            char * num = malloc(strlen(token)+1);
            strncpy(num, token, strlen(token)+1);
            stack_push(&operands, num);
            stack_pop(&input);
        }
        else
        {
            stack_top(input, &operand1);
            stack_pop(&input);
            stack_top(input, &operand2);
            stack_pop(&input);
        }
    
}

void
assert_calc_func(const char * expr, long expected, long (*func)(const char *))
{
    long r = 0l;

    r = (*func)(expr);

    if (r != expected)
    {
        printf("%s FAILED: evaluated to %ld instead of %ld.\n", expr, r, expected);
    }
    else
    {
        printf("%s PASSED!\n", expr);
    }
}

void
run_tests(void)
{
    // Let's enter the function in just one place
    long (*f)(const char *) = simple_evaluate;
    
    assert_calc_func("2+3", 5, f);
    assert_calc_func("2+3*8", 40, f); // Precedence doesn't exit yet
    assert_calc_func("2^11", 2048, f);
    assert_calc_func("2^0", 1, f);
    assert_calc_func("2^1", 2, f);
    assert_calc_func("12/0", 0, f);   // Division by zero returns zero
    assert_calc_func("127^3", 2048383, f);
    assert_calc_func("9*9*9", 729, f);
    assert_calc_func("9^3", 729, f);
}

int
main(int argc, char **argv)
{
    /* char input[MAXBUF] = ""; */
    char *input = NULL;

    long res = 0L;
    char * expression = NULL;

    token_stack parsed_input;
    
    // Very simple argument parsing
    if (argc > 1)
    {
        char * usage = (char *) malloc(75);
        snprintf(usage, 75, "%s [-t]  (-t = run tests)\n", *argv+2);
        if (argv[1][0] == '-')  // Makes more sense than *(*argv+1)
        {
            switch(argv[1][1])
            {
                case 't':
                    run_tests();
                    break;
                case 'h':
                    puts(usage);
                    break;
            }
        }
	else
	{
            puts(usage);
	}
    }
    else // I don't like this. Maybe a nice goto with the args...
    {
        printf("Trey's Simple Calculator.\n");
        printf("Basic maths - addition, subtraction, multiplcation, division, modulo.\n");
        printf("Enter your equations, please. Empty line will exit.\n");
    
        // readline allocates buffer for us; rl_gets frees it on each call
        /* input = malloc(MAXBUF); */
        /* memset(input, '\0', MAXBUF); */
    
        for ( ; ; )
        {
            /* read_stdin(input); */
            input = rl_gets("");

            // readline (rl_gets) removes the newline, so testing
            // against \0 instead of \n.
            if (input[0] != '\0')
            {
                /* res = simple_evaluate((const char *)input); */
                parsed_input = parse_input(input);
                res = postfix_evaluate(&parsed_input);
                printf("%ld\n", res);
            }
            else
            {
                break;
            }
        }
    }

    return 0;
}
