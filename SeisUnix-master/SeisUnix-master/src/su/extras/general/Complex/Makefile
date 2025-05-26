# Makefile for ...gen/Complex
SHELL = /bin/sh

# Adapt the flags in the following paragraph to your system
ROOT = /usr/local
OPTC = -O
# Search on the word "Comment" to complete adaptation

B = $(ROOT)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L

LIB	= $L/libcomplex.a
ARCH	= $(LIB)(complex.o)
INCLUDE = $I/complex.h

INSTALL:	$(ARCH) $(INCLUDE)
	@touch $@

$(LIB)	:	$(ARCH)
	ranlib $(LIB)

$(INCLUDE):	$$(@F)
	@cp $? $@
	@$(ICHMODLINE)
	@echo $(@F) installed in $I

remake	:
	@rm -f $(LIB) $(INCLUDE)
	@make

list	:
	ar tv $(LIB)

clean	:
	rm -f junk* core a.out *.o
