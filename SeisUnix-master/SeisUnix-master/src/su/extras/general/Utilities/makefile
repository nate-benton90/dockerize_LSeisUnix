# Makefile for ...gen/Utilities
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local
OPTC = -O
# Search on the word "Comment" to complete adaptation

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L
LINTFLAGS= -I$I


# Comment out if your make knows about C libs
#.c.a:
#	@$(CC) -c $(CFLAGS) $<
#	@ar rv $@ $*.o
#	@rm -f $*.o

# Comment out unless your ar needs ranlib and you don't have it
#.c.a:
#	@$(CC) -c $(CFLAGS) $<
#	@lorder $*.o | tsort | xargs ar qv $@
#	@rm -f $*.o
# Note: if you don't have xargs, alternate lorder line is:
#	@ar rv $@ `lorder $*.o | tsort`

IDLE = terminal datatapes renice tell att2bsd cwpcal fiscal \
	showget send findit tapefit makedir givemail

SHELLS =		\
	$B/411		\
	$B/add411	\
	$B/argv		\
	$B/bigfiles	\
	$B/bigjobs	\
	$B/cpall	\
	$B/dirtree	\
	$B/hangup	\
	$B/locks	\
	$B/motd		\
	$B/news		\
	$B/overwrite	\
	$B/precedence	\
	$B/replace	\
	$B/rcswho	\
	$B/type		\
	$B/unlock	\
	$B/usernames	\
	$B/varlist

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

LINKS = $B/rcpall

LIB = $L/libutil.a
ARCH = $(LIB)(iof.o)


INSTALL:	$(SHELLS) $(PROGS) $(ARCH) $(LINKS)
	@touch $@


$(SHELLS):	$$(@F).sh
	@cp $? $@
	@chmod 755 $@
	@echo $(@F) installed in $B

$(PROGS):	$$(@F).c
	@$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

$(LINKS):	$B/cpall
	-@rm -f $B/rcpall
	@ln $B/cpall $B/rcpall
	@echo $B/rcpall installed in $B

$(LIB)	:	$(ARCH)
# Comment off following line if you don't need or don't have ranlib
#	ranlib $(LIB)

list	:
	ar tv $(LIB)

remake:
#	@touch *.c *.sh
	@rm -f $(LIB)
	@make

clean	:
	rm -f junk* core a.out *.o
