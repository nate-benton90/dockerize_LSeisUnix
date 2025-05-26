B = ../bin/
L = ../lsu/libsu.a
LP = ../lvplot/libvplot.a
LV = ../lvec/libvec.a

CFLAGS	= -g
FFLAGS	= -g

.SUFFIXES: .c .r .f .o
.c.o:	;cc $(CFLAGS) -c $*.c;
.f.o:	;f77 $(FFLAGS) -c $*.f;
.r.f:	;ratfor $*.r > $*.f

junk	:$Bsusyn
	$Bsusyn >junk

$Bsusyn susyn	:$Dsusyn.o $Dputspk.o $Dsynt_tr.o
	$(CC) $(CFLAGS) $Dsusyn.o $Dputspk.o $Dsynt_tr.o $L $(LV) -lm -o $Bsusyn
$Bsusynv susynv	:$Dsusynv.o $Dmmphase.o
	$(CC) $(CFLAGS) $Dsusynv.o $Dmmphase.o $(LVA) $L $(LV) -lm -o $Bsusynv
$Bsusycdf  susycdf	:$Dsusycdf.o $Dputspk.o
	$(CC) $(CFLAGS) $Dsusycdf.o $Dputspk.o $(LVA) $L $(LV) -lm -o $Bsusycdf
$Bsuspike suspike	:suspike.o
	$(CC) suspike.o $L $(LV)  -lm -o $Bsuspike

synt_tr.o	:synt_tr.f
	f77 -c synt_tr.f
		
sutrseq	:$Dsutrseq.o trseq.o
	cc $Dsutrseq.o trseq.o $L -o sutrseq

$Bsus sus	:sus.o
	cc $(CFLAGS) sus.o $L -lm -o $Bsus

remake	:
	rm -f *.o
	make all
	
clean	:
	-/bin/rm -f j*

dmotest	:$Bsuspike
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



getpar	:$Btgetpar
	$Btgetpar i=-10 l=2 u=3 h=4 f=5e-1 z=6e+1 s="s p a c e" b=yes v=-7,-8 i=1
	echo i=8 l=2 >PARFILE
	$Btgetpar i=1 par=PARFILE
	$Btgetpar PARFILE i=1
$Btgetpar tgetpar	:tgetpar.c /usr/local/lib/libsu.a
	cc $(CFLAGS) tgetpar.c $L -o $Btgetpar
