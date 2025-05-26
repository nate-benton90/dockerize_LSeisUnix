
SU = /files2/data5/zli/lf
B = /home/stgpzli/TESTbin/slt

ROOT= $(SU)/cwp
UROOT= $(SU)/slt
LDIR= $(ROOT)/lib
MDIR= $(UROOT)/lib
IDIR= $(ROOT)/include
KDIR= $(UROOT)/include
D =

CFLAGS= -xO3 -fast -v -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64  \
	-I$(KDIR) -I$(IDIR)
FFLAGS= -O3 -fast -v -I$(KDIR) -I$(IDIR)
FFLAGP= -O3 -fast -v -loopinfo -explicitpar -vpara

MSUBS= -L$(MDIR) -lusu -lgrid -lsubf -lsubc
LFLAGS= -O3 -fast -v -explicitpar $(MSUBS) -L$(LDIR) -lsu -lpar -lcwp -lm
CC= cc
FC= $(FC)

ktm.o: ktm.f
	$(FC) -c $(FFLAGP) ktm.f -o ktm.o

ktmaniso.o: ktmaniso.f
	$(FC) -c $(FFLAGP) ktmaniso.f -o ktmaniso.o

ktmig: ktmig.o ktm.o vm.o f2p5.o fhicut.o
	$(FC) ktmig.o ktm.o vm.o f2p5.o fhicut.o $(LFLAGS) -o $B/ktmig

ktmigeta: ktmig.o ktm.o ktmaniso.o vm.o f2p5.o fhicut.o
	$(FC) ktmig.o ktm.o ktmaniso.o vm.o f2p5.o fhicut.o $(LFLAGS) -o $B/ktmigeta

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out
