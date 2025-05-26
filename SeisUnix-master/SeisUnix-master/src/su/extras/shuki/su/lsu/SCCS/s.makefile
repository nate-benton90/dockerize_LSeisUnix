h32774
s 00004/00005/00023
d D 1.7 88/11/15 14:01:58 shuki 7 6
c 
e
s 00004/00005/00024
d D 1.6 88/11/06 09:52:08 shuki 6 5
c 
e
s 00002/00001/00027
d D 1.5 88/09/01 07:58:19 shuki 5 4
c Malloc
e
s 00001/00000/00027
d D 1.4 88/05/03 09:19:23 shuki 4 3
c 
e
s 00001/00001/00026
d D 1.3 88/04/28 09:55:01 shemer 3 2
c 
e
s 00001/00001/00026
d D 1.2 88/04/26 09:29:29 shuki 2 1
c 
e
s 00027/00000/00000
d D 1.1 88/04/14 13:48:48 shuki 1 0
c date and time created 88/04/14 13:48:48 by shuki
e
u
U
f e 0
t
T
I 1
#
# makefile for the su/lsu directory
#
D 7
#L = ./libsu.a
LDIR	= /usr/local/lib/
L = $(LDIR)libsu.a

E 7
I 7
# %W% %G%
#
L = ./libsu.a
E 7
CFLAGS = -g
FFLAGS = -g

D 6
O = $L(msgpkge.o) $L(iopkge.o) $L(getpars.o) $L(valpkge.o) $L(hdrpkge.o) \
    $L(isatape.o) $L(isapipe.o) $L(isadir.o) $L(isadisk.o) $L(isadevnull.o) \
    $L(selfdoc.o) $L(gname.o) \
D 3
    $L(powerof.o) $L(getclip.o)
E 3
I 3
D 5
    $L(powerof.o) $L(getclip.o) $L(velfile.o)
E 5
I 5
    $L(powerof.o) $L(getclip.o) $L(velfile.o) \
	$L(Malloc.o)
E 6
I 6
O = $L(msgpkge.o) $L(iopkge.o)  $L(getpars.o) $L(valpkge.o) $L(hdrpkge.o) \
    $L(isatape.o) $L(isapipe.o) $L(isadir.o)  $L(isadisk.o) $L(isadevnull.o) \
    $L(selfdoc.o) $L(gname.o)   $L(powerof.o) $L(getclip.o) $L(velfile.o) \
D 7
	$L(Malloc.o)  $L(dtime.o)
E 7
I 7
    $L(Malloc.o)  $L(dtime.o)
E 7
E 6
E 5
E 3

$L	:$O
	ranlib $L

remake	:
	-make clean
	make $L

list	:
	ar tv $L

clean	:
D 2
	-/bin/rm -f *.o *.a $L
E 2
I 2
	-/bin/rm -f *.o *.a
I 4
	-/bin/rm $L
E 4
E 2
E 1
