# Makefile for sulib
#############################################################################
# the archiver
AR = ar 

#############################################################################
# ar FLAGS
ARFLAGS = rv

#############################################################################
# ranlib definition
RANLIB = ranlib

# ranlib flags
RANFLAGS = 

SUL = $(CWPROOT)/lib
SUI = $(CWPROOT)/include
I = ../include

LFLAGS= -L$SUL -lcwp -lsu

L = $(HOME)/prog/lib

LIB = $L/libsulib.a

# standard CFLAGS
CC = gcc
CFLAGS = -I$(SUI) -I$(I) -O3  -m64 -DLITTLE -DCWP_LITTLE_ENDIAN -D_FILE_OFFSET_BITS=64 -D_LARGEFILE_SOURCE  -Wall -ansi

ARCH =			\
	$(LIB)(3dbin.o)	\
	$(LIB)(corr.o)	\
	$(LIB)(filter.o)	\
	$(LIB)(fgettra.o)	\
	$(LIB)(getgthr.o)	\
	$(LIB)(shuffle.o)	\
	$(LIB)(smooth_sm.o)	\
	$(LIB)(uqsort.o)	\
	$(LIB)(window.o)        \
	$(LIB)(pcaneivfun.o)    \
	$(LIB)(matrix.o)        \
	$(LIB)(barrier.o)       \
	$(LIB)(regrid3.o)	\
	$(LIB)(seqm_bnd.o)      \
	$(LIB)(small_funct.o)	\
	$(LIB)(fit.o)		\
	$(LIB)(stat.o)          \
	$(LIB)(gain.o)		\
	$(LIB)(sort.o)		\
	$(LIB)(compare.o)       \
	$(LIB)(sets.o)          \
	$(LIB)(index.o)	        \
	$(LIB)(bitarray.o)      \
	$(LIB)(wrap_array.o)    \
	$(LIB)(goertzel.o)      \
	$(LIB)(timer.o)         \
	$(LIB)(nnw2di.o)
	

INSTALL:	$(LIB) $L
	@-rm -f INSTALL
	@touch $@

$(ARCH)	: $D

$(LIB)	:	$(ARCH)
	$(RANLIB) $(LIB)

remake	:
	@-rm -f $(LIB) INSTALL
	@$(MAKE) INSTALL

list	:
	$(AR) tv $(LIB)

clean::
	rm -f junk* core a.out

.PRECIOUS:	 $(LIB)

.SUFFIXES : .c .a
.c.a    :
	$(CC) -c $(CFLAGS) $<
	$(AR) $(ARFLAGS) $@ $*.o
	rm -f $*.o
