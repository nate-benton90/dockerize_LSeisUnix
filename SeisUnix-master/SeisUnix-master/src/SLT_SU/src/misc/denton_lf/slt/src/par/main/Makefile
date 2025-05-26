SU = /pad/data5/zli/lf
B= /home/stgpzli/OS6bin/slt

ROOT = $(SU)/cwp
UROOT = $(SU)/slt
LDIR= $(ROOT)/lib
IDIR= $(ROOT)/include
MDIR= $(UROOT)/lib
UDIR= /usr/lib
 
CFLAGS= -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -I$(IDIR)
FFLAGS= -O3
LFLAGS= -L$(LDIR) -lpar -lcwp -lm
MSUBS= -L$(MDIR) -lsubf -lsubc
FL= -L$(UDIR) -lF77 -lI77 -lU77

.c.o:
	cc -c $(CFLAGS) $*.c

.f.o:
	$(FC) -c $(FFLAGS) $*.f


window:	window.c
	cc $(CFLAGS) window.c $(LFLAGS) -o window
	mv window $(B)/window  

jobmonitor:	jobmonitor.c
	cc $(CFLAGS) jobmonitor.c $(LFLAGS) -o jobmonitor
	mv jobmonitor $(B)/jobmonitor  

maxmin:	maxmin.c
	cc $(CFLAGS) maxmin.c $(LFLAGS) -o maxmin
	mv maxmin $(B)/maxmin

clean:
	rm -f *.o junk* a.out
