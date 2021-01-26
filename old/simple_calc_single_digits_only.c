// This was fun to write but for now it handles only single digit
// input and output. Will be fun to expand on this in the future.
// Getting this right and somewhat realistic (useful) will be big step
// toward understanding programming and C and structures/algorithms.

// This is busted. Well, as it is - it isn't accounting for
// multi-digit numbers. Sort of a big deal. What it needs to do is
// something like this:
// 1. Call strtol on the input. This will convert all characters
// to numbers up to the first invalid digit, which would be the operator.
// 2. Store the operator. Move the pointer forward.
// 3. Call strtol again.
// 4. Calculate the result. (not doing precendence yet...that's tuff)
// 5. Repeat this until newline is found.
//
// This will at teach pointer manipulation, along with working
// with ** (because of second argument to strol)
//
// When add precedence, then I will learn about data structures
// and algorithms for real because will probably want to store the
// input in some sort of tree/graph/something. Sorted somehow.

#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS/EXIT_FAILURE
#include <errno.h>  // for strtol()
#include <limits.h> // for LONG_(MIN|MAX)
#include <string.h> // for strlen()
#include <ctype.h>  // for isdigit()

#define and &&
#define or  ||

#define MAXBUF  120
#define DECIMAL 10

enum operators {
    NUL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};

long
convert_to_num(const char *str)
{
    long res = 0;

    res = strtol(str, NULL, DECIMAL);
    if (res == LONG_MIN)
    {
        fprintf(stderr, "Could not convert %s to a number without underflowing.\n", str);
        return LONG_MIN;
    }
    else if (res == LONG_MAX)
    {
        fprintf(stderr, "Could not convert %s to a number without overflowing.\n", str);
        return LONG_MAX;
    }
    else if ((res = 0) and (errno == EINVAL))
    {
        fprintf(stderr, "Could not convert %s to a number.\n", str);
        exit(EXIT_FAILURE);
    }
}

        
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
            is_oper = 1;
            break;
        default:
            is_oper = 0;
    }

    return is_oper;
}

// Want to see if works for single digit numbers
long
calculate(char lnum, enum operators oper, char rnum)
{
    long left = 0,
         right = 0;
    long result = 0;

    // Assuming valid input. :) This will happen elsewhere in final
    // version. This function, if it remains, will receive actual numbers.
    left  = atoi(&lnum);
    right = atoi(&rnum);
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
    }

    return result;
}


int
main(int argc, char **argv)
{
    char input[MAXBUF];

    int  have_operator = 0,
         first_op = 1;

    long res = 0,
         working_res = 0;
    char loperand = 0,
         roperand = 0;
    enum operators operator = NUL;
    
    
    scanf("%s", input);

    for (int i = 0; i < strlen(input); ++i)
    {
        if (isdigit(input[i]) and !have_operator)
        {
            // This should only be true for first number
            loperand = input[i];
            continue;
        }
        else if (isdigit(input[i]) and have_operator)
        {
            roperand = input[i];
            if (!first_op)
                loperand = working_res + '0';
            else
                first_op = 0;
            working_res = calculate(loperand, operator, roperand);
            have_operator = 0;
            continue;
        }

        if (is_operator(input[i]))
        {
            // We may need to check the next value for a double-char operator
            switch(input[i])
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
                default:
                    operator = NUL;
                    break;
            }
            if (operator != NUL)
            {
                have_operator = 1;
            }
        }                    
    }
    
    printf("%ld\n", working_res);
    return 0;
}
