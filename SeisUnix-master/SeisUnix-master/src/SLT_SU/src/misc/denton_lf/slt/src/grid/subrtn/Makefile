# Makefile for libgrid.a
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /pad/data5/zli/lf
ROOT =$(SU)/cwp
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

LIB = $(UROOT)/lib/libgrid.a

ARCH =				\
	$(LIB)(grids.o)		\
	$(LIB)(l2g.o)		\
	$(LIB)(usgheds.o)

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
