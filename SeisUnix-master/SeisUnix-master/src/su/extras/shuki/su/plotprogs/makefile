#
# makefile for su/plotprogs
#
BDIR	= /usr/local/bin
BIN	= $(BDIR)
L   = /usr/local/lib
BDIR	= ../bin/
LDIR    = ./

EXE	= $(BIN)wiggle $(BIN)contour $(BIN)graph $(BIN)thplot \
	  $(BIN)max $(BIN)rows $(BIN)curve $(BIN)plas
EXE	= $(BIN)curve $(BIN)plas

LV	= ../lcplot/libcplot.a
LSU	= ../lsu/libsu.a ../lCC/libCC.a
LSU	= ../lsu/libsu.a

CFLAGS = -g

exe install	:$(EXE)
	@echo made

$(BIN)ftnstrip ftnstrip	:ftnstrip.o
	cc $(CFLAGS) ftnstrip.o $(LSU) $(LV) -lm -o $(BIN)ftnstrip
	chmod 775 $(BIN)ftnstrip
$(BIN)plas plas	:plas.o
	cc $(CFLAGS) plas.o $(LSU) $(LV) -lm -o $(BIN)plas
	chmod 775 $(BIN)plas
$(BIN)disfil disfil	:disfil.o
	cc $(CFLAGS) disfil.o $(LSU) -o $(BIN)disfil
#	cc $(CFLAGS) mainplot.o disfil.o $(LSU) -o $(BIN)disfil
	chmod 775 $(BIN)disfil
$(BIN)rows rows	:rows.o
	cc $(CFLAGS) mainplot.o rows.o $(LSU) -o $(BIN)rows
	chmod 775 $(BIN)rows
$(BIN)max max	:max.o
	cc $(CFLAGS) mainplot.o max.o $(LSU) -lm -o $(BIN)max
	chmod 775 $(BIN)max
$(BIN)curve curve	:curve.o
	cc $(CFLAGS) curve.o $(LSU) $(LV) -lm -o $(BIN)curve
	chmod 775 $(BIN)curve
$(BIN)wiggle wiggle	:wiggle.o wgl1.o
	cc $(CFLAGS) mainplot.o wiggle.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)wiggle
	chmod 775 $(BIN)wiggle
$(BIN)contour contour	:contour.o
	cc $(CFLAGS) mainplot.o contour.o gain.o $(LSU) $(LV) -lm -o $(BIN)contour
	chmod 775 $(BIN)contour
$(BIN)graph graph	:graph.o wgl1.o
	cc $(CFLAGS) mainplot.o graph.o wgl1.o gain.o $(LSU) $(LV) -lm -o $(BIN)graph
	chmod 775 $(BIN)graph
$(BIN)thplot thplot	:thplot.o
	cc $(CFLAGS) mainplot.o thplot.o gain.o $(LSU) $(LV) -lm -o $(BIN)thplot
	chmod 775 $(BIN)thplot

clean	:
	-/bin/rm -f *.o $(EXE)
