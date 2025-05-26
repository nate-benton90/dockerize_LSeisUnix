# Makefile for ...xplot/main
# Just say "make"
# xpwin is NOT automatically compiled (portability)

SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /pad/data5/zli/lf
ROOT = $(SU)/cwp
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

B = /home/stgpzli/OS6bin/cwp
I = $(ROOT)/include
L = $(ROOT)/lib
#D = $I/cwp.h $I/par.h $I/xplot.h $L/libcwp.a $L/libpar.a $L/libxplot.a
D = $L/libcwp.a $L/libpar.a $L/libxplot.a
CC = $(CC)
ENDIANFLAG = -DCWP_BIG_ENDIAN

CFLAGS= $(OPTC) -I$I $(ENDIANFLAG)
LFLAGS= -L$L -lxplot -lX11 -lpar -lcwp -lm

PROGS =			\
	$B/lcmap	\
	$B/lprop	\
	$B/scmap	\
	$B/xcontour	\
	$B/ximage	\
	$B/xpicker	\
	$B/xwigb

INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$D $$(@F).c
	$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

# this one needs Display PostScript
$B/xpwin:	$D xpwin.c
	$(CC) $(CFLAGS) xpwin.c $(LFLAGS) -ldps -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B
xpwin:	xpwin.c
	$(CC) $(CFLAGS) xpwin.c $(LFLAGS) -ldps -o xpwin
	
# Some makes don't understand the $$ notation above.  In this case
# you have to type out the compile paragraph for each PROG.  Sigh.
# Here's a start, good luck.
#
#$B/lcmap:	lcmap.c
#	$(CC) $(CFLAGS) lcmap.c $(LFLAGS) -o $@
#	@chmod 755 $@
#	@echo $(@F) installed in $B

remake:
	@touch *.c
	@make ROOT=$(ROOT) OPTC=$(OPTC)

clean	:
	rm -f junk* core a.out *.o
