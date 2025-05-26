# Makefile for ...cwp/lib
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /net/denton/pad/data5/zli/lf
ROOT = $(SU)/cwp
UROOT = $(SU)/slt
OPTC = -xO3 -fast -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64
# Search on the word "Comment" to complete adaptation

I = $(ROOT)/include 
K = $(UROOT)/include
L = $(UROOT)/lib
D = 
CC = cc

CFLAGS= $(OPTC) -I$K -I$I
FFLAGS= -O3 -fast -I$K -I$I

LIB = $L/libusu.a

ARCH =				\
	$(LIB)(dpf.o)		\
	$(LIB)(dipscn.o)		\
	$(LIB)(itov.o)		\
	$(LIB)(xy2sl.o)		\
	$(LIB)(xy2sldb.o)		\
	$(LIB)(sl2xy.o)		\
	$(LIB)(sl2xydb.o)		\
	$(LIB)(tar3.o)		\
	$(LIB)(f2p5.o)

INSTALL:	$(LIB)
	@touch $@

$(ARCH)	: $D

$(LIB)	:	$(ARCH)
	ranlib $(LIB)

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
