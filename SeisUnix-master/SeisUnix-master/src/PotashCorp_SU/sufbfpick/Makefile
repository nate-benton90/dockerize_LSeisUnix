
PROGRAM = sufbfpick
include ../Makefile.conf


OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) -g $(CFLAGS) $(INC) $(PROGRAM).c
