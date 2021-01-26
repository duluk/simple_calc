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
// with ** (because of second argument to strol)
//
// When I add precedence, then I will learn about data structures
// and algorithms for real because will probably want to store the
// input in some sort of tree/graph/something. Sorted somehow.

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

#include <stdio.h>
#include <stdlib.h> // for EXIT_SUCCESS/EXIT_FAILURE
#include <errno.h>  // for strtol()
#include <limits.h> // for LONG_(MIN|MAX)
#include <string.h> // for strlen()
#include <ctype.h>  // for isdigit()
#include <readline/readline.h>
#include <readline/history.h>

//#sorrynotsorry
#define and &&
#define or  ||

#define MAXBUF  1024
#define DECIMAL 10

enum operators {
    NUL,
    ADD,
    SUB,
    MUL,
    DIV,
    MOD
};

// I don't know the best function to use for input, but using
// "scanf("%s", input)" caused spaces to mess up the result, due to
// buffering stuff and when scanf returns. So we'll use this. Maybe
// fgets or something is better. (switching to readline anyway)
int
read_stdin(char * s)
{
    char c = 0;
    char*p = s;
    int  i = 0;
    
    do
    {
        c = fgetc(stdin);
        // Can't do *s++ because that's moving the memory address
        // pointed to by the actual variable passed in, so when
        // control passed back to the process, it's basically invalid
        // (pointed after the string).
        *p++ = c;
        ++i;
    } while (c != '\n');
    s[i] = '\0';          // Keep the \n

    return i;
}

// From the GNU docs. Just gonna use their input, for history.
/* A static variable for holding the line. */
static char *line_read = (char *)NULL;

/* Read a string, and return a pointer to it.  Returns NULL on EOF. */
char *
rl_gets(const char * prompt)
{
  /* If the buffer has already been allocated, return the memory
     to the free pool. */
  if (line_read)
  {
      free (line_read);
      line_read = (char *)NULL;
  }

  /* Get a line from the user. */
  line_read = readline (prompt);

  /* If the line has any text in it, save it on the history. */
  if (line_read && *line_read)
    add_history (line_read);

  return (line_read);
}

/*
int
consume_stdin(void)
{
    int  i = 0;
    char c = '\0';

    do
    {
        c = getchar();
        ++i;
    } while (c != '\n');

    return i;
}
*/

int
strip_newline(char * b)
{
    if (b == NULL)
    {
        return -1;
    }
  
    int len = strlen(b);
    if (b[len-1] == '\n')
    {
        b[len-1] = '\0';
    }
    /* It's ok for there to be no newline; not returning
       -1 for that case. */

    return 0;
}

// I can ever get strtol to work correctly. I don't know why.
long int
convert_str_to_long(const char *str)
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
        exit(EXIT_FAILURE);  // I can't even right now...just bail.
    }

    return res;
}

int
convert_long_to_str(long num, char * str)
{
    long t = -1;
    char c = 0;
    char
        * ptr = NULL,
        * tmp = NULL;

    if (num == 0)
    {
        *str++ = '0';
        *str   = '\0';
        goto done;     // Yeah that's right. I could make the entire
                       // rest of the function an else statement, but...
    }
    
    tmp = malloc(MAXBUF);  // Do I need to cast this?
    memset(tmp, '\0', MAXBUF);

    // This reverses the number. I can't figure out how to do it
    // first without reversing it.
    ptr = tmp;
    while (num != 0)
    {
        t = num % 10;
        c = t + '0';
        *ptr++ = c;

        num /= 10;
    }

    // So we reverse it back. Seems like a hack.
    ptr = tmp+(strlen(tmp)-1);  // -1 for null terminator
    for (size_t i = 0; i < strlen(tmp); ++i)
    {
        str[i] = *ptr--;
	// *str-- = *ptr--;
    }
    // This should be set due to memset above, but just to be sure
    str[strlen(tmp)] = '\0';
    
    free(tmp);

  done:
    return 0;
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
    }

    return result;
}

int
main(int argc, char **argv)
{
    /* char input[MAXBUF] = ""; */
    char *input = NULL;
    char *p = NULL;

    int
        need_to_calculate = 0,
        have_operator = 0,
        first_done = 0,
        first_op = 1;

    long
        res = 0,
        working_res = 0;
    char
        loperand = 0,
        roperand = 0;
    char
        *left_num_str = NULL,
        *left_char = NULL,
        *right_num_str = NULL,
        *right_char = NULL;

    enum operators operator = NUL;
    
    
    printf("Trey's Simple Calculator.\n");
    printf("Basic maths - addition, subtraction, multiplcation, division, modulo.\n");
    printf("Enter your equations, please. Empty line will exit.\n");
    
    // readline allocates buffer for us; rl_gets frees it on each call
    /* input = malloc(MAXBUF); */
    /* memset(input, '\0', MAXBUF); */
    
    // Setting it to null terminator just because it seems good to me
    left_num_str  = malloc(MAXBUF);
    memset(left_num_str, '\0', MAXBUF);
    left_char = left_num_str;

    right_num_str = malloc(MAXBUF);
    memset(right_num_str, '\0', MAXBUF);
    right_char = right_num_str;
    
    for ( ; ; )
    {
        /* read_stdin(input); */
        input = rl_gets("");

        // readline (rl_gets) removes the newline, so testing
	// against \0 instead of \n
        if (input[0] == '\0')
            goto done;

        // readline removes it; no need to call it
        /* strip_newline(input); */

        p = input;
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
                //            ++p;
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
        printf("%ld\n", working_res);

        first_done        = 0;
        first_op          = 1;
        have_operator     = 0;
        working_res       = 0;
        need_to_calculate = 0;
        memset(input, '\0', MAXBUF);
    }

  done:
    free(left_num_str);
    free(right_num_str);
    
    return 0;
}
