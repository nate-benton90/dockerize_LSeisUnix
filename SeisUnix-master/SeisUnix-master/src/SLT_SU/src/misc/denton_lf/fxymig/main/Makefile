
SU = /pad/data5/zli/lf
###B = /home/stgpzli/TESTbin/slt
B = /home/stgpzli/OS6bin/slt

ROOT= $(SU)/cwp
UROOT= $(SU)/slt
IDIR= $(ROOT)/include
KDIR= $(UROOT)/include
LDIR= $(ROOT)/lib
MDIR= $(UROOT)/lib

CFLAGS= -xO3 -fast -v -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 \
	-I$(KDIR) -I$(IDIR)
FFLAGS= -O3 -fast -v
FFLAGP= -O3 -fast -v -loopinfo -explicitpar -vpara 
LFLAGS= -O3 -fast -v -explicitpar -L$(MDIR) -lusu -lgrid -lsubc -lsubf \
	-L$(LDIR) -lsu -lpar -lcwp -lm 
CC= $(CC)
FC= $(FC)

.c.o:
	$(CC) -c $(CFLAGS) $*.c

errcom.o:	errcom.f
	$(FC) -c -stackvar $(FFLAGS) errcom.f

cfft2d.o:	cfft2d.f
	$(FC) -c $(FFLAGS) cfft2d.f

cfft.o:		cfft.f
	$(FC) -c $(FFLAGS) cfft.f

vwmig.o:	vwmig.f
	$(FC) -c -stackvar $(FFLAGS) vwmig.f

v2tris.o:	v2tris.f
	$(FC) -c $(FFLAGS) v2tris.f

fxysub.o:	fxysub.f
	$(FC) -c $(FFLAGP) fxysub.f

fxymig:	fxymig.o fxysub.o errcom.o vwmig.o v2tris.o cfft2d.o cfft.o
	$(FC) fxymig.o fxysub.o errcom.o vwmig.o v2tris.o \
	cfft2d.o cfft.o	\
	$(LFLAGS) -o $(B)/fxymig

fxymcal: fxymcal.o
	$(FC) fxymcal.o $(LFLAGS) -o $(B)/fxymcal

fxymerge: fxymerge.o
	$(FC) fxymerge.o $(LFLAGS) -o $(B)/fxymerge

fxymerge.test: fxymerge.o
	$(FC) fxymerge.o $(LFLAGS) -o $(B)/fxymerge.test

ffm: ffm.o
	$(FC) ffm.o $(LFLAGS) -o $(B)/ffm


clean:
	rm -f *.o junk* a.out
