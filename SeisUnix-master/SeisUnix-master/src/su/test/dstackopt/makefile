B = /usr/local/

SUBS.O = stackopt.o getitbl.o rmed.o copy.o flush.o nyqout.o \
	 tsinc.o taper.o unfrac.o dcout.o fmaxv.o \
	 apcrct_.o apstk_.o aprstk_.o apxcmx_.o

.SUFFIXES: .c .o .apc
.c.o:   ; cc -g -c $*.c
.apc.o: ; apc $*.apc -lV

$Bstackopt stackopt: ${SUBS.O} stackopt.h
	cc -g ${SUBS.O} -lFPSF -lsegy -lsep -lm -o $Bstackopt

$Bsyapply syapply: syapply.o apshft_.o clrch_.o
	cc -g syapply.o apshft_.o clrch_.o -lFPSF -lsegy -lsep -lm -o $Bsyapply

$Bstsave stsave: stsave.o
	cc -g stsave.o -lsegy -lsep -lm -o $Bstsave

$Bchtbl chtbl: chtbl.c stackopt.h
	cc -g chtbl.c -lsegy -lsep -lm -o $Bchtbl
