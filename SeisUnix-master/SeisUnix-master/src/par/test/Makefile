# Makefile for ...par/test
# Just say "make"
# NOTE: These guys are NOT put in /usr/local/bin!

SHELL = /bin/sh

TESTDIR = .

# Adapt the flags in the following paragraph to your system
ROOT = $(CWPROOT)
OPTC = -O
TARGETFLAG = $$(@F)

B = $(TESTDIR)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lcwp -lpar -lm
LINTFLAGS= -I$I

PROGS =			\
	$B/erealloct


INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$(TARGETFLAG).c
	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake:
	@$(MAKE) ROOT=$(ROOT)

clean	:
	rm -f junk* core a.out *.o
