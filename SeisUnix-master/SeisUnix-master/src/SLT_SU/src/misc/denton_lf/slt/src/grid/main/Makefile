
ROOT= /pad/data5/zli/lf/cwp
UROOT= /pad/data5/zli/lf/slt
B = /home/stgpzli/OS6bin/slt

OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64

L= $(ROOT)/lib
I= $(ROOT)/include

M= $(UROOT)/lib
K= $(UROOT)/include
 
CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= -O3 -fast -I$K -I$I
MSUBS= -L$M -lusu -lvelo -lgrid -lsubf -lsubc
###D = $L/libcwp.a $L/libpar.a $L/libsu.a $L/libxplot.a
D = $L/libcwp.a $L/libpar.a $L/libsu.a
KD = $M/libsubc.a $M/libsubf.a $M/libgrid.a

###LFLAGS= $(MSUBS) -L$L -lxplot -lsu -lpar -lcwp -lm -lF77
LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm -lF77

CC= cc
FC= $(FC) 

.PRECIOUS	= print

PROGS = 	\
	$B/grid2horz	\
	$B/grid2lmk	\
	$B/gridatlayer	\
	$B/gridclip	\
	$B/griddeform	\
	$B/gridheader	\
	$B/gridfilter	\
	$B/gridmerge	\
	$B/gridop2	\
	$B/gridrotate	\
	$B/gridslice	\
	$B/gridshift	\
	$B/gridtrsp	\
	$B/gridwind	\
	$B/gridsalt	\
	$B/gridsc3d	\
	$B/gridsediment	\
	$B/gridstack	\
	$B/gridsubs	\
	$B/gridthickness	\
	$B/lmk2grid \
	$B/lmk2grid.new \
	$B/lmkwind \
	$B/regrid3d

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

$(PROGS): $D $(KD) $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

###lmk2grid: $D $(KD) lmk2grid.o
###	$(FC) $(CFLAGS) lmk2grid.o $(LFLAGS) -o $B/lmk2grid

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
