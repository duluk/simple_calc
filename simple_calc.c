// Getting this right and somewhat realistic (useful) will be big step
// toward understanding programming and C and structures/algorithms.

// This will at least teach pointer manipulation, along with working
// with ** (array of pointers), because of second argument to strol.
// (or, actually, because of assigning variables to parameters: e.g.,
// parse_infix or stack_peek in calc_stack.c)
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
//  1. *DONE* Allow entering '*2' as input and calculate based on last stored
//     result
//  2. *DONE* Error recovery. Entering 3+# will cause it to constantly read
//     input.
//  3. *DONE* Better use of data structures. Maybe rearrange the whole
//     line of input for better parsing, whether tree situation or even
//     just pre/post-fix notation.
//       expression = parse_input
//       evaluate(expression)
//  4. *DONE* Add variables. Oh yeah...
//  5. *DONE* Add power (^) operation
//  6. *DONE* Add () to evaluation
//  7. *DONE* Rename stack_top, stack_peek
//  8. *DONE* Make sure any item declared for stack element is using stack_type
//  9. *DONE* How to handle mismatched parenthesis. Not right currently.
// 10. *DONE* Add error checking for malloc calls
// 11. *DONE* Add double precision for results. Right now it's just integer
//     math (at least for input).
// 12. *DONE* Use calloc or memset to intialize the stack struct.
// 12a. Check that memset is actually working.
// 13. Find cause of memory leaks. (valgrind)
// 14. Allow negative numbers in input. The problem is how to
//     distinguish between a sign and an operator.
//     14a. - Also need to probably stop storing operands as strings but as
//     acutal numbers. Then when we do get a negative sign, multiply the number
//     by -1 to get the negative number.
// 15. Get rid of the treating numbers (operands) as strings. Convert
//     them to numbers as soon as possible and pass them around as numbers.
// 16. Add support for different bases:
//     set base to binary (2?)
//     mode binary
//     convert 45 to binary; convert 110011 to decimal; etc...

// Make the list of variables a hash of linked lists. Write a simple
// hash function that takes a variable and converts it to a number,
// from say 1 to 10. That will be an index into an array/hash table.
// Then each index has a linked list of variables. Instead of
// traversing the whole list each time looking for a variable, only
// have to iterate over a few.

// Actually - here is the data structure. Array of 26 elements.
// Convert the variable letter (only allowing single letters right now
// anyway) to a number (A=0, B=1, etc) and make that the index into
// the array. Then we don't need to even have the struct. Just have
// the array of numbers. Index given by user when entering a variable.

#include <stdio.h>
#include <stdlib.h> // malloc/free, EXIT_(SUCCESS|FAILURE)
#include <string.h> // strlen()
#include <ctype.h>  // isdigit() and isspace()
#include <math.h>   // pow()

#include <readline/readline.h>
#include <readline/history.h>

#include <mylib.h>
#include "simple_calc.h"

// Keep it secret, keep it safe
static calc_t last_result = 0L;

const char supported_operations[] =
    "addition, subtraction, multiplication, division, modulus, factorial, power";

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
        case '!':
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
factorial(calc_t n)
{
    if (n == 0)
        return 1;
    else
        return n * factorial(n-1);
}

calc_t
calculate(char *lnum, enum operators oper, char *rnum)
{
    calc_t
        left   = 0,
        right  = 0,
        result = 0;

    // This may be assuming too much. If lnum is NULL, then it may be
    // that something like "*2" was entered, to use the last result
    // for the left operand (basically). But we're sort of assuming that.
    // printf("Operator: %i %c\n", oper, operator_to_str(oper));
    // printf("Calculating: %s %c %s\n", lnum, operator_to_str(oper), rnum);
    if ((lnum == NULL) && last_result)
        left = last_result;
    else if (lnum != NULL)
        left = atoi(lnum);
    else if (oper != FAC)
    {
        // Factorial is a unary operator, so right operand would be ignored,
        // except in postfix notation the operand will be 'right', thus for
        // factorial the left operand will be NULL. That _should_ be the only
        // options at this point: NULL or FAC operator
        printf("Error with left operand in calculate.\n");
        return -1;
    }

    if (rnum != NULL)
        right = atoi(rnum);
    else
    {
        printf("Error with right operand in calculate.\n");
        return -1;
    }
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
            if (right == 0)
            {
                fprintf(stderr, "Attempt to modulo by zero. Returning zero.\n");
                result = 0;
            }
            else
            {
                result = (int)left % (int)right;
            }
            break;
        case FAC:
            // Factorial is a unary operator, so right operand would be ignored,
            // except in postfix notation the operand will be 'right'.

            // I have a feeling the loop is fater but wanted to try the recursive
            // function to make sure I knew hwo to do it.
            // result = 1;
            // for (int i = 1; i <= right; i++)
            //   result *= i;
            result = factorial(right);
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
        case '!':
            operator = FAC;
            break;
        case '^':
            operator = POW;
            break;
        default:
            // Again, compiler warning prevention...
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
        case FAC:
            op_c = '!';
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
        case '^':
            prec = 5;
            break;
        case '*':
        case '/':
        case '%':
        case '!':
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
// my use of stacks are wrong though; is it normal to reverse it prior
// to evaluating/displaying it?)
char *
stack_to_str(token_stack input)
{
    char * expr   = emalloc(MAXBUF);
    char space[]  = " ";
    stack_type  * item    = NULL;
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
            fprintf(stderr, "Error popping off top of stack in stack_to_str. Can't even.\n");
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
    // are right associative. Just return true.
    //   ...a few moments later...
    // Well, turns out ^ is right-associative. Who knew?
    return (op != '^');
}

// Closely following shunting-yard algorithm as detailed in Wikipedia
int
parse_infix(const char * infix, token_stack ** postfix_stack, calc_t * var_list)
{
    int
        ret_val = 0;

    stack_type
        * top_op = NULL,
        * c      = NULL,
        * p      = NULL;

    token_stack
      * output    = {0},  // Difference between this and NULL?
        operators = {0};

    output = emalloc(sizeof(token_stack));
    // No way all these parens are needed
    if (((stack_initialize(output)) != 0) ||
        ((stack_initialize(&operators)) != 0))
    {
        fprintf(stderr, "Something extremely weird happened. Aborting.\n");
        exit(EXIT_FAILURE);
    }

    p = (char *)infix;
    while (*p != '\0')
    {
        if (isspace(*p))
        {
            ++p;
            continue;
        }

        // If it's a digit, get the whole number, push to output stack
        if (isdigit(*p))
        {
            c = p++; // p assigned to c first
          /* proc_digit: */
            while (isdigit(*p))
                ++p;

            // I'm not sure this malloc is necessary. Is there a way
            // to track all of this without allocating any memory?
            // Like managing my own buffer. Seems a little complicated.
            stack_type * num = NULL;
            size_t len = p - c;       // 64-bits overkill but
            num = emalloc(len+1);
            strncpy(num, c, len);
            *(num+len) = '\0';
            stack_push(output, num);
        }
        /* else if (issign(*p)) */
        /* { */
        /*     if (isdigit(*(p+1))) */
        /*     { */
        /*         ++p; */
        /*         goto proc_digit; */
        /*     } */
        /* } */

        // If it's a letter (variable), find its value and push to
        // output stack, since it's the same as a number (above)
        else if (isalpha(*p))
        {
            calc_t n = 0L;
            stack_type * num = emalloc(MAX_LONG_LONG_STRLEN+1);
            if ((lookup_var(*p, var_list, &n)) == 0)
            {
                convert_long_to_str(n, num);
                stack_push(output, num);
            }
            ++p;
        }
        else if (is_operator(*p))
        {
            // I had this call in the while condition, but I'm not convinced
            // it was working as expected. This is better and more explicit.
            enum STACK_ERRORS ret = STACK_FAIL;
            ret = stack_peek(&operators, &top_op);

            // I really want to get this to look "right". Whoops.
            // What's happening is this: while the operator at the top
            // of the operators stack is higher or equal precedence
            // (basically) than the operator currently in the input
            // stream, but not a '(' - then we need to move the operator
            // at the top of the operators stack onto the output/postfix
            // stack to get the precedence right.
            while (
                   (ret == STACK_SUCCESS)
                && ((precedence(*top_op) > precedence(*p))
                 || ((precedence(*top_op) == precedence(*p))
                  &&  left_associative(*p)))
                && (*top_op != '(')
            )
            {
                stack_push(output, top_op);
                stack_pop(&operators);
                ret = stack_peek(&operators, &top_op);
            }

            stack_type * op = emalloc(2); 
            *op = *p;
            *(op+1) = '\0';
            stack_push(&operators, op);
            ++p;
        }
        else if (*p == '(')
        {
            // Mark our parenthetized expression. We'll use it for popping
            // off the operators when we hit the ')'.
            stack_type * op = emalloc(2);
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
                stack_push(output, top_op);
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
                stack_clear(output);
                stack_clear(&operators);
                ret_val = -1;
                goto broke; // :)
            }
            ++p;
        }
        else
        {
            ret_val = -1;
            goto broke;
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
            stack_clear(output);
            stack_clear(&operators);
            ret_val = -1;
            goto broke;   // :)
        }
        stack_push(output, op);
        stack_pop(&operators);
    }

    *postfix_stack = output;
    // printf("Postfix: %s\n", stack_to_str(*output));
  broke:
    stack_clear(&operators);
    return ret_val;
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

    char
        * str_result = NULL,
        * operand1   = NULL,
        * operand2   = NULL;

    calc_t result = 0.0L;

    enum operators operator = NUL;

    stack_initialize(&operands);

    // The stack as-made is actually reversed for processing, so we
    // will reverse it. I don't know if that's my algorithmic problem
    // or if that's normal. This essentially converts it to a queue.
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
            str_result = emalloc(MAX_LONG_LONG_STRLEN);
            convert_long_to_str(result, str_result);

            // Push result to operand stack so it's used in next calculation
            stack_push(&operands, str_result);

            // Remove the item from the stack; we're done with it.
            stack_pop(eval_ordered_stack);
        }
    }

    stack_clear(&operands);    // This doesn't free str_result...why?
    free(eval_ordered_stack);  // malloc'd by reverse_stack
    return result;
}

void
assert_calc(const char * expr, calc_t expected, calc_t * vars)
{
    token_stack * r_st = NULL;
    char * postfix_str = NULL;

    calc_t asw = 0.0L;

    printf("%s: ", expr);
    if ((parse_infix(expr, &r_st, vars)) == 0)
    {
        postfix_str = stack_to_str(*r_st);
        asw = postfix_evaluate(r_st);

        if (asw != expected)
        {
            printf("FAILED: evaluated to %f instead of %f.\n", asw, expected);
            printf("Parsed expression: %s\n", postfix_str);
        }
        else
        {
            printf("PASSED!\n");
        }

        stack_clear(r_st);
        // postfix_str allocated by stack_to_str
        free(postfix_str);
    }

    // r_st allocated in parse_infix
    free(r_st);
}

void
run_tests(void)
{
    calc_t vars[MAX_VAR] = {0};

    // Maybe get rid of some of these...

    // addition/subtraction
    assert_calc("2+3", 5, vars);
    assert_calc("2-5", -3, vars);
    assert_calc("15-5-2", 8, vars);

    // multiplication
    assert_calc("15*0", 0, vars);
    assert_calc("15*1", 15, vars);
    assert_calc("123*42", 5166, vars);

    // division
    assert_calc("12/4", 3, vars);
    assert_calc("12/0", 0, vars);   // Division by zero returns zero
    assert_calc("156/156", 1, vars);
    assert_calc("974832/138", 7064, vars);

    // power
    assert_calc("2^0", 1, vars);
    assert_calc("2^1", 2, vars);
    assert_calc("9^3", 729, vars);
    assert_calc("127^3", 2048383, vars);

    // modulus
    assert_calc("1234%123", 4, vars);
    assert_calc("91872364%1234", 1064, vars);
    assert_calc("100%1004", 100, vars);
    assert_calc("1000%1000", 0, vars);

    // factorial
    assert_calc("0!", 1, vars);
    assert_calc("1!", 1, vars);
    assert_calc("2!", 2, vars);
    assert_calc("12!", 479001600, vars);

    // same operator mult. times, which caused problems with one of my
    // parsing implementations.
    assert_calc("9*9*9", 729, vars);
    assert_calc("2+4+6+8", 20, vars);
    assert_calc("8-2-4-2", 0, vars);
    assert_calc("2*2*3*4", 48, vars);
    assert_calc("144/12/3", 4, vars);
    assert_calc("4096%100%10", 6, vars);

    // precedence
    assert_calc("2+3*8", 26, vars);
    assert_calc("1+2*3+4", 11, vars);
    assert_calc("12+15/3*9", 57, vars);
    assert_calc("15/3*9-1", 44, vars);
    assert_calc("1-15/3*9", -44, vars);

    // parenthesis
    assert_calc("2*(1+2)", 6, vars);
    assert_calc("(1+3)*5", 20, vars);
    assert_calc("(1+3+4)*5", 40, vars);
    assert_calc("(1+2)*(3+4)", 21, vars);
    assert_calc("5*(1+3+4)/5", 8, vars);
    assert_calc("5*(1+2*3)/7", 5, vars);
    assert_calc("5*(1*2+3)/5", 5, vars);

    // precedence within parenthesis
    assert_calc("(1+3/2*4)*4/2*(355-12*2*6)", 2110, vars);

    // Test variables (and hash_var)
    vars[hash_var('a')] = 10;
    vars[hash_var('h')] = 100;
    vars[hash_var('z')] = 1000;

    assert_calc("a*5+3", 53, vars);
    assert_calc("5+3*a", 35, vars);
    assert_calc("12*h/3", 400, vars);
    assert_calc("15*(z+3)", 15045, vars);
    assert_calc("(a*h+z)/a", 200, vars);

    // malformed expressions with parenthesis
    printf("\nThe following two tests should fail with mismatched parens.\n");
    assert_calc("12*2+3)", 60, vars);
    assert_calc("12*(2+3", 60, vars);

    // test assert_calc for failed test
    printf("\nThe following test should fail. Testing assert_calc function.\n");
    assert_calc("12*(2+3)", 0, vars);
}

int
main(int argc, char **argv)
{
    char *input = NULL;
    token_stack * parsed_input = NULL;
    Commands command = NONE;

    // Very simple argument parsing
    if (argc > 1)
    {
        // Casting malloc is not required for C. (last I checked)
        // Don't plan on compiling this for C++.
        char * usage = emalloc(75);
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
        printf("Duluk's Simple Calculator. (c) 2021.\n");
        printf("Basic maths - %s\n", supported_operations);
        printf("Enter your expressions, please. Empty line will exit.\n");

        for ( ; ; )
        {
            // Keep it around for each call (0-init only on first iteration)
            static calc_t var_list[MAX_VAR] = {0};

            input = rl_gets("");

            // readline (rl_gets) removes the newline, so testing
            // against \0 instead of \n.
            if (input[0] != '\0')
            {
                // We are handling English only, so let's look for ASCII only,
                // nip any other languages/unicode in the bud.
                if ((command = command_found(input)))
                {
                    return_code ret = FAILED;
                    Variable * var = {0};
                    ret = parse_command(input, &var, command);
                    if (ret != FAILED)
                    {
                        // Add the variable to the "hash-based" array
                        int h = hash_var(var->name);
                        // TODO: fix a memory leak that will occur if the
                        // variable already has something stored in the
                        // array.
                        var_list[h] = var->value;

                        printf("%c set to %f\n", var->name, var->value);

                    }
                    else
                    {
                        printf("Invalid input.\n");
                    }
                }
                else if ((parse_infix(input, &parsed_input, var_list)) == 0)
                {
                    last_result = postfix_evaluate(parsed_input);
                    printf("%f\n", last_result);

                    stack_clear(parsed_input);
                    // parsed_input allocated in parse_infix
                    free(parsed_input);
                }
                else
                {
                    printf("Invalid input.\n");
                }
            }
            else
            {
                break;
            }
        }
    }

    return 0;
}
