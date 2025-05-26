
PROGRAM = sugprscl
include ../Makefile.conf

OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) -g $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) -g $(INC) $(PROGRAM).c
