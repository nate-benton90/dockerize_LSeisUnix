

PROGRAM = su3d2par
include ../Makefile.conf

OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS)
	$(CC) $(OBJECTS) $(D) $(LIBS) -o $(EXEDIR)$@

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
