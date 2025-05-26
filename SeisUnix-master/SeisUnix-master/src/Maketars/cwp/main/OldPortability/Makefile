# Makefile for ...cwp/main
# Just say "make"

SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local
OPTC = -O
# Search on the word "Comment" to complete adaptation

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib
D = $I/cwp.h $L/libcwp.a

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lcwp

PROGS =			\
	$B/ctrlstrip	\
	$B/fcat		\
	$B/isatty	\
	$B/maxints	\
	$B/newback	\
	$B/pause	\
	$B/pgen		\
	$B/rec		\
	$B/t


INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$$(@F).c
	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake:
	@touch *.c
	@make

clean	:
	rm -f junk* core a.out *.o
