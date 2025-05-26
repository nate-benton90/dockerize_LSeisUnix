h36802
s 00003/00004/00020
d D 1.3 88/11/15 14:04:52 shuki 3 2
c 
e
s 00001/00002/00023
d D 1.2 88/05/03 09:22:46 shuki 2 1
c 
e
s 00025/00000/00000
d D 1.1 88/04/14 14:00:57 shuki 1 0
c date and time created 88/04/14 14:00:57 by shuki
e
u
U
f e 0
t
T
I 1
#
# makefile for the su/lCC directory
#
D 3
#L = ./libCC.a
LDIR	= /usr/local/lib/
L = $(LDIR)libCC.a

E 3
I 3
# %W% %G%
#
L = ./libCC.a
E 3
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
D 2
	-/bin/rm -f *.o *.a

E 2
I 2
	-/bin/rm -f *.o *.a $L
E 2
E 1
