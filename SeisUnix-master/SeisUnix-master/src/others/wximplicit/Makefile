# to install w-x 3-d implicit f.d. modeling and migration programs
# type 'make wxmd3d' and 'make wxmg3d', respectively, or 'make all'.
#
LDIR= /usr/local/cwp/lib
IDIR= /usr/local/cwp/include
MDIR= /usr/local/cwp/src/seismic/wximplicit/lib
 
CFLAGS= -O -qlanglvl=ansi -I$(IDIR)
FFLAGS= -O
LFLAGS= -L$(LDIR) -lpar -lcwp -lxlf -lm
MSUBS= -L$(MDIR) -lwxsub 

.c.o:
	cc -c $(CFLAGS) $*.c

.f.o:
	xlf -c $(FFLAGS) $*.f

wxmg3d:	wxmg3d.o
	cc wxmg3d.o $(LFLAGS) $(MSUBS) -o wxmg3d
	mv wxmg3d /usr/local/cwp/bin/wxmg3d

wxmd3d:	wxmd3d.o 
	cc wxmd3d.o $(LFLAGS) $(MSUBS) -o wxmd3d
	mv wxmd3d /usr/local/cwp/bin/wxmd3d

all:	wxmd3d wxmg3d clean

clean:
	rm -f *.o junk* a.out
