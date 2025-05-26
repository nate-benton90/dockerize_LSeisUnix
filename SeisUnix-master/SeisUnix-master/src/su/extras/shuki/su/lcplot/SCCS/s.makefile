h34934
s 00003/00003/00021
d D 1.3 88/11/15 14:08:56 shuki 3 2
c 
e
s 00001/00000/00023
d D 1.2 88/05/03 09:20:29 shuki 2 1
c 
e
s 00023/00000/00000
d D 1.1 88/04/14 13:56:06 shuki 1 0
c date and time created 88/04/14 13:56:06 by shuki
e
u
U
f e 0
t
T
I 1
#
# Makefile for libcplot
#
D 3
#L	= ./libplot.a
LDIR	= /usr/local/lib/
L = $(LDIR)libcplot.a
E 3
I 3
# %W% %G%
#
L	= ./libcplot.a
E 3
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
I 2
	-/bin/rm -f $L
E 2
E 1
