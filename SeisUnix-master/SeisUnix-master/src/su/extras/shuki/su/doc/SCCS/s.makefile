h11306
s 00001/00001/00015
d D 1.4 88/05/03 17:38:05 valer 4 3
c 
e
s 00003/00000/00013
d D 1.3 88/04/26 09:26:36 shuki 3 2
c 
e
s 00007/00014/00006
d D 1.2 88/04/14 12:22:12 shuki 2 1
c 
e
s 00020/00000/00000
d D 1.1 88/04/14 11:30:04 shuki 1 0
c date and time created 88/04/14 11:30:04 by shuki
e
u
U
f e 0
t
T
I 1
#
# makefile for su/doc directory
#

D 2
C =	apass.cat dotpow.cat err.cat getbh.cat getpar.cat	\
	gettr.cat hisclose.cat hislog.cat			\
	hispr.cat input.cat output.cat				\
	putbh.cat puttr.cat selfdoc.cat				\
	sufilt.cat sutpow.cat sugpow.cat			\
        dogpow.cat
E 2
I 2
M = /usr/man/manl/
E 2

D 2
.SUFFIXES: .1l .cat
.1l.cat:	;nroff -man $*.1l > $*.cat
E 2
I 2
S = apass.1l dotpow.1l err.1l getbh.1l getpar.1l gettr.1l hisclose.1l \
       hislog.1l hispr.1l input.1l output.1l putbh.1l puttr.1l selfdoc.1l \
D 4
       sufilt.1l sutpow.1l sugpow.1l dogpow.1l
E 4
I 4
       sufilt.1l sutpow.1l sugpow.1l dogpow.1l supef.1l
E 4
E 2

D 2
all	:$C
	@echo made

clean	:
	/bin/rm -f *.cat

E 2
I 2
install:
	cp $S $M
	chmod 644 $M$S
I 3

clean	:
	@echo "clean what?"
E 3
E 2
E 1
