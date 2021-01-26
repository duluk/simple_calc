CC = gcc
CFLAGS = -W -Wall -pedantic -Wformat-nonliteral -Wcast-align -Wpointer-arith -Wbad-function-cast -Wstrict-prototypes -Wmissing-declarations -Winline -Wundef -Wnested-externs -Wcast-qual -Wshadow -Wconversion -Wwrite-strings -Wno-conversion -ffloat-store
C99 = -std=c99
C89 = -std=c89
C11 = -std=c11

DEBUG = -ggdb
OPTZ = -O2

BACK = *~

# Because this is in OneDrive and I use mutliple OSs to work on it
ifeq ($(OS),Windows_NT) 
    detected_OS := Windows
else ifneq (, $(findstring Microsoft, $(shell uname -a 2>/dev/null)))
    HOME_DIR = /mnt/c/Users/jab3
else ifneq (, $(findstring microsoft, $(shell uname -a 2>/dev/null)))
    HOME_DIR = ../../../..
else ifneq (, $(findstring Darwin, $(shell uname -a 2>/dev/null)))
    HOME_DIR = ../../../..
else
    detected_OS := Linux
    HOME_DIR = /home/jab3
    OS_name := $(shell sh -c 'uname 2>/dev/null || echo Unknown')
endif

#ifeq ($(detected_OS),Windows)
#endif
#ifeq ($(detected_OS),Linux)
#endif

SRC_HOME = $(HOME_DIR)/OneDrive/Programming/mine

MYLIB_HOME = $(SRC_HOME)/mylib
MYLIB_OBJ = $(MYLIB_HOME)/mylib.o
MYLIB_INC = -I$(MYLIB_HOME) -L$(MYLIB_HOME)

MYLIB = -lmylib 
READLINE = -lreadline
MATH = -lm

STDLIBS = $(MYLIB) $(READLINE) 

COMMANDS = commands

STACK = calc_stack
STACK_TEST = test_calc_stack

SC = simple_calc
SC_SHORT = sc
SC_FIRST = simple_calc.first_method_final.c
SC_FIRST_SHORT = sc-first

RM = /bin/rm
MV = /bin/mv
CP = /bin/cp
AR = /usr/bin/ar

all: $(SC_SHORT)

#$(SC).o:
#	$(CC) $(CFLAGS) $(C99) $(MYLIB_INC) -c -o $(SC).o $(SC).c $(STDLIBS)

$(COMMANDS).o: $(COMMANDS).c $(SC).h
	$(CC) $(CFLAGS) $(C99) $(DEBUG) $(MYLIB_INC) -c -o $(COMMANDS).o $(COMMANDS).c $(MYLIB)

$(STACK).o: $(STACK).c $(STACK).h
	$(CC) $(CFLAGS) $(C99) $(DEBUG) -c -o $(STACK).o $(STACK).c

$(STACK_TEST): $(STACK_TEST).c $(STACK).o
	$(CC) $(CFLAGS) $(C99) $(DEBUG) -o $(STACK_TEST) $(STACK_TEST).c $(STACK).o

infix_test: infix_to_postfix.c $(STACK).o
	$(CC) $(CFLAGS) $(C99) $(DEBUG) -o infix_test infix_to_postfix.c $(STACK).o

$(SC_SHORT): $(SC).c $(SC).h $(STACK).o $(COMMANDS).o
	$(CC) $(CFLAGS) $(C99) $(DEBUG) $(MYLIB_INC) -o $(SC_SHORT) $(SC).c $(STACK).o $(COMMANDS).o $(STDLIBS) $(MATH)

$(SC_FIRST_SHORT): $(SC_FIRST)
	$(CC) $(CFLAGS) $(C99) $(DEBUG) $(MYLIB_INC) -o $(SC_FIRST_SHORT) $(SC_FIRST) $(STDLIBS) $(MATH)

clean:
	$(RM) -f a.out $(SC_SHORT) $(SC).o $(SC_FIRST_SHORT) $(STACK_TEST) $(STACK).o $(COMMANDS).o $(BACK) core*
