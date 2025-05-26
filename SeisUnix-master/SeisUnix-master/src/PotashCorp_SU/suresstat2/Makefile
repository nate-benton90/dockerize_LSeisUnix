
PROGRAM = suresstat_tr_f
include ../Makefile.conf


OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
	
	$(MAKE) -f Makefile_merge
	$(MAKE) -f Makefile_suapstat_f
