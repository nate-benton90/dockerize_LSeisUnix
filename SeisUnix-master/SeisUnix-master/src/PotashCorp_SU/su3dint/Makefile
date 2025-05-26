

PROGRAM = su3dint_nn
include ../Makefile.conf


OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(D) $(LIBS) -o $(EXEDIR)$@

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
