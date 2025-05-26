# Makefile for ebcdic-ieee conversion
# just type "make"
#
# Adapt the flags in the following paragraph to your system

ROOT = /files2/data5/zli/lf/cwp
UROOT = /files2/data5/zli/lf/slt

OPTC =
I = $(ROOT)/include 
K = $(UROOT)/include 

# Search on the word "Comment" to complete adaptation

CFLAGS= $(OPTC) -I$K -I$I

# Comment out if your make knows about C libs
.c.a:
	cc -c $(CFLAGS) $<
	ar rv $@ $*.o
	/bin/rm -f $*.o

.f.a:
	$(FC) -c $(CFLAGS) $<
	ar rv $@ $*.o
	/bin/rm -f $*.o

.PRECIOUS:	 $(LIB)

LIB = $(UROOT)/lib/libwgc.a

ARCH =				\
	$(LIB)(ebcasc.o)	\
	$(LIB)(wgc2sgy.o)	

INSTALL:	$(LIB)
	@touch $@

$(ARCH)	: $D

$(LIB)	:	$(ARCH)
	ranlib $(LIB)

remake	:
	@/bin/rm -f $(LIB)
	@make

list	:
	ar tv $(LIB)

clean:
	/bin/rm -f junk* core a.out
