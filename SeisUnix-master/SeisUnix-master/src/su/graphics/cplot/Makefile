# Makefile for ...su/graphics/cplot
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local/cwp
OPTC = -g
#OPTC = -O

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lcplot -lsu -lpar -lcwp -lm


.PRECIOUS	= print

XP  =	$B/sucwig $B/suctwig $B/sucpwig \
	$B/sucwignmax $B/sucqgraph $B/sucgraph $B/sucmaxplot

INSTALL	: $(XP)
	touch $@

remake	:
	-touch *.c
	make ROOT=$(ROOT) OPTC=$(OPTC)
	

$(XP)	:$$(@F).c
	-$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B


clean:
	rm -f a.out junk* JUNK* core

#list:
#	@ls *.c | pr -5 -t -l22 | p 
#
#printall:
#	@for i in *.c			;\
#	do				 \
#		croff $$i		;\
#	done
#
#print:	*.c
#	@for i in $?			;\
#	do				 \
#		echo "printing $$i ..."	;\
#		croff $$i		;\
#	done
#	touch $@
