# Creating arrays in heap storage
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local
OPTC = -O
# Search on the word "Comment" to complete adaptation

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

#CFLAGS= $(OPTC) -I$I
CFLAGS= -g
LFLAGS= -L$L -lsu
LINTFLAGS= -I$I


# Functions with built in -DTEST drivers
F = dim1 dim2 dim3

# Array macro header file
A = dim.h

# Test drivers for array macros
M = mdim1 mdim2 mdim3 mdimen2 mdimen3 mdet2


compile	: $F $M
	@touch $@

$F	: $$@.c
	cc $(CFLAGS) -DTEST $@.c $(LFLAGS) -o $@

$M	: $$@.c $A
	cc $(CFLAGS) $@.c $(LFLAGS) -o $@

clean:
	a.out core dim? mdim? mdet2
