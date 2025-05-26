
# Makefile
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /net/denton/pad/data5/zli/lf
ROOT = $(SU)/cwp
UROOT = $(SU)/slt
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
# Search on the word "Comment" to complete adaptation

I = $(ROOT)/include
K = $(UROOT)/include
D = 
CC = cc
FC = $(FC)

CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= $(OPTC) -I$K -I$I

LIB = $(UROOT)/lib/libvelo.a

ARCH =				\
	$(LIB)(dvdzread.o)	\
	$(LIB)(printsc3d.o)	\
	$(LIB)(printhvel.o)	\
	$(LIB)(printhvelxy.o)	\
	$(LIB)(printhvel2.o)	\
	$(LIB)(printvs3d.o)	\
	$(LIB)(readhvel.o)	\
	$(LIB)(readvs3d.o)	\
	$(LIB)(removeinv.o)	\
	$(LIB)(vconvert.o) 	\
	$(LIB)(velfread.o) 	\
	$(LIB)(veloread.o)	\
	$(LIB)(vg.o) 		\
	$(LIB)(vsm3d.o) 	\
	$(LIB)(vsmsub.o) 	\
	$(LIB)(vsubs.o)		\
	$(LIB)(wavel.o)


INSTALL:	$(LIB)
	@touch $@

$(ARCH)	: $D

$(LIB)	:	$(ARCH)

remake	:
	@rm -f $(LIB)
	@make ROOT=$(ROOT) OPTC=$(OPTC)

list	:
	ar tv $(LIB)

clean:
	rm -f junk* core a.out

# Can comment out if your make knows about C libs
.c.a:
	@$(CC) -c $(CFLAGS) $<
	@ar rv $@ $*.o
	@rm -f $*.o

.f.a:
	@$(FC) -c $(FFLAGS) $<
	@ar rv $@ $*.o
	@rm -f $*.o


.PRECIOUS:	 $(LIB)
