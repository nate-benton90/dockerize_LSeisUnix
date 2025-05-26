
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
MSUBS= -L$M -lvelo -lusu -lwgc -lgrid -lsubf -lsubc
#D = $L/libcwp.a $L/libpar.a $L/libsu.a $L/libxplot.a
D = $L/libcwp.a $L/libpar.a $L/libsu.a
E = $M/libsubc.a $M/libsubf.a $M/libgrid.a \
	$M/libwgc.a $M/libusu.a $M/libvelo.a

LFLAGS= $(MSUBS) -L$L -lsu -lpar -lcwp -lm -lF77 -lsunmath

CC= cc
FC= $(FC) 

.PRECIOUS	= print

PROGS = 	\
	$B/aresam	\
	$B/ascii2hvel	\
	$B/hvel2vs3d	\
	$B/hveladdxy	\
	$B/hvelconvert	\
	$B/hvelmod	\
	$B/hvelwind	\
	$B/lmkassign	\
	$B/lmktzconv	\
	$B/qcvs3d	\
	$B/vgrid3d	\
	$B/vgupdate	\
	$B/vmerge	\
	$B/traceline2xy	\
	$B/vt2vz	\
	$B/vz2vt	\
	$B/vrms2vint \
	$B/vint2vavg \
	$B/vgrid2hvel \
	$B/vs3dedit \
	$B/vs3dfilter \
	$B/vs3dshift \
	$B/vsmoo3d \
	$B/xy2traceline

INSTALL :$(PROGS)
	touch $@

remake  :
	-touch *.c
	make

$(PROGS): $D $(E) $$(@F).c
	-$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out core
