
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
MSUBS= -L$M -lusu -lgrid -lsubf -lsubc
D = $L/libcwp.a $L/libpar.a $L/libsu.a
E = $M/libsubc.a $M/libsubf.a $M/libgrid.a $M/libusu.a

LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm

CC= cc
FC= $(FC) 

.PRECIOUS	= print

vsmoo3d: $D $(E) vsmoo3d.c vsm3d.o vsmsub.o wavel.o 
	$(CC) $(CFLAGS) vsmoo3d.c vsm3d.o vsmsub.o wavel.o \
	$(LFLAGS) -o $(B)/vsmoo3d
	@chmod 755 $(B)/vsmoo3d
	@echo vsmoo3d installed in $B
clean:
	/bin/rm -f *.o junk* a.out core

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

