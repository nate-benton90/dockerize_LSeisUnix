#
# makefile for the su/lvec directory
#
# @(#)makefile	1.13 11/15/88
#
L =./libvec.a
CFLAGS = -g
FFLAGS = -g

.f.a:
	f77 $(FFLAGS) -c $<
	ar rv $@ $*.o
	rm -f $*.o

O = $L(libvec.o)							\
    $L(cefft.o) $L(cvfft.o) $L(refft.o) $L(rvfft.o) \
    $L(dotpow.o) $L(doclip.o) $L(doepow.o) $L(dogpow.o) \
    $L(doqbal.o) $L(dopbal.o) $L(doscale.o) $L(dotrap.o) \
    $L(quant.o) $L(balclip.o) $L(copyabs.o) $L(bfill.o)\
    $L(ibmflt.o) \
    $L(operhan.o) $L(convv.o) $L(rowcc.o) $L(croscor.o) $L(blend.o)     \
    $L(toepl.o) $L(agcf.o) $L(winhan.o) $L(fftprep.o) $L(rfftm.o) \
    $L(nextfft.o) $L(cfftm.o) \
    $L(filrec.o) $L(precheb.o) $L(polcheb.o) $L(recform.o) \
    $L(semblan.o) $L(burg.o)

$L lvec : $O
	ranlib $L

list	:
	ar tv $L

remake	:
	make clean
	make $L

clean	:
	-rm $L
	-/bin/rm -f *.o
	-/bin/rm -f *.a
