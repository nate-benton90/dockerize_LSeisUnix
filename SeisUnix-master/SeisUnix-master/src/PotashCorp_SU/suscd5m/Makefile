
PROGRAM = suscd5_m1
include ../Makefile.conf

OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
	
	$(MAKE) -f Makefile_m2
	$(MAKE) -f Makefile_ap
