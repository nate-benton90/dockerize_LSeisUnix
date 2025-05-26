CFLAGS	= -g
FFLAGS	= -g

BDIR	= /usr/local/bin
LDIR    = /usr/local/lib/
BDIR	= ../bin/
LSU	= ../lsu/libsu.a

X = $(BDIR)pldb $(BDIR)tube $(BDIR)vipen $(BDIR)sunpen $(BDIR)pixpen $(BDIR)pspen
X = $(BDIR)pldb $(BDIR)tube $(BDIR)vipen $(BDIR)falpen

.SUFFIXES: .c .f .o
.c.o:	;cc -g -c $*.c

all	:$X

$(BDIR)pspen pspen	:pspen.o
	cc $(CFLAGS) mainpen.o pspen.o $(LSU) -o $(BDIR)pspen

$(BDIR)pixpen pixpen	:pixpen.o
	cc $(CFLAGS) mainpen.o pixpen.o $(LSU) -lpixrect -o $(BDIR)pixpen

$(BDIR)sunpen sunpen	:sunpen.o
	cc $(CFLAGS) mainpen.o sunpen.o $(LSU) -lcgi -lsunwindow -lpixrect -lm -o $(BDIR)sunpen

$(BDIR)falpen falpen:	falpen.o mainpen.o
	cc $(CFLAGS) mainpen.o falpen.o $(LSU) -o $(BDIR)falpen

$(BDIR)vipen vipen:	vipen.o mainpen.o
	cc $(CFLAGS) mainpen.o vipen.o $(LSU) -o $(BDIR)vipen

$(BDIR)pldb pldb:	pldb.o mainpen.o
	cc $(CFLAGS) mainpen.o pldb.o $(LSU) -o $(BDIR)pldb

$(BDIR)hppen hppen:	mainpen.o hppen.c
	cc $(CFLAGS) -c hppen.c -DHPTERM
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -lddhpterm -lsb1 -lsb2 -o $(BDIR)hppen

$(BDIR)hpen300 hpen300:	mainpen.o hppen.c
	cc $(CFLAGS) -c hppen.c -DHP300H
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -ldd300h -lsb1 -lsb2 -o $(BDIR)hpen300

$(BDIR)tube tube	:tube.csh
	cp tube.csh $(BDIR)/tube
	chmod 775 $(BDIR)/tube

clean:
	-/bin/rm -f *.o $X
