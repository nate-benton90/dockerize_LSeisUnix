
SU = /pad/data5/zli/lf
B = /home/stgpzli/OS6bin/slt

ROOT= $(SU)/cwp
UROOT= $(SU)/slt
LDIR= $(ROOT)/lib
MDIR= $(UROOT)/lib
IDIR= $(ROOT)/include
KDIR= $(UROOT)/include

CFLAGS= -xO3 -fast -v -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-I$(KDIR) -I$(IDIR)
FFLAGS= -O3 -fast -v 
FFLAGP= -O3 -fast -v -loopinfo -explicitpar -vpara
###FFLAGP= -O3 -fast -v 

MSUBS= -L$(MDIR) -lusu -lgrid -lsubf -lsubc
##LFLAGF= -O3 -fast -v -L$(MDIR) -lusu -lgrid -lsubc -lsubf \
##	-L$(LDIR) -lsu -lpar -lcwp -lm
LFLAGF= -O3 -fast -v -explicitpar -L$(MDIR) -lusu -lgrid -lsubc -lsubf \
	-L$(LDIR) -lsu -lpar -lcwp -lm
LFLAGC= -xO3 -fast -v -L$(MDIR) -lusu -lgrid -lsubc -lsubf \
	-L$(LDIR) -lsu -lpar -lcwp -lm 

CC= cc 
FC= $(FC) 

kzm.o: kzm.f 
	$(FC) -c $(FFLAGP) kzm.f -o kzm.o

kzmig: kzmig.o kzm.o migsub.o 
	$(FC) kzmig.o kzm.o migsub.o $(LFLAGF) -o $B/kzmig

kzplanner: kzplanner.o
	$(FC) kzplanner.o $(LFLAGF) -o $B/kzplanner

kmmerge: kmmerge.o 
	$(FC) kmmerge.o $(LFLAGF) -o $B/kmmerge
	@chmod 755 $B/kmmerge

.c.o:
	$(CC) -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f

clean:
	/bin/rm -f *.o junk* a.out
