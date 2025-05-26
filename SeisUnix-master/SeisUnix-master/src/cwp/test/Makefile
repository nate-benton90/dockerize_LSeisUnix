# Makefile for ...cwp/test
# Just say "make"
# NOTE: These guys are NOT put in /usr/local/bin!

SHELL = /bin/sh

TESTDIR = .

# Adapt the flags in the following paragraph to your system
ROOT = $(CWPROOT)
OPTC = -O

B = $(TESTDIR)/bin
I = $(ROOT)/include
L = $(ROOT)/lib

CFLAGS= $(OPTC) -I$I
LFLAGS= -L$L -lcwp -lm
LINTFLAGS= -I$I
TARGETFLAG= $$(@F)

PROGS =			\
	$B/abelt	\
	$B/airyapt	\
	$B/airyat	\
	$B/airybpt	\
	$B/airybt	\
	$B/alloct	\
	$B/antialiast	\
	$B/bft	\
	$B/blast	\
	$B/bmt	\
	$B/convt	\
	$B/closetot	\
	$B/csplint	\
	$B/dget	\
	$B/filestatt	\
	$B/frannort	\
	$B/franunit	\
	$B/hankelt	\
	$B/hilbertt	\
	$B/hpsortt	\
	$B/intcubt	\
	$B/intcubt2	\
	$B/intl2bt	\
	$B/intlint	\
	$B/intlint2	\
	$B/ints8ct	\
	$B/ints8ct2	\
	$B/ints8rt	\
	$B/ints8rt2	\
	$B/macrot	\
	$B/mkdifft	\
	$B/mkhdifft	\
	$B/mksinct	\
	$B/npfaot	\
	$B/npfat	\
	$B/pfa2b	\
	$B/pfa2cct	\
	$B/pfa2rct	\
	$B/pfab	\
	$B/pfacct	\
	$B/pfamcct	\
	$B/pfarct	\
	$B/pp1dt	\
	$B/qkisortt	\
	$B/qksortt	\
	$B/questt	\
	$B/ress8rt	\
	$B/rkstept	\
	$B/sect	\
	$B/sget	\
	$B/shfs8rt	\
	$B/shfs8rt2	\
	$B/sqrt	\
	$B/sqrt2	\
	$B/tridift	\
	$B/xcort	\
	$B/xindext	\
	$B/yxtoxyt	\
	$B/zeronrbt


INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$(TARGETFLAG).c
	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake:
	@$(MAKE)

clean	:
	rm -f junk* core a.out *.o
