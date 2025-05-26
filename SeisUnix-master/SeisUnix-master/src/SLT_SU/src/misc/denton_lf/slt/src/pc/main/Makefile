
SU = /pad/data5/zli/lf
ROOT= $(SU)/cwp
UROOT= $(SU)/slt
B = /home/stgpzli/OS6bin/slt

OPTC = -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

L= $(ROOT)/lib
I= $(ROOT)/include 

M= $(UROOT)/lib
K= $(UROOT)/include
 
CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= -O3 -I$K -I$I
MSUBS= -L$M -lusu -lvelo -lgrid -lsubf -lsubc
###D = $L/libcwp.a $L/libpar.a $L/libsu.a $L/libxplot.a
D = $L/libcwp.a $L/libpar.a $L/libsu.a
KD = $M/libsubc.a $M/libsubf.a $M/libgrid.a

###LFLAGS= $(MSUBS) -L$L -lxplot -lsu -lpar -lcwp -lm -lF77
LFLAGS= $(MSUBS) -L$L -lgrid -lsu -lpar -lcwp -lm

CC= cc
FC= $(FC) 

.PRECIOUS	= print

SRCS = 	\
	bswap.c

PROGS = $(SRCS:.c=)

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

$(PROGS): $D $(KD) $(SRCS)
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $B/$@
	@chmod 755 $B/$@
	@echo $(@F) installed in $B

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
