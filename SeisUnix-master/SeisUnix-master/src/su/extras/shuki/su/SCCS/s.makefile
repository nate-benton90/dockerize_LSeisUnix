h17898
s 00037/00023/00016
d D 1.3 88/11/15 14:01:03 shuki 3 2
c 
e
s 00001/00001/00038
d D 1.2 88/05/16 06:40:56 shuki 2 1
c 
e
s 00039/00000/00000
d D 1.1 88/04/14 11:24:20 shuki 1 0
c date and time created 88/04/14 11:24:20 by shuki
e
u
U
f e 0
t
T
I 1
#
DIRS =	include lsu lcplot lvec progs pens plotprogs

#all	:
#	foreach i ($(DIRS))			;\
#		echo $i				;\
#	end

#	for i in $(DIRS)			;\
#	do					;\
#		echo $i				;\
#	done

default	:
D 3
	(cd include; make)
D 2
	(cd lCC; make)
E 2
I 2
#(cd lCC; make)
E 2
	(cd lsu; make)
	(cd lvec; make)
	(cd lcplot; make)
	(cd pens; make "BIN=/usr/local/")
	(cd plotprogs; make "BIN=/usr/local/")
	(cd progs; make "BIN=/usr/local/")
	(cd tests; ptest "BIN=/usr/local/")
	(cd doc; make)
E 3
I 3
	(cd include;   make )
	(cd lsu;       make )
	(cd lvec;      make )
	(cd lcplot;    make )
	(cd pens;      make "BDIR=../bin/" )
	(cd plotprogs; make "BDIR=../bin/" )
	(cd progs;     make "BDIR=../bin/" )
	(cd tests;     ptest)
E 3

I 3
#(cd include; make)
#(cd lCC; make "LDIR=/data/shuki/lib/")
#(cd lsu;       make "LDIR=/data/shuki/lib/" )
#(cd lvec;      make "LDIR=/data/shuki/lib/" )
#(cd lcplot;    make "LDIR=/data/shuki/lib/" )
#(cd pens;      make "BDIR=/data/shuki/bin/" )
#(cd plotprogs; make "BDIR=/data/shuki/bin/" )
#(cd progs;     make "BDIR=/data/shuki/bin/" )
#(cd tests;     ptest                        )
#(cd doc; make)

E 3
clean	:
D 3
	(cd include; make clean)
	(cd lCC; make clean)
	(cd lsu; make clean)
	(cd lvec; make clean)
	(cd lcplot; make clean)
	(cd pens; make clean)
	(cd plotprogs; make clean)
	(cd progs; make clean)
	(cd tests; make clean)
	(cd doc; make clean)
	find . -name core -print
	find . -name a.out -print
	find . -name \*j\* -print
E 3
I 3
	-rm bin/*
	-rm */*.o
	-rm */*.a
	-rm tests/j*

#(cd include; make clean)
#(cd lCC; make clean)
#(cd lsu; make clean)
#(cd lvec; make clean)
#(cd lcplot; make clean)
#(cd pens; make clean)
#(cd plotprogs; make clean)
#(cd progs; make clean)
#(cd tests; make clean)
#(cd doc; make clean)
#find . -name core -print
#find . -name a.out -print
#find . -name \*j\* -print
E 3
E 1
