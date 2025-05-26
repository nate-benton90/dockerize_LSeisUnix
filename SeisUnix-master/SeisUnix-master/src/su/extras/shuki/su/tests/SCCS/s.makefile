h13892
s 00008/00031/00054
d D 1.5 88/11/15 14:19:33 shuki 5 4
c 
e
s 00025/00019/00060
d D 1.4 88/11/06 09:54:33 shuki 4 3
c 
e
s 00001/00001/00078
d D 1.3 88/06/27 13:46:51 shuki 3 2
c decon
e
s 00006/00003/00073
d D 1.2 88/05/19 09:54:59 valer 2 1
c 
e
s 00076/00000/00000
d D 1.1 88/05/05 07:25:28 shuki 1 0
c date and time created 88/05/05 07:25:28 by shuki
e
u
U
f e 0
t
T
I 1
D 4
BIN = /usr/local/
#L = -lsegy
#L = ../lsu/libsu.a ../lCC/libCC.a
E 4
I 4
D 5
B = /usr/local/
E 4
D 2
L = -lsu -lCC
E 2
I 2
L = -lsu
E 2
LP = -lvplot
D 4
#LP = ../graphics/lib/libvplot.a
#LV = ../lvec/libvec.a
E 4
LV = -lvec
LVA = /usr/local/lib/libvec.a
D = /usr/local/obj/
E 5
I 5
B = ../bin/
L = ../lsu/libsu.a
LP = ../lvplot/libvplot.a
LV = ../lvec/libvec.a
E 5

CFLAGS	= -g
FFLAGS	= -g

.SUFFIXES: .c .r .f .o
.c.o:	;cc $(CFLAGS) -c $*.c;
.f.o:	;f77 $(FFLAGS) -c $*.f;
.r.f:	;ratfor $*.r > $*.f

D 4
junk	:$(BIN)susyn
	$(BIN)susyn >junk
E 4
I 4
junk	:$Bsusyn
	$Bsusyn >junk
E 4

D 4
$(BIN)susyn susyn	:$Dsusyn.o $Dputspk.o $Dsynt_tr.o
	$(CC) $(CFLAGS) $Dsusyn.o $Dputspk.o $Dsynt_tr.o $L $(LV) -lI77 -lF77 -lm -o $(BIN)susyn
E 4
I 4
$Bsusyn susyn	:$Dsusyn.o $Dputspk.o $Dsynt_tr.o
D 5
	$(CC) $(CFLAGS) $Dsusyn.o $Dputspk.o $Dsynt_tr.o $L $(LV) -lI77 -lF77 -lm -o $Bsusyn
E 4
$Dsusyn.o	:susyn.c
	$(CC) -c $(CFLAGS) susyn.c
	mv susyn.o $D
E 5
I 5
	$(CC) $(CFLAGS) $Dsusyn.o $Dputspk.o $Dsynt_tr.o $L $(LV) -lm -o $Bsusyn
E 5
D 2
$(BIN)susynv susynv	:$Dsusynv.o $Dputspk.o
	$(CC) $(CFLAGS) $Dsusynv.o $Dputspk.o $(LVA) $L $(LV) -lI77 -lF77 -lm -o $(BIN)susynv
E 2
I 2
D 4
$(BIN)susynv susynv	:$Dsusynv.o $Dmmphase.o
	$(CC) $(CFLAGS) $Dsusynv.o $Dmmphase.o $(LVA) $L $(LV) -lI77 -lF77 -lm -o $(BIN)susynv
E 4
I 4
$Bsusynv susynv	:$Dsusynv.o $Dmmphase.o
D 5
	$(CC) $(CFLAGS) $Dsusynv.o $Dmmphase.o $(LVA) $L $(LV) -lI77 -lF77 -lm -o $Bsusynv
E 4
E 2
$Dsusynv.o	:susynv.c
	$(CC) -c $(CFLAGS) susynv.c
	mv susynv.o $D
$Dputspk.o	:putspk.c
	$(CC) -c $(CFLAGS) putspk.c
	mv putspk.o $D
I 2
$Dmmphase.o	:mmphase.c
	$(CC) -c $(CFLAGS) mmphase.c
	mv mmphase.o $D
E 2
$Dsynt_tr.o	:synt_tr.f
	$(FC) -c $(FFLAGS) synt_tr.f
	mv synt_tr.o $D
E 5
I 5
	$(CC) $(CFLAGS) $Dsusynv.o $Dmmphase.o $(LVA) $L $(LV) -lm -o $Bsusynv
E 5
D 4
$(BIN)susycdf  susycdf	:$Dsusycdf.o $Dputspk.o
	$(CC) $(CFLAGS) $Dsusycdf.o $Dputspk.o $(LVA) $L $(LV) -lI77 -lF77 -lm -o $(BIN)susycdf
E 4
I 4
$Bsusycdf  susycdf	:$Dsusycdf.o $Dputspk.o
D 5
	$(CC) $(CFLAGS) $Dsusycdf.o $Dputspk.o $(LVA) $L $(LV) -lI77 -lF77 -lm -o $Bsusycdf
E 4
$Dsusycdf.o	:susycdf.c
	$(CC) -c $(CFLAGS) susycdf.c
	mv susycdf.o $D

D 4
#$(CC) susyn.o putspk.o synt_tr.o $L $(LV) -lI77 -lF77 -lPW -lm -o $(BIN)susyn
E 4
I 4
#$(CC) susyn.o putspk.o synt_tr.o $L $(LV) -lI77 -lF77 -lPW -lm -o $Bsusyn
E 4

E 5
I 5
	$(CC) $(CFLAGS) $Dsusycdf.o $Dputspk.o $(LVA) $L $(LV) -lm -o $Bsusycdf
E 5
D 4
$(BIN)suspike suspike	:suspike.o
	$(CC) suspike.o $L $(LV) -lPW -lm -o $(BIN)suspike
E 4
I 4
$Bsuspike suspike	:suspike.o
	$(CC) suspike.o $L $(LV)  -lm -o $Bsuspike
E 4

synt_tr.o	:synt_tr.f
	f77 -c synt_tr.f
		
sutrseq	:$Dsutrseq.o trseq.o
	cc $Dsutrseq.o trseq.o $L -o sutrseq

D 4
$(BIN)sus sus	:sus.o
	cc $(CFLAGS) sus.o $L -lm -o $(BIN)sus
E 4
I 4
$Bsus sus	:sus.o
	cc $(CFLAGS) sus.o $L -lm -o $Bsus
E 4

remake	:
	rm -f *.o
	make all
	
clean	:
D 3
	-/bin/rm -f *.o j*
E 3
I 3
	-/bin/rm -f j*
E 3

D 4
dmotest	:$(BIN)suspike
E 4
I 4
dmotest	:$Bsuspike
E 4
	suspike x0=0,0,0,30 ntr=40 t0=10,25,40,40 nt=50	|\
	suwig						|\
	hpen300 
	@sleep 10
	suspike x0=0,0,0,30 ntr=40 t0=10,25,40,40 nt=50	|\
	sushw key=offset a=20				|\
	suchw key2=tracl				|\
	sudmo cospike					|\
	suwig						|\
	hpen300 
I 4



getpar	:$Btgetpar
	$Btgetpar i=-10 l=2 u=3 h=4 f=5e-1 z=6e+1 s="s p a c e" b=yes v=-7,-8 i=1
	echo i=8 l=2 >PARFILE
	$Btgetpar i=1 par=PARFILE
	$Btgetpar PARFILE i=1
$Btgetpar tgetpar	:tgetpar.c /usr/local/lib/libsu.a
D 5
	cc $(CFLAGS) tgetpar.c -lsu -o $Btgetpar
E 5
I 5
	cc $(CFLAGS) tgetpar.c $L -o $Btgetpar
E 5
E 4
E 1
