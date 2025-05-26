# Description file for SU includes
# Note that hdr.h is created, not retrieved
#
# $Author: jkc $
# $Source: /src/su/include/RCS/makefile,v $
# $Revision: 1.9 $ ; $Date: 88/05/15 06:37:27 $

.PRECIOUS	: print

CFLAGS = -g -q

all	: hdr.h cwp.h
	touch $@

hdr.h	:mkhdr
	mkhdr >hdr.h
mkhdr	:mkhdr.c offsets.h prehdr.h
	cc mkhdr.c -o mkhdr
offsets.h	:mkoffs.sh segy.h
	/lib/cpp segy.h | mkoffs.sh >offsets.h
prehdr.h	:mkprehdr.sh segy.h
	/lib/cpp segy.h | mkprehdr.sh >prehdr.h

cwp.h	: cwpdefs.h portdefs.h
	touch $@

clean	:
	rm -f mkhdr offsets.h prehdr.h *.o

remake	:
	@make clean
	@make hdr.h

printall:
	@for i in *.h			;\
	do				 \
		croff $$i		;\
	done

print:	*.h
	@for i in $?			;\
	do				 \
		echo "printing $$i ..."	;\
		 croff $$i		;\
	done
	touch $@
