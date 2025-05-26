# Makefile for ...cwp/test
# Just say "make"
# NOTE: These guys are NOT put in /usr/local/bin!

SHELL = /bin/sh

TESTDIR = .

# Adapt the flags in the following paragraph to your system
ROOT = $(CWPROOT)
OPTC = -O

B = $(TESTDIR)
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lXcwp -lXm -lXt -lX11 -lcwp -lm
LINTFLAGS= -I$I

PROGS =			\
	$B/axes


INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$$(@F).c
	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake:
	@touch *.c
	@-rm -f $(PROGS)
	@$(MAKE) ROOT=$(ROOT) OPTC=$(OPTC)

clean	:
	rm -f axes junk* core a.out *.o
