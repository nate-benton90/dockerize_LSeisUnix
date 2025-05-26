h52041
s 00006/00006/00034
d D 1.13 88/11/15 14:04:15 shuki 14 13
c 
e
s 00018/00087/00022
d D 1.12 88/09/04 08:36:21 valer 13 12
c 
e
s 00003/00002/00106
d D 1.11 88/06/19 13:13:33 shemer 12 10
c 
e
s 00003/00002/00106
d R 1.11 88/06/19 13:04:05 shemer 11 10
c 
e
s 00004/00001/00104
d D 1.10 88/06/15 14:32:35 shemer 10 9
c 
e
s 00014/00001/00091
d D 1.9 88/06/13 14:20:05 valer 9 8
c filrec,polcheb,precheb,recfil adding. VALERY
e
s 00014/00001/00078
d D 1.8 88/05/29 09:56:54 moshe 8 7
c 
e
s 00004/00001/00075
d D 1.7 88/05/18 11:28:23 valer 7 6
c winhan has been added
e
s 00005/00005/00071
d D 1.6 88/05/17 10:50:40 moshe 6 5
c 
e
s 00004/00001/00072
d D 1.5 88/05/15 11:47:56 moshe 5 4
c 
e
s 00011/00004/00062
d D 1.4 88/05/08 17:27:00 valer 4 3
c 
e
s 00004/00001/00062
d D 1.3 88/05/05 17:48:53 valer 3 2
c croscor.f adding
e
s 00039/00020/00024
d D 1.2 88/04/27 10:00:39 shuki 2 1
c Hetergenuous
e
s 00044/00000/00000
d D 1.1 88/04/14 13:50:17 shuki 1 0
c date and time created 88/04/14 13:50:17 by shuki
e
u
U
f e 0
t
T
I 1
#
# makefile for the su/lvec directory
#
D 2

CFLAGS = -g
FFLAGS = -g

E 2
D 13
#L     =	./libvec.a
E 13
D 14
LDIR	= /usr/local/lib/
I 2
D 13
D = /usr/local/obj/
E 13
E 2
L = $(LDIR)libvec.a

E 14
I 14
# %W% %G%
#
L =./libvec.a
E 14
D 2
O =	libvec.o \
	cefft.o cvfft.o refft.o rvfft.o convv.o rowcc.o	\
	dotpow.o doagc.o doclip.o doepow.o dogpow.o \
	doqbal.o dopbal.o doscale.o dotrap.o \
	quant.o balclip.o copyabs.o bfill.o	\
	operhan.o
E 2
I 2
CFLAGS = -g
FFLAGS = -g
E 2

D 2
#.SUFFIXES: .c .r .f .o
#.c.o:	;cc $(CFLAGS) -c $*.c;
E 2
I 2
D 13
.SUFFIXES: .c .f .a
#.c.o:
#	$(CC) -c $(CFLAGS) $<
#	ar rv $@ $*.o
#	/bin/rm -f $*.o
#.f.o:
#	$(FC) -c $(FFLAGS) $<
#	ar rv $@ $*.o
#	/bin/rm -f $*.o
E 2
#.f.o:	;f77 $(FFLAGS) -c $*.f;
#.r.f:	;ratfor $*.r > $*.f
E 13
I 13
.f.a:
	f77 $(FFLAGS) -c $<
	ar rv $@ $*.o
	rm -f $*.o
E 13

D 2
$L lvec : $O
	ar rv $L $O
E 2
I 2
D 13
O =	$L(libvec.o)							\
	$L(cefft.o) $L(cvfft.o) $L(refft.o) $L(rvfft.o) \
D 6
	$L(dotpow.o) $L(doagc.o) $L(doclip.o) $L(doepow.o) $L(dogpow.o) \
E 6
I 6
	$L(dotpow.o) $L(doclip.o) $L(doepow.o) $L(dogpow.o) \
E 6
	$L(doqbal.o) $L(dopbal.o) $L(doscale.o) $L(dotrap.o) \
D 12
	$L(quant.o) $L(balclip.o) $L(copyabs.o) $L(bfill.o)
E 12
I 12
	$L(quant.o) $L(balclip.o) $L(copyabs.o) $L(bfill.o)\
        $L(ibmflt.o)
E 13
I 13
O = $L(libvec.o)							\
    $L(cefft.o) $L(cvfft.o) $L(refft.o) $L(rvfft.o) \
    $L(dotpow.o) $L(doclip.o) $L(doepow.o) $L(dogpow.o) \
    $L(doqbal.o) $L(dopbal.o) $L(doscale.o) $L(dotrap.o) \
    $L(quant.o) $L(balclip.o) $L(copyabs.o) $L(bfill.o)\
    $L(ibmflt.o) \
    $L(operhan.o) $L(convv.o) $L(rowcc.o) $L(croscor.o) $L(blend.o)     \
D 14
	$L(toepl.o) $L(agcf.o) $L(winhan.o) $L(fftprep.o) $L(rfftm.o) \
E 14
I 14
    $L(toepl.o) $L(agcf.o) $L(winhan.o) $L(fftprep.o) $L(rfftm.o) \
E 14
    $L(nextfft.o) $L(cfftm.o) \
D 14
	$L(filrec.o) $L(precheb.o) $L(polcheb.o) $L(recform.o) \
	$L(semblan.o) $L(burg.o)
E 14
I 14
    $L(filrec.o) $L(precheb.o) $L(polcheb.o) $L(recform.o) \
    $L(semblan.o) $L(burg.o)
E 14
E 13
E 12

D 3
F = $Doperhan.o $Dconvv.o $Drowcc.o
E 3
I 3
D 4
F = $Doperhan.o $Dconvv.o $Drowcc.o $Dcroscor.o
E 4
I 4
D 13
F = $Doperhan.o $Dconvv.o $Drowcc.o $Dcroscor.o $Dblend.o     \
D 5
	$Dtoepl.o        
E 5
I 5
D 6
	$Dtoepl.o $Dagc.o
E 6
I 6
D 7
	$Dtoepl.o $Dagcf.o
E 7
I 7
D 8
	$Dtoepl.o $Dagcf.o $Dwinhan.o
E 8
I 8
	$Dtoepl.o $Dagcf.o $Dwinhan.o $Dfftprep.o $Drfftm.o \
D 9
    $Dnextfft.o $Dcfftm.o
E 9
I 9
    $Dnextfft.o $Dcfftm.o \
D 10
	$Dfilrec.o $Dprecheb.o $Dpolcheb.o $Drecform.o
E 10
I 10
D 12
	$Dfilrec.o $Dprecheb.o $Dpolcheb.o $Drecform.o $Dsemblan.o
E 12
I 12
	$Dfilrec.o $Dprecheb.o $Dpolcheb.o $Drecform.o $Dsemblan.o 
E 12
E 10
E 9
E 8
E 7
E 6
E 5
E 4
E 3

$L lvec : $O $F
	ar rv $L $F
E 13
I 13
$L lvec : $O
E 13
E 2
	ranlib $L

I 5
D 6
$Dagc.o	:agc.f
	$(FC) -c $(FFLAGS) agc.f
	mv agc.o $D
E 6
I 6
D 13
$Dagcf.o	:agcf.f
	$(FC) -c $(FFLAGS) agcf.f
	mv agcf.o $D
E 6
E 5
I 2
D 4
$Doperhan.o	:operhan.f
	$(FC) -c $(FFLAGS) operhan.f
	mv operhan.o $D
E 4
I 4
$Dblend.o	:blend.f
	$(FC) -c $(FFLAGS) blend.f
	mv blend.o $D
I 8
$Dcfftm.o	:cfftm.f
	$(FC) -c $(FFLAGS) cfftm.f
	mv cfftm.o $D
E 8
E 4
$Dconvv.o	:convv.f
	$(FC) -c $(FFLAGS) convv.f
	mv convv.o $D
I 3
$Dcroscor.o	:croscor.f
	$(FC) -c $(FFLAGS) croscor.f
	mv croscor.o $D
I 9
$Dfilrec.o	:filrec.f
	$(FC) -c $(FFLAGS) filrec.f
	mv filrec.o $D
E 9
I 8
$Dfftprep.o	:fftprep.f
	$(FC) -c $(FFLAGS) fftprep.f
	mv fftprep.o $D
$Dnextfft.o	:nextfft.f
	$(FC) -c $(FFLAGS) nextfft.f
	mv nextfft.o $D
E 8
I 4
$Doperhan.o	:operhan.f
	$(FC) -c $(FFLAGS) operhan.f
	mv operhan.o $D
I 9
$Dpolcheb.o	:polcheb.f
	$(FC) -c $(FFLAGS) polcheb.f
	mv polcheb.o $D
$Dprecheb.o	:precheb.f
	$(FC) -c $(FFLAGS) precheb.f
	mv precheb.o $D
$Drecform.o	:recform.f
	$(FC) -c $(FFLAGS) recform.f
	mv recform.o $D
E 9
I 8
$Drfftm.o	:rfftm.f
	$(FC) -c $(FFLAGS) rfftm.f
	mv rfftm.o $D
E 8
E 4
E 3
$Drowcc.o	:rowcc.f
	$(FC) -c $(FFLAGS) rowcc.f
	mv rowcc.o $D
I 4
$Dtoepl.o	:toepl.f
	$(FC) -c $(FFLAGS) toepl.f
	mv toepl.o $D
I 7
$Dwinhan.o	:winhan.f
	$(FC) -c $(FFLAGS) winhan.f
	mv winhan.o $D
I 10
$Dsemblan.o	:semblan.f
	$(FC) -c $(FFLAGS) semblan.f
	mv semblan.o $D
E 10
E 7
E 4

E 13
E 2
list	:
	ar tv $L

remake	:
D 13
	rm -f $L
E 13
I 13
	make clean
E 13
	make $L

D 2
operhan.o	:operhan.f
	f77 -c $(CFLAGS) operhan.f
rowcc.o	:rowcc.f
	f77 -c $(CFLAGS) rowcc.f
convv.o	:convv.f
	f77 -c $(CFLAGS) convv.f
E 2
I 2
D 13
#operhan.o	:operhan.f
#	f77 -c $(FFLAGS) operhan.f
#rowcc.o	:rowcc.f
#	f77 -c $(FFLAGS) rowcc.f
#convv.o	:convv.f
#	f77 -c $(FFLAGS) convv.f
E 2

E 13
clean	:
I 2
D 13
	-/bin/rm $L
E 13
I 13
	-rm $L
E 13
E 2
	-/bin/rm -f *.o
	-/bin/rm -f *.a
E 1
