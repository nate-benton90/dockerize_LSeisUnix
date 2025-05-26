
ROOT= /home/stgpzli/cwp
UROOT= /home/stgpzli/slt
B = /home/stgpzli/slt/bin

L= $(ROOT)/lib
I= $(ROOT)/include
A= /app/seispact-3.4/SunOS5.6_sun4/include
ALIB= /app/seispact-3.4/SunOS5.6_sun4/lib/seilib.a

M= $(UROOT)/lib
K= $(UROOT)/include
 
CFLAGS= -xO3 -fast -DAWARE -I$A -I$K -I$I -I./

FFLAGS= -O3 -fast -I$K -I$I
MSUBS= -L$M -lgrid -lsubf -lsubc
D = $L/libcwp.a $L/libpar.a $L/libsu.a
KD = $M/libsubc.a $M/libsubf.a $M/libgrid.a

LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm -lF77

CC= cc
FC= $(FC) 

.PRECIOUS	= print

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

$(PROGS): $D $(KD) $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

sucompress: sucompress.c
	$(CC) $(CFLAGS) sucompress.c $(ALIB) $(LFLAGS) -o $B/sucompress

sudecomp: sudecomp.c
	$(CC) $(CFLAGS) sudecomp.c $(ALIB) $(LFLAGS) -o $B/sudecomp

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
