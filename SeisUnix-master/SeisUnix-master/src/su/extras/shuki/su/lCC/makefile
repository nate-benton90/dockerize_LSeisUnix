#
# makefile for the su/lCC directory
#
# @(#)makefile	1.3 11/15/88
#
L = ./libCC.a
CFLAGS = -g
FFLAGS = -g

O =	$L(index.o) $L(bcopy.o) $L(bzero.o)

$L	:$O
	ranlib $L

remake	:
	-make clean
	make $L

list	:
	ar tv $L

clean	:
	-/bin/rm -f *.o *.a $L
