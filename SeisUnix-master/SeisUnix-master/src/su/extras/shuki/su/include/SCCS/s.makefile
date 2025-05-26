h44327
s 00000/00000/00016
d D 1.3 88/11/15 14:01:17 shuki 3 2
c 
e
s 00003/00004/00013
d D 1.2 88/04/14 14:10:21 shuki 2 1
c 
e
s 00017/00000/00000
d D 1.1 88/04/14 14:04:07 shuki 1 0
c date and time created 88/04/14 14:04:07 by shuki
e
u
U
f e 0
t
T
I 1
CFLAGS = -g

hdrs.h	:mkhdrs
	mkhdrs > hdrs.h

mkhdrs	:mkhdrs.c su.h trhdr.h bhdr.h
	cc $(CFLAGS) mkhdrs.c -o mkhdrs

bhdr.h	:su.h mkbhdr.sh
D 2
	mkbhdr.sh > bhdr.h
E 2
I 2
	sh mkbhdr.sh > bhdr.h
E 2

trhdr.h	:su.h mktrhdr.sh
D 2
	mktrhdr.sh > trhdr.h
E 2
I 2
	sh mktrhdr.sh > trhdr.h
E 2

clean	:
D 2
	-/bin/rm -f mkhdrs
	-/bin/rm -f *.o
E 2
I 2
	-/bin/rm -f mkhdrs *.o hdrs.h bhdr.h trhdr.h
E 2
E 1
