// Getting this right and somewhat realistic (useful) will be big step
// toward understanding programming and C and structures/algorithms.

// This will at least teach pointer manipulation, along with working
// with ** (array of pointers), because of second argument to strol.
//
// When I add precedence, then I will learn about data structures
// and algorithms for real because will probably want to store the
// input in some sort of tree/graph/something. Sorted somehow. Or not
// - really it's more of an algorithm thing. A stack/queue is
// involved, so that could be interesting. But see below -
// shunting-yard algorithm seems to work (on paper at least).

// 20200103 - Implemented version that handles only single digit numbers
// 20200106 - Implemented version that parses from left to right,
// ignoring precedence but handles multi-digit numbers.
// 20200110 - Implemented stack-based, postfix notation evaluation
// using the shunting-yard algorithm to convert infix to postfix.

// TODO:
//
// 1. Allow entering '*2' as input and calculate based on last stored
//    result
// 2. Error recovery. Entering 3+# will cause it to constantly read
//    input.
// 3. *DONE* Better use of data structures. Maybe rearrange the whole
//    line of input for better parsing, whether tree situation or even
//    just pre/post-fix notation.
//      expression = parse_input
//      evaluate(expression)
// 4. Add variables. Oh yeah...
// 5. *DONE* Add power (^) operation
// 6. *DONE* Add () to evaluation
// 7. *DONE* Rename stack_top, stack_peek
// 8. Make sure any item declared for stack element is using stack_type
// 9. How to handle mismatched parenthesis. Not right currently.


#include <stdio.h>
#include <stdlib.h> // for malloc/free, EXIT_SUCCESS/EXIT_FAILURE
#include <string.h> // for strlen()
#include <ctype.h>  // for isdigit() and isspace()
#include <math.h>   // for pow()

#include <readline/readline.h>
#include <readline/history.h>

#include "mylib.h"
#include "simple_calc.h"

const char supported_operations[] = "addition, subtraction, multiplication, division, modulus, power";

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

void
display_expression(char *op1, enum operators oper, char *op2)
{
    char op = operator_to_str(oper);
    
    printf("Calculating: %s %c %s\n", op1, op, op2);
}

calc_t
calculate(char *lnum, enum operators oper, char *rnum)
{
    calc_t
        left   = 0,
        right  = 0,
        result = 0;

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

enum operators
get_operator(char op)
{
    enum operators operator = NUL;
    
    switch(op)
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

    return operator;
}

char
operator_to_str(enum operators op)
{
    char op_c = ' ';
    
    switch(op)
    {
        case ADD:
            op_c = '+';
            break;
        case SUB:
            op_c = '-';
            break;
        case MUL:
            op_c = '*';
            break;
        case DIV:
            op_c = '/';
            break;
        case MOD:
            op_c = '%';
            break;
        case POW:
            op_c = '^';
            break;
        default:
            op_c = '0';
            break;
    }

    return op_c;
}

int
precedence(char op)
{
    int prec = 0;

    switch (op)
    {
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
char *
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
                // What's happening is this: while the operator at the top
                // of the operators stack is higher or equal precedence
                // (basically) than the operator currently in the input
                // stream, but not a ( - then we need to move the operator
                // at the top of the operators stack onto the output/postfix
                // stack to get the precedence right.
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
            // Precedence is handled via the normal path. What's really
            // happening with the () is that we pop the operators inside the
            // parenthesis "early."
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
            // There shouldn't be any parenthesis at this point. If
            // there are, we have mismatched parenthesis.
            printf("Invalid input. Mismatched parenthesis.\n");
            break;
        }
        stack_push(&output, op);
        stack_pop(&operators);
    }

    stack_clear(&operators);

    return output;
}

calc_t
postfix_evaluate(token_stack * input)
    /*
      Ex: A*B+C*D = AB*CD*+
      1. Reverse the stack (for now just do it)
      2. Walk the stack
      3. Retrieve top of stack
      4. If number, push to operand stack
      5. If operator, pop off last two operands and calculate
      6. Push result to operand stack
      7. Goto 3.
    */
{
    token_stack operands = {0};
    token_stack * eval_ordered_stack = NULL;

    char * str_result = NULL;
    
    char * operand1 = NULL;
    char * operand2 = NULL;

    calc_t result = 0L;
    
    enum operators operator = NUL;

    stack_initialize(&operands);
    
    eval_ordered_stack = stack_reverse(input);
    
    char * token = NULL;
    while (!stack_empty(eval_ordered_stack))
    {
        stack_peek(eval_ordered_stack, &token);
        if (!is_operator(*token))  // Write is_number function?
        {
            stack_push(&operands, token);
            stack_pop(eval_ordered_stack);
        }
        else
        {
            // What if there aren't two operands to pop off? Need to
            // use last_result. Need to work through the algorithm
            // more carefully. It's a little too simple right now.
            // Since we're popping off a stack, operand 2 is "above"
            // operand 1. It matters (e.g., subtraction/division).
            stack_peek(&operands, &operand2);
            stack_pop(&operands);
            stack_peek(&operands, &operand1);
            stack_pop(&operands);

            operator = get_operator(*token);
            /* display_expression(operand1, operator, operand2); */

            result = calculate(operand1, operator, operand2);

            // Don't need these anymore. Get rid of those leaks.
            // Commenting out. Causes issue when the input stack is
            // cleared in main.
            /* free(operand1); */
            /* free(operand2); */
            
            // Valgrind says this is a leak. I don't know how/where to free
            // them though. If I do it above, then valgrind says I'm illegally
            // freeing. (though app doesn't crash ... undefined?)
            str_result = malloc(MAX_LONG_LONG_STRLEN);
            convert_long_to_str(result, str_result);

            // Push result to operand stack so it's used in next calculation
            stack_push(&operands, str_result);

            // Remove the item from the stack; we're done with it.
            stack_pop(eval_ordered_stack);
        }
    }

    stack_clear(&operands);    // Not sure this is right way
    free(eval_ordered_stack);  // malloc'd by reverse_stack
    return result;
}

void
assert_calc(const char * expr, calc_t expected)
    // The function pointer was cool when it was all one thing. That
    // ain't happening now.
{
    token_stack r_st = {0};
    calc_t r = 0l;
    char * expr_str = NULL;

    stack_initialize(&r_st);
    
    r_st = parse_infix(expr);
    expr_str = make_expression(r_st);
    r = postfix_evaluate(&r_st);

    if (r != expected)
    {
        printf("%s FAILED: evaluated to %lld instead of %lld.\n", expr, r, expected);
        printf("Expression: %s\n", expr_str);
    }
    else
    {
        printf("%s PASSED!\n", expr);
    }

    stack_clear(&r_st);
    free(expr_str);
}

void
run_tests(void)
{
    // Maybe get rid of some of these...
    assert_calc("2+3", 5);
    assert_calc("2-5", -3);
    assert_calc("15-5-2", 8);
    assert_calc("15*0", 0);
    assert_calc("15*1", 15);
    assert_calc("12/4", 3);
    assert_calc("12/0", 0);   // Division by zero returns zero
    assert_calc("156/156", 1);
    assert_calc("2^11", 2048);
    assert_calc("2^0", 1);
    assert_calc("2^1", 2);
    assert_calc("127^3", 2048383);
    assert_calc("1234%123", 4);
    assert_calc("91872364%1234", 1064);
    assert_calc("100%1004", 100);
    assert_calc("1000%1000", 0);
    assert_calc("9*9*9", 729);
    assert_calc("9^3", 729);
    assert_calc("2+4+6+8", 20);
    assert_calc("8-2-4-2", 0);
    assert_calc("2*2*3*4", 48);
    assert_calc("144/12/3", 4);
    assert_calc("4096%100%10", 6);
    assert_calc("2+3*8", 26);
    assert_calc("1+2*3+4", 11);
    assert_calc("12+15/3*9", 57);
    assert_calc("15/3*9-1", 44);
    assert_calc("1-15/3*9", -44);
    assert_calc("2*(1+2)", 6);
    assert_calc("(1+3)*5", 20);
    assert_calc("(1+3+4)*5", 40);
    assert_calc("5*(1+3+4)/5", 8);
    assert_calc("5*(1+2*3)/7", 5);
    assert_calc("5*(1*2+3)/5", 5);

    // This test is for precedence within parenthesis
    assert_calc("(1+3/2*4)*4/2*(355-12*2*6)", 2110);

    // Test malformed expressions with parenthesis
    assert_calc("12*2+3)", 60);
    assert_calc("12*(2+3", 60);
}

int
main(int argc, char **argv)
{
    char *input = NULL;
    calc_t res = 0L;
    token_stack parsed_input;

    
    stack_initialize(&parsed_input);
    
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
        printf("Trey's Simple Calculator. (c) 2021.\n");
        printf("Basic maths - %s\n", supported_operations);
        printf("Enter your equations, please. Empty line will exit.\n");
    
        for ( ; ; )
        {
            input = rl_gets("");

            // readline (rl_gets) removes the newline, so testing
            // against \0 instead of \n.
            if (input[0] != '\0')
            {
                parsed_input = parse_infix(input);
                res = postfix_evaluate(&parsed_input);
                printf("%lld\n", res);
                stack_clear(&parsed_input);
            }
            else
            {
                break;
            }
        }
    }

    return 0;
}
