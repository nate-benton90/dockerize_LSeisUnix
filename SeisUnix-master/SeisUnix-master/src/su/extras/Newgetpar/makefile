# $Author: jkc $
# $Source: /src/su/src/RCS/makefile,v $
# $Revision: 1.31 $ ; $Date: 88/06/03 13:11:45 $

# This makefile is complicated by the fact that we wanted to have:
#	(1) the binaries in a remote directory with the stripped names
#	(2) not have a separate make routine for each source
#


#B = /usr/local/bin
B = .
L = $B/lib

# Commented lines are for those who don't have their libraries in the
# "standard" place.

#LIB = $L/libsu.a
#V = $L/libvec.a
#FFT = $L/libfft.a
#FFTS = $L/libfftsep.a
#C = $L/libcplot.a
LIB = getpar.o -lsu
V = -lvec
FFT = -lfft
FFTS = -lfftsep
C = -lcplot

# Following is somewhat system dependent
L77 = -lF77 -lU77 -lI77 -lF77 -lm

S = ./Subs
I = ../include/segy.h ../include/cwp.h
P = ../include/portdefs.h ../include/limits.h ../include/cwpdefs.h

XX =	$B/sunmo  $B/sulogdmo\
	$B/suiomega $B/sufft2d\
	$B/sugazmig $B/sugazdc $B/suhaledmo

XXX =	$B/supmovie

XA =	su3dchart suabshw suaddhead suchart	\
	suchw suedit suflip sugethw		\
	sumute surange susort suspike		\
	suplane sustrip sutab suwind sumark	\
	segyhdrs segyread segywrite

XM =	suamp suilog suintvel sulog sushw sustkvel	\
	suunpack1 suunpack2

XF =	suband suenv sufft suhilb suifft

XV =	suabs sudiff sumax supack1 supack2	\
	supow suprod suquo surecip sustack susum suus

XVF =
XVM =	suaddnoise suagc sugain
XPVM =	suwig sutwig sutwig2 sutwig2v sutwig4 supwig sucontour \
	sugraph suqgraph suwignmax

Q = q
#CFLAGS= -O -$Q -s
OPT = O3
FFLAGS= -$(OPT) -c
CFLAGS= -g -q
#FFLAGS= -g -q

.PRECIOUS	: print

# supmovie is not included in "all" because it currently has Masscomp calls
all	:$(XX) $(XA) $(XM) $(XF) $(XV) $(XVM) $(XPVM)
	touch $@

remake	:
	-touch *.c
	make all
	
$(XA)	:$$@.c $I
	-cc $(CFLAGS) $@.c $(LIB) -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$(XM)	:$$@.c $I
	-cc $(CFLAGS) $@.c $(LIB) -lm -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$(XV)	:$$@.c $I
	-cc $(CFLAGS) $@.c $(LIB) $V -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$(XVM)	:$$@.c $I
	-cc $(CFLAGS) $@.c $(LIB) $V -lm -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$(XF)	:$$@.c $I
	-cc $(CFLAGS) $@.c $(LIB) $(FFT) -lm -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$(XPVM)	:$$@.c $I
	-cc $(CFLAGS) $@.c $C $(LIB) $V -lm -o $@
	@chmod 775 $@
	@mv -f $@ $B/$@

$B/supmovie supmovie	: supmovie.o $I
	-cc $(CFLAGS) supmovie.o $(LIB) $V -lgp -o $B/supmovie

$B/sufft2d sufft2d: sufft2d.o 2dfftsbs.o $I
	-cc sufft2d.o 2dfftsbs.o $(LIB) $V $(FFTS) $(L77) -o $B/sufft2d
	rm sufft2d.o 2dfftsbs.o
2dfftsbs.o:	$S/2dfftsbs.f
	f77 $(FFLAGS) $S/2dfftsbs.f $V $(FFTS)

$B/sunmo sunmo: sunmo.o nmosubs.o $I
	-cc sunmo.o nmosubs.o $(LIB) $V $(L77) -o $B/sunmo
	rm sunmo.o nmosubs.o
nmosubs.o:	$S/nmosubs.f
	f77 $(FFLAGS) $S/nmosubs.f $V

$B/sulogdmo		:sulogdmo.o logdmosbs.o  $I
	-cc sulogdmo.o logdmosbs.o $(LIB) $(FFTS) $(L77) -o $B/sulogdmo
	rm sulogdmo.o logdmosbs.o
logdmosbs.o:	$S/logdmosbs.f
	f77 $(FFLAGS) $S/logdmosbs.f

$B/suhaledmo suhaledmo	:suhaledmo.o haledmosbs.o  $I
	-cc suhaledmo.o haledmosbs.o $(LIB) $(FFTS) $(L77) -o $B/suhaledmo
	rm suhaledmo.o haledmosbs.o
haledmosbs.o		:$S/haledmosbs.f
	f77 $(FFLAGS) $S/haledmosbs.f

$B/sugazmig sugazmig	:sugazmig.o gazmigsbs.o  $I
	-cc sugazmig.o gazmigsbs.o $(LIB) $V $(FFTS) $(L77) -o $B/sugazmig
	rm sugazmig.o gazmigsbs.o
gazmigsbs.o		:$S/gazmigsbs.f $I
	f77 $(FFLAGS) $S/gazmigsbs.f

$B/sugazdc sugazdc	:sugazdc.o gazdcsbs.o  $I
	-cc sugazdc.o gazdcsbs.o $(LIB) $V $(FFTS) $(L77) -o $B/sugazdc
	rm sugazdc.o gazdcsbs.o
gazdcsbs.o		:$S/gazdcsbs.f $I
	f77 $(FFLAGS) $S/gazdcsbs.f

$B/suiomega suiomega		:suiomega.o iomegasubs.o  $I
	-cc suiomega.o iomegasubs.o $(LIB) $(FFTS) $(L77) -o $B/suiomega
	rm suiomega.o iomegasubs.o
iomegasubs.o		:$S/iomegasubs.f
	f77 $(FFLAGS) $S/iomegasubs.f

#include file dependencies
$I	: $P

list:
	@ls *.c | pr -5 -t -l22 | p 

clean:
	rm -f a.out junk* JUNK* core

printall:
	@for i in *.c			;\
	do				 \
		croff $$i		;\
	done

print:	*.c
	@for i in $?			;\
	do				 \
		echo "printing $$i ..."	;\
		croff $$i		;\
	done
	touch $@
