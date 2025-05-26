
PROGRAM = check_xindexf
CWPDIR = /home/nemeth/prog/src/su-33
L=$(CWPDIR)/lib
ML=/home/nemeth/prog/lib
EXEDIR = ./



CFLAGS = -c -O2 -m486 -DLITTLE -DCWP_LITTLE_ENDIAN -Wall
CC = gcc 
D = $L/libcwp.a $L/libpar.a $L/libsu.a
LIBS =  -L$(ML) -lsulib -L$(CWPDIR)/lib -lsu -lpar -lcwp -lpthread -lm -lssl
INC = -I$(CWPDIR)/include -I../../include


OBJECTS = $(PROGRAM).o 


$(PROGRAM): $(OBJECTS) $D
	$(CC) $(OBJECTS) $(LIBS) -o $(EXEDIR)$(PROGRAM)

$(PROGRAM).o: $(PROGRAM).c
	$(CC) $(CFLAGS) $(INC) $(PROGRAM).c
