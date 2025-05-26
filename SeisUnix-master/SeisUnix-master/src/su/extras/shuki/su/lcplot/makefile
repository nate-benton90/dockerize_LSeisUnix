#
# Makefile for libcplot
#
# @(#)makefile	1.3 11/15/88
#
L	= ./libcplot.a
O	= $L(libcplot.o)

CFLAGS	= -g

$L lcplot : $O
	ranlib $L

list	:
	ar tv $L

remake	:
	rm -f $L
	make $L

clean	:
	-/bin/rm -f *.o
	-/bin/rm -f *.a
	-/bin/rm -f $L
