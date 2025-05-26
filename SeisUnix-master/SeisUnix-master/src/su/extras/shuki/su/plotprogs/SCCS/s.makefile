h29192
s 00020/00028/00036
d D 1.6 88/11/15 14:08:41 shuki 6 5
c 
e
s 00012/00012/00052
d D 1.5 88/06/27 12:22:19 shuki 5 4
c 
e
s 00001/00001/00063
d D 1.4 88/05/16 06:42:31 shuki 4 3
c 
e
s 00001/00001/00063
d D 1.3 88/05/03 09:25:14 shuki 3 2
c 
e
s 00021/00006/00043
d D 1.2 88/04/26 16:21:16 shuki 2 1
c /usr/local/obj
e
s 00049/00000/00000
d D 1.1 88/04/14 13:56:58 shuki 1 0
c date and time created 88/04/14 13:56:58 by shuki
e
u
U
f e 0
t
T
I 1
D 2
O	= gain.o mainplot.o
E 2
I 2
#
# makefile for su/plotprogs
#
E 2
D 6
BIN	= /usr/local/
E 6
I 6
BDIR	= /usr/local/
BIN	= $(BDIR)
E 6
I 2
L   = /usr/local/lib
D 6
O   = /usr/local/obj/
E 6
I 6
BDIR	= ../bin/
LDIR    = ./
E 6

E 2
D 6
EXE	= $(BIN)wiggle $(BIN)contour $(BIN)graph $(BIN)thplot $(BIN)max $(BIN)rows $(BIN)curve $(BIN)plas
E 6
I 6
EXE	= $(BIN)wiggle $(BIN)contour $(BIN)graph $(BIN)thplot \
	  $(BIN)max $(BIN)rows $(BIN)curve $(BIN)plas
E 6
EXE	= $(BIN)curve $(BIN)plas
I 2

E 2
D 6
#LV	= ../lplot/libplot.a
D 2
#LSU	= ../lsu/libsu.a ../lCC/libCC.a
E 2
LV	= -lcplot
E 6
I 6
LV	= ../lcplot/libcplot.a
LSU	= ../lsu/libsu.a ../lCC/libCC.a
LSU	= ../lsu/libsu.a
E 6
I 2

D 6
#LSU	= ../lsu/libsu.a ../lCC/libCC.a
E 2
D 4
LSU	= -lsu -lCC
E 4
I 4
LSU	= -lsu
E 4

E 6
I 2
CFLAGS = -g

E 2
D 6
#.SUFFIXES: .c .f .o
D 5
.c.o:	;cc -c -g $*.c
E 5
I 5
#.c.o:	;cc -c $(CFLAGS) $*.c
E 5

E 6
exe install	:$(EXE)
	@echo made

$(BIN)ftnstrip ftnstrip	:ftnstrip.o
D 5
	cc -g ftnstrip.o $(LSU) $(LV) -lm -o $(BIN)ftnstrip
E 5
I 5
	cc $(CFLAGS) ftnstrip.o $(LSU) $(LV) -lm -o $(BIN)ftnstrip
E 5
	chmod 775 $(BIN)ftnstrip
D 2
$(BIN)plas plas	:plas.o
	cc -g plas.o $(LSU) $(LV) -lm -o $(BIN)plas
E 2
I 2
D 6
$(BIN)plas plas	:$Oplas.o
D 5
	cc -g $Oplas.o $(LSU) $(LV) -lm -o $(BIN)plas
E 5
I 5
	cc $(CFLAGS) $Oplas.o $(LSU) $(LV) -lm -o $(BIN)plas
E 6
I 6
$(BIN)plas plas	:plas.o
	cc $(CFLAGS) plas.o $(LSU) $(LV) -lm -o $(BIN)plas
E 6
E 5
E 2
	chmod 775 $(BIN)plas
I 2
D 6
$Oplas.o	:plas.c
	cc -c $(CFLAGS) plas.c
	mv plas.o $O
E 2
$(BIN)disfil disfil	:disfil.o $O
E 6
I 6
$(BIN)disfil disfil	:disfil.o
E 6
D 5
	cc -g disfil.o $(LSU) -o $(BIN)disfil
#	cc -g mainplot.o disfil.o $(LSU) -o $(BIN)disfil
E 5
I 5
	cc $(CFLAGS) disfil.o $(LSU) -o $(BIN)disfil
#	cc $(CFLAGS) mainplot.o disfil.o $(LSU) -o $(BIN)disfil
E 5
	chmod 775 $(BIN)disfil
D 6
$(BIN)rows rows	:rows.o $O
E 6
I 6
$(BIN)rows rows	:rows.o
E 6
D 5
	cc -g mainplot.o rows.o $(LSU) -o $(BIN)rows
E 5
I 5
	cc $(CFLAGS) mainplot.o rows.o $(LSU) -o $(BIN)rows
E 5
	chmod 775 $(BIN)rows
D 6
$(BIN)max max	:max.o $O
E 6
I 6
$(BIN)max max	:max.o
E 6
D 5
	cc -g mainplot.o max.o $(LSU) -lm -o $(BIN)max
E 5
I 5
	cc $(CFLAGS) mainplot.o max.o $(LSU) -lm -o $(BIN)max
E 5
	chmod 775 $(BIN)max
D 2
$(BIN)curve curve	:curve.o
	cc -g curve.o $(LSU) $(LV) -lm -o $(BIN)curve
E 2
I 2
D 6
$(BIN)curve curve	:$Ocurve.o
D 5
	cc -g $Ocurve.o $(LSU) $(LV) -lm -o $(BIN)curve
E 5
I 5
	cc $(CFLAGS) $Ocurve.o $(LSU) $(LV) -lm -o $(BIN)curve
E 6
I 6
$(BIN)curve curve	:curve.o
	cc $(CFLAGS) curve.o $(LSU) $(LV) -lm -o $(BIN)curve
E 6
E 5
E 2
	chmod 775 $(BIN)curve
I 2
D 6
$Ocurve.o	:curve.c
	cc -c $(CFLAGS) curve.c
	mv curve.o $O
E 2
$(BIN)wiggle wiggle	:wiggle.o wgl1.o $O
E 6
I 6
$(BIN)wiggle wiggle	:wiggle.o wgl1.o
E 6
D 5
	cc -g mainplot.o wiggle.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)wiggle
E 5
I 5
	cc $(CFLAGS) mainplot.o wiggle.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)wiggle
E 5
	chmod 775 $(BIN)wiggle
D 6
$(BIN)contour contour	:contour.o $O
E 6
I 6
$(BIN)contour contour	:contour.o
E 6
D 5
	cc -g mainplot.o contour.o gain.o $(LSU) $(LV) -lm -o $(BIN)contour
E 5
I 5
	cc $(CFLAGS) mainplot.o contour.o gain.o $(LSU) $(LV) -lm -o $(BIN)contour
E 5
	chmod 775 $(BIN)contour
D 6
$(BIN)graph graph	:graph.o wgl1.o $O
E 6
I 6
$(BIN)graph graph	:graph.o wgl1.o
E 6
D 5
	cc -g mainplot.o graph.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)graph
E 5
I 5
	cc $(CFLAGS) mainplot.o graph.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)graph
E 5
	chmod 775 $(BIN)graph
D 6
$(BIN)thplot thplot	:thplot.o $O
E 6
I 6
$(BIN)thplot thplot	:thplot.o
E 6
D 5
	cc -g mainplot.o thplot.o gain.o $(LSU) $(LV) -lm -o $(BIN)thplot
E 5
I 5
	cc $(CFLAGS) mainplot.o thplot.o gain.o $(LSU) $(LV) -lm -o $(BIN)thplot
E 5
	chmod 775 $(BIN)thplot

clean	:
D 3
	-/bin/rm -f *.o
E 3
I 3
	-/bin/rm -f *.o $(EXE)
E 3
E 1
