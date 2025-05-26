
SU = /pad/data5/zli/lf
ROOT= $(SU)/cwp
UROOT= $(SU)/slt

L= $(ROOT)/lib
I= $(ROOT)/include

M= $(UROOT)/lib
K= $(UROOT)/include
B = /home/stgpzli/OS6bin/slt
 
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= -O3 -fast -I$K -I$I
MSUBS= -L$M -lusu -lgrid -lwgc -lsubf -lsubc
D = $L/libcwp.a $L/libpar.a $L/libsu.a $L/libxplot.a
E = $M/libsubc.a $M/libsubf.a $M/libgrid.a $M/libwgc.a $M/libusu.a

LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm -lF77

CC= cc
FC= $(FC) 

.PRECIOUS	= print

PROGS = 	\
	$B/intp3d

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

intp3d: intp3d.o taupsub.o vccvft.o
	$(FC) intp3d.o taupsub.o vccvft.o $(LFLAGS) $(MSUBS) -o $(B)/intp3d

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
