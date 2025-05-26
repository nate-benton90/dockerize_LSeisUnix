
SU = /pad/data5/zli/lf
B= /home/stgpzli/TESTbin/slt

ROOT= $(SU)/cwp
UROOT= $(SU)/slt
LDIR= $(ROOT)/lib
MDIR= $(UROOT)/lib
IDIR= $(ROOT)/include 
KDIR= $(UROOT)/include
D =
###TTCIDIR = /app/geoenergy3d-beta3/Examples
TTCIDIR = /home/stgpzli/test/ttc/traveltime_compression_SUN_dist/Examples
TTCDIR = /home/stgpzli/test/ttc/traveltime_compression_SUN_dist/Libs
TTCLIB = -lttc2 -lmwt  -lnsl -ldl
###TTCDIR = /app/geoenergy3d-beta3/libs
 
CFLAGS= -xO3 -fast -v -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-I$(KDIR) -I$(IDIR) -I$(TTCIDIR)
FFLAGS= -O3 -fast -C -v
####FFLAGS= -O3 -fast -stackvar -C -v
FFLAGP= -O3 -fast -v -loopinfo -explicitpar -vpara -C
####FFLAGP= -O3 -fast -v

LFLAGS= -O3 -fast -v -explicitpar -L$(MDIR) -lgrid -lsubc -lsubf \
	-L$(TTCDIR) $(TTCLIB) \
	-L$(LDIR) -lpar -lcwp -lm
##LFLAGS= -O3 -fast -v -L$(MDIR) -lgrid -lsubc -lsubf \
##	-L$(LDIR) -lpar -lcwp -lm
CC= cc
FC= $(FC) 

rt3dp.o: 
	$(FC) -c $(FFLAGP) rt3dp.f

raytime.o:
	$(FC) -c $(FFLAGS) -stackvar raytime.f

ray3d.o:
	$(FC) -c $(FFLAGS) -stackvar ray3d.f

rayt3d: rayt3d.o rt3dp.o raytime.o ray3d.o vel3d.o dv2.o ov2int.o trans.o 
	$(FC) rayt3d.o rt3dp.o raytime.o vel3d.o \
	ray3d.o dv2.o ov2int.o trans.o $(LFLAGS) -o $B/rayt3d
	@chmod 755 $B/rayt3d

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out
