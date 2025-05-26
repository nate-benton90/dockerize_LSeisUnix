
PROGRAM = smooth1
include ../Makefile.conf


OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
