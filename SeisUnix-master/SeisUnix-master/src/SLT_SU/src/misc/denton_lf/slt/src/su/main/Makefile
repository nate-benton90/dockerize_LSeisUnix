
ROOT= /pad/data5/zli/lf/cwp
UROOT= /pad/data5/zli/lf/slt
B = /home/stgpzli/OS6bin/slt

L= $(ROOT)/lib
I= $(ROOT)/include

M= $(UROOT)/lib
K= $(UROOT)/include
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
 
CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= -O3 -fast -I$K -I$I
MSUBS= -L$M -lusu -lvelo -lgrid -lwgc -lsubf -lsubc
D = $L/libcwp.a $L/libpar.a $L/libsu.a
E = $M/libsubc.a $M/libsubf.a $M/libgrid.a $M/libwgc.a $M/libusu.a

LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm -lF77
###LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp
##LXFLAGS= $(MSUBS) -L$L -lsu -lxplot -lX11 -lpar -lcwp
LXFLAGS= $(MSUBS) -L$L -lsu -lxplot -lX11 -lpar -lcwp -lm -lF77

CC= cc
FC= $(FC) 

.PRECIOUS	= print

PROGS = 	\
	$B/RNmo \
	$B/segy2movie \
	$B/sgyin \
	$B/sgyout \
	$B/su2sl \
	$B/suaddxy \
	$B/suareazero \
	$B/su3dto2d \
	$B/subalance \
	$B/sufill \
	$B/sufold \
	$B/sugrd2hdr \
	$B/sugrid2hdr \
	$B/sumerge \
	$B/suphase \
	$B/surenum \
	$B/surunmix \
	$B/supstack \
	$B/suscale3d	\
	$B/sushift	\
	$B/susplit	\
	$B/mutesu	\
	$B/mutepick	\
	$B/sutosmute	\
	$B/sutzconv	\
	$B/suxyfix

XPGM = 	\
	$B/sudetect	\
	$B/mipick	\
	$B/mwpick

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

$(PROGS): $D $(E) $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

$(XPGM): $D $(E) $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LXFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

suvautop: $D $(E) suvautop.c
	$(CC) $(CFLAGS) suvautop.c $(LXFLAGS) -o $B/suvautopick

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
