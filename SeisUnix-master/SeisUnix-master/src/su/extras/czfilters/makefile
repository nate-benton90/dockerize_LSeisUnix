# Description file for cz running under SU

# $Author: jkc $
# $Source: /src/su/czfilters/RCS/makefile,v $
# $Revision: 1.11 $ ; $Date: 88/05/21 23:42:16 $

.PRECIOUS:	print

B  	=	/usr/local/bin
L    	=	-lsu
I	=	../include
SRC	= 	/src/su/czfilters
TEXTS	=	$(SRC)/cz1fil.c $(SRC)/cz2fil.c $(SRC)/cz3fil.f
CFLAGS	=	-O -q
FFLAGS	=	-O3 -u
#CFLAGS	=	-g -q
#FFLAGS	=	-g -u

# Install
install:	$B/cz1fil $B/cz2fil $B/cz3fil
	@/bin/touch $@

$B/cz1fil cz1fil: cz1fil.c $(I)/segy.h $(I)/cwp.h
	cc $(CFLAGS) -o $B/cz1fil $(SRC)/cz1fil.c $L

$B/cz2fil cz2fil: cz2fil.c $(I)/segy.h $(I)/cwp.h
	cc $(CFLAGS) -o $B/cz2fil $(SRC)/cz2fil.c $L

$B/cz3fil cz3fil: cz3fil.f
	f77 $(FFLAGS) -o $B/cz3fil $(SRC)/cz3fil.f


# Utilities
reinstall:
	/bin/rm $B/cz1fil $B/cz2fil $B/cz3fil
	make install

record:	IMAGE
	@$B/t >record  # crashing any old record
	@/bin/echo comment: >>record
	@/usr/bin/vi comment
	@/bin/cat comment >>record
	@/bin/echo >>record
	@/bin/echo BOUNDS: >>record
	@/bin/cat BOUNDS >>record
	@/bin/echo >>record
	@/bin/echo data1in: >>record
	@/bin/cat data1in >>record
	@/bin/echo >>record
	@/bin/echo data2in: >>record
	@/bin/cat data2in >>record
	@/bin/echo >>record
	@/bin/echo CZINFO: >>record
	@/bin/cat CZINFO >>record
	@/bin/echo >>record
	@/bin/echo paraddhead: >>record
	@/bin/cat paraddhead >>record
	@/bin/echo >>record
	@/bin/echo parshw1: >>record
	@/bin/cat parshw1 >>record
	@/bin/echo >>record
	@/bin/echo parshw2: >>record
	@/bin/cat parshw2 >>record
	$B/hpr record

clean:
	rm -f BARE CZINFO DATA IMAGE INVTRACES PTRACES SYNINFO TRACES  \
	data1in data2in parfil par.tmp paraddhead parshw1 parshw2 record

print:	$(TEXTS)
	$B/croff $?
	@/bin/touch $@
