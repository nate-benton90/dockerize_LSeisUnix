SHELL = /bin/sh

TESTDIR = .

# Adapt the flags in the following paragraph to your system
ROOT = $(CWPROOT)
OPTC = -O

B = $(TESTDIR)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lcwp -lpar -lm
LINTFLAGS= -I$I

PROGS =	


#INSTALL:	$(PROGS)
#	@touch $@

#$(PROGS):	$$(@F).c
#	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
#	@$(MCHMODLINE)
#	@echo $(@F) installed in $B

#remake:
#	@touch *.c
#	@make

clean	:
	rm -f junk* core a.out *.o *000
