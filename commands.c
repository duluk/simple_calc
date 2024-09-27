#include <stdio.h>
#include <stdlib.h>  // For malloc/free
#include <string.h>  // For strlen, strcmp, substr, etc
#include <ctype.h>   // for isalpha/isdigit, toupper

#include <mylib.h>
#include "simple_calc.h"

// Sort of weird 'efficiency' strategy is to not walk through the
// whole command list every command, but just the commands that begin
// with same letter. Not really big deal as won't have that many
// commands. Just trying the technique out. Other ways, like binary
// search or some tree or something. Whatever. IANASE.
Commands
command_found(char * cmd)
{
    Commands ret = NONE;

    // Read comment for function to know why switching on the first letter
    // of the command and not using something like GNU's strcasestr.
    switch(*cmd)
    {
        case 'l':
            // strwstr is strstr but word-bound; in -lmylib
            if ((strwstr(lower(cmd), "let")) != NULL)
            {
                ret = LET;
            }
            break;
        case 's':
            if ((strwstr(lower(cmd), "show")) != NULL)
            {
                ret = SHOW;
            }
            break;
    }

    return ret;
}

int
validate_variable(const char * v)
{
    const char * p = v;

    while (*p != '\0')
        // Not allowing numbers in variable names
        if (!isalpha(*p++))
            return -1;

    return 0;
}

int
validate_value(const char * v)
{
    const char * p = v;

    while (*p != '\0')
        // Values should be digits only (for now)
        if (!isdigit(*p++))
            return -1;

    return 0;
}

// "hash-ish" function
int
hash_var(char v)
{
    int c = toupper(v);

    return c - 'A';
}

int
lookup_var(char x, calc_t * vars, calc_t * n)
{
    // What is the best data structure for the variables? Array, list,
    // hash? What algorithm is best for this lookup?

    // How do I traverse the list? How do I know how many elements if
    // it is an array? Would I need to create my own iterator function?

    // This isn't a calc_t** because I'm not assigning to the n variable;
    // I'm assigning to what n points to. That works.
    *n = vars[hash_var(x)];

    return 0;
}

// Sort of brute-forcey...
Variable *
parse_let(char * input)
{
    int    r = 0;
    char * s = NULL;
    char * f = NULL;
    Variable * var = NULL;

    /* // Don't want to modify input */
    /* s = malloc(strlen(input) * sizeof(char)); */
    /* strncpy(s, input, strlen(input)); */
    s = input;

    // We can throw this away; already have the cmd
    f = strtok(s, " ");
    if ((r = strncmp("let", f, 3)) != 0)
        return NULL;

    // Oh to have access to a script's regex parsing...
    // regex.compile('let ([A-Za-z]+) = ([0-9]+)')
    if (f)
    {
        var = malloc(sizeof(Variable));

        // Variable
        f = strtok(NULL, " ");

        // I think this will go bad though. Particularly in my actual
        // program. This one may work because it is using the buf
        // variable below. May do something input, which I suppose
        // still exists in the calling function so is there when
        // returned.
        if ((validate_variable(f)) < 0)
            return NULL;
        var->name = *f;

        // This should be the equal sign
        f = strtok(NULL, " ");
        if ((r = strncmp("=", f, 1)) != 0)
            return NULL;

        // Value
        f = strtok(NULL, " ");
        if ((validate_value(f)) < 0)
            return NULL;
        calc_t val  = 0;
        val = atoll(f);
        var->value = val;
    }

    return var;
}

return_code
parse_command(char * input_str, Variable ** var, Commands cmd)
{
    return_code r = FAILED;

    switch(cmd)
    {
        case LET:
        {
            // parse_let malloc's the return value, given to let_v
            Variable * let_v = parse_let(input_str);
            if (let_v != NULL)
            {
                /* var = memcpy(var, let_v, sizeof(Variable)); // or sizeof(let_v)? */
                *var = let_v;
                r = SUCCESS;
            }
            break;
        }
        case SHOW:
            // Not implemented
            break;
        case NONE:
            // Silence compiler
            break;
    }

    return r;
}

const char *
enum_cmd_str(Commands * c)
{
    switch(*c)
    {
        case LET:
            // strncpy(command_str, "let", 4); // 4 for \0?
            return "let";
            break;
        case SHOW:
            // strncpy(command_str, "show", 5); 
            return "show";
            break;
        case NONE:
            // Silence compiler...
            break;
    }

    return NULL;
}
