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
	(cd include;   make )
	(cd lsu;       make )
	(cd lvec;      make )
	(cd lcplot;    make )
	(cd pens;      make "BDIR=../bin/" )
	(cd plotprogs; make "BDIR=../bin/" )
	(cd progs;     make "BDIR=../bin/" )
	(cd tests;     ptest)

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

clean	:
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
