
PROGRAM = su3dmigpspi_mpi
include ../Makefile.conf-mpi

OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $(D)
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
