# Makefile for ...su/main
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /pad/data5/zli/lf
B = /home/stgpzli/OS6bin/cwp

ROOT = $(SU)/cwp
UROOT = $(SU)/slt
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

I = $(ROOT)/include 
K = $(UROOT)/include
L = $(ROOT)/lib
M = $(UROOT)/lib
D = $L/libcwp.a $L/libpar.a $L/libsu.a $M/libwgc.a $M/libsubf.a $M/libsubc.a
CC = $(CC)

CFLAGS= $(OPTC) -I$K -I$I
LFLAGS= -L$M -lwgc -lsubf -lsubc -L$L -lsu -lpar -lcwp -lm

PROGS =			\
	$B/suabshw    \
	$B/suaddhead    \
	$B/suband       \
	$B/sucbhw        \
	$B/suchart        \
	$B/suchw        \
	$B/suedit	\
	$B/sugain	\
	$B/sugetcbhd	\
	$B/sugethw	\
	$B/sumax       \
	$B/suop2       \
	$B/suresamp       \
	$B/sushw        \
	$B/susort       \
	$B/sustack       \
	$B/sustrip       \
	$B/sutvband	\
	$B/suus        \
	$B/suwind

INSTALL	:$(PROGS)
	touch $@


$(PROGS): $D $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

sgyin.test: $D sgyin.c
	$(CC) $(CFLAGS) sgyin.c $(LFLAGS) -o /home/stgpzli/TESTbin/slt/sgyin

# Some makes don't understand the $$ notation above.  In this case
# you have to type out the compile paragraph for each PROG.  Sigh.
# Here's a start, good luck.
#

remake	:
	-touch *.c
	make ROOT=$(ROOT) OPTC=$(OPTC)
	
clean:
	rm -f a.out junk* JUNK* core
