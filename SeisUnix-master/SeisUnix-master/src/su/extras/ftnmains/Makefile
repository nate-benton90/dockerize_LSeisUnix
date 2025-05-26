# Makefile for ...su/main
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local
OPTC = -O
OPTF = -O

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
FFLAGS = -c $(OPTF)
LFLAGS= -L$L -lsu -lpar -lcwp -lm

# Following is system dependent
Lftn = -lxlf -lm
#Lftn = -lF77 -lU77 -lI77 -lF77 -lm



S = ./Subs

# These codes are "special" (use Fortran subroutines)
XX =	$B/sulogdmo  $B/sudmostk \
	$B/suhalekdmo \
	$B/sufft2d   \
	$B/sugazmig $B/sugazdc $B/suhaledmo

INSTALL	: $(XX)
	touch $@

remake	:
	-touch *.f
	make ROOT=$(ROOT) OPTC=$(OPTC) OPTF=$(OPTF)
	


#$B/sudecon sudecon: sudecon.c $S/deconsubs.f
#	-$(FC) $(FFLAGS) $S/deconsubs.f
#	-$(CC) $(CFLAGS) sudecon.c deconsubs.o $(LFLAGS) $(Lftn) -o $B/sudecon
#	@-rm sudecon.o deconsubs.o
#$B/suva suva	:suva.c $S/svanlsubs.f $S/nmosubs.f
#	$(FC) $(FFLAGS) $S/svanlsubs.f
#	$(FC) $(FFLAGS) $S/nmosubs.f
#	-$(CC) $(CFLAGS) suva.c svanlsubs.o nmosubs.o $(LFLAGS) $(Lftn) -o $B/suva
#	@-rm suva.o svanlsubs.o nmosubs.o

$B/sufft2d sufft2d: sufft2d.c $S/2dfftsbs.f
	-$(FC) $(FFLAGS) $S/2dfftsbs.f
	-$(CC) $(CFLAGS) sufft2d.c 2dfftsbs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/sufft2d
	@-rm sufft2d.o 2dfftsbs.o


$B/sulogdmo sulogdmo	:sulogdmo.c $S/logdmosbs.f
	-$(FC) $(FFLAGS) $S/logdmosbs.f
	-$(CC) $(CFLAGS) sulogdmo.c logdmosbs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/sulogdmo
	@-rm sulogdmo.o logdmosbs.o

$B/suhaledmo suhaledmo	:suhaledmo.c $S/haledmosbs.f
	-$(FC) $(FFLAGS) $(Lftn) $S/haledmosbs.f
	-$(CC) $(CFLAGS) suhaledmo.c haledmosbs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/suhaledmo
	@-rm suhaledmo.o haledmosbs.o

$B/suzoep suzoep	:suzoep.c $S/zoepsubs.f
	-$(FC) $(FFLAGS) $S/zoepsubs.f
	-$(CC) $(CFLAGS) suzoep.c zoepsubs.o $(LFLAGS) $(Lftn) -o $B/suzoep
	@-rm suzoep.o zoepsubs.o

$B/suhalekdmo suhalekdmo	:suhalekdmo.c $S/haleksubs.f
	-$(FC) $(FFLAGS) $S/haleksubs.f
	-$(CC) $(CFLAGS) suhalekdmo.c haleksubs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/suhalekdmo
	@-rm suhalekdmo.o haleksubs.o

$B/sudmostk sudmostk	:sudmostk.c $S/dmostksubs.f
	-$(FC) $(FFLAGS) $S/dmostksubs.f
	-$(CC) $(CFLAGS) sudmostk.c dmostksubs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/sudmostk
	@-rm sudmostk.o dmostksubs.o

$B/sugazmig sugazmig	:sugazmig.c $S/gazmigsbs.f
	-$(FC) $(FFLAGS) $S/gazmigsbs.f
	-$(CC) $(CFLAGS) sugazmig.c gazmigsbs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/sugazmig
	@-rm sugazmig.o gazmigsbs.o

$B/sugazdc sugazdc	:sugazdc.c $S/gazdcsbs.f
	-$(FC) $(FFLAGS) $S/gazdcsbs.f
	-$(CC) $(CFLAGS) sugazdc.c gazdcsbs.o \
		$(LFLAGS) -lfftsep $(Lftn) -o $B/sugazdc
	@-rm sugazdc.o gazdcsbs.o


clean:
	rm -f a.out junk* JUNK* core

#list:
#	@ls *.c | pr -5 -t -l22 | p 
#
#printall:
#	@for i in *.c			;\
#	do				 \
#		croff $$i		;\
#	done
#
#print:	*.c
#	@for i in $?			;\
#	do				 \
#		echo "printing $$i ..."	;\
#		croff $$i		;\
#	done
#	touch $@
