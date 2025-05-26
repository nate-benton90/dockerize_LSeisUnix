
SU = /files2/data5/zli/lf
B = /home/stgpzli/OS6bin/slt

ROOT= $(SU)/cwp
UROOT= $(SU)/slt
LDIR= $(ROOT)/lib
MDIR= $(UROOT)/lib
IDIR= $(ROOT)/include
KDIR= $(UROOT)/include
D =
 
CFLAGS= -xO3 -fast -v -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-I$(KDIR) -I$(IDIR)
FFLAGS= -O3 -fast -C -v

MSUBS= -L$(MDIR) -lgrid
LFLAGS= $(MSUBS) -L$(LDIR) -lpar -lcwp -lm
CC= cc
FC= $(FC) 

rayt2d: rayt2d.o raytime2d.o ray2d.o vel2d.o dv2.o ov2int.o
	$(FC) rayt2d.o raytime2d.o vel2d.o \
	ray2d.o dv2.o ov2int.o $(LFLAGS) -o $B/rayt2d
	@chmod 755 $B/rayt2d

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out
