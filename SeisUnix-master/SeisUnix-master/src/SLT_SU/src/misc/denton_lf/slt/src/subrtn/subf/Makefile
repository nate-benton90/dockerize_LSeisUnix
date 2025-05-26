# Makefile for ...subf/lib
SHELL = /bin/sh

SU = /files2/data5/zli/lf
ROOT = $(SU)/cwp
UROOT = $(SU)/slt

OPTC = -O3 -fast
# Search on the word "Comment" to complete adaptation

I = $(ROOT)/include
K = $(UROOT)/include

# Safest to use next line, but ...
D = 
CC = cc
FC = $(FC)

CFLAGS= $(OPTC) -I$I -I$K

LIB = $(UROOT)/lib/libsubf.a

ARCH =				\
	$(LIB)(agc.o)	\
	$(LIB)(bilint.o)	\
	$(LIB)(cancel.o)	\
	$(LIB)(cfft.o)	\
	$(LIB)(cfft2d.o)	\
	$(LIB)(ctris.o)	\
	$(LIB)(diskio.o)	\
	$(LIB)(dskmpx.o)	\
	$(LIB)(dummyopen.o)	\
	$(LIB)(four1.o)	\
	$(LIB)(ftncls.o)	\
	$(LIB)(intp2d.o)	\
	$(LIB)(lens.o)	\
	$(LIB)(linint.o)	\
	$(LIB)(pol3coe.o)	\
	$(LIB)(polyint.o)	\
	$(LIB)(qcksrt.o)	\
	$(LIB)(qcksti.o)	\
	$(LIB)(rfft.o)	\
	$(LIB)(smooth.o)	\
	$(LIB)(tp.o)	\
	$(LIB)(v2tris.o)	\
	$(LIB)(vctris.o)	\
	$(LIB)(vrfft.o)	\
	$(LIB)(radix.o)		\
	$(LIB)(spint.o)		\
	$(LIB)(zeroal.o)

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
	@$(FC) -c $(CFLAGS) $<
	@ar rv $@ $*.o
	@rm -f $*.o


.PRECIOUS:	 $(LIB)
