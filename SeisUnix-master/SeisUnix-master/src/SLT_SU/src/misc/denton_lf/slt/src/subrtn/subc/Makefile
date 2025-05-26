# Makefile for ...cwp/lib
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
SU = /files2/data5/zli/lf
ROOT = $(SU)/cwp
UROOT = $(SU)/slt

OPTC = -xO3 -fast
# Search on the word "Comment" to complete adaptation

I = $(ROOT)/include  
K = $(UROOT)/include
D = 
CC= cc

CFLAGS= $(OPTC) -I$K -I$I

#Comment out if your make knows about C libs
.c.a:
	$(CC) -c $(CFLAGS) $<
	@ar rv $@ $*.o
	@rm -f $*.o


.PRECIOUS:	 $(LIB)

LIB = $(UROOT)/lib/libsubc.a

ARCH =				\
	$(LIB)(ieeefinite.o)	\
	$(LIB)(gettime.o)	\
	$(LIB)(intsln.o)	\
	$(LIB)(linefit.o)		\
	$(LIB)(msgsc.o)		\
	$(LIB)(pfft2d.o)	\
	$(LIB)(qkisort.o)

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
