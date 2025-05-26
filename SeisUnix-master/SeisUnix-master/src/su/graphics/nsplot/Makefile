# Makefile for ...su/graphics/x
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local/cwp
OPTC = -O

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib
D = $L/libcwp.a $L/libpar.a $L/libsu.a

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lsu -lpar -lcwp -lm


.PRECIOUS	= print

PROGS  =	$B/sunsimage

INSTALL	: $(PROGS)
	touch $@


$(PROGS)	: $D $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake	:
	@-touch *.c
	@-rm -f $(PROGS)
	@$(MAKE) ROOT=$(ROOT) OPTC=$(OPTC)
	

clean:
	rm -f a.out junk* JUNK* core
