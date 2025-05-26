#
# makefile for the su/lsu directory
#
# @(#)makefile	1.7 11/15/88
#
L = ./libsu.a
CFLAGS = -g
FFLAGS = -g

O = $L(msgpkge.o) $L(iopkge.o)  $L(getpars.o) $L(valpkge.o) $L(hdrpkge.o) \
    $L(isatape.o) $L(isapipe.o) $L(isadir.o)  $L(isadisk.o) $L(isadevnull.o) \
    $L(selfdoc.o) $L(gname.o)   $L(powerof.o) $L(getclip.o) $L(velfile.o) \
    $L(Malloc.o)  $L(dtime.o)

$L	:$O
	ranlib $L

remake	:
	-make clean
	make $L

list	:
	ar tv $L

clean	:
	-/bin/rm -f *.o *.a
	-/bin/rm $L
