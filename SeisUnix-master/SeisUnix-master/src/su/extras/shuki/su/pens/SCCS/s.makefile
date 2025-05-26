h57846
s 00027/00042/00021
d D 1.8 88/11/15 14:07:38 shuki 10 9
c 
e
s 00007/00001/00056
d D 1.7 88/08/08 09:32:32 tamar 9 6
c pspen
e
s 00001/00001/00062
d R 1.8 88/08/08 09:16:49 tamar 8 7
c pspen
e
s 00007/00001/00056
d R 1.7 88/08/04 14:35:49 tamar 7 6
c add pspen
e
s 00006/00000/00051
d D 1.6 88/08/03 15:08:23 rafi 6 5
c pixpen
e
s 00001/00001/00050
d D 1.5 88/05/16 06:42:12 shuki 5 4
c 
e
s 00004/00002/00047
d D 1.4 88/05/03 09:26:22 shuki 4 3
c 
e
s 00026/00011/00023
d D 1.3 88/04/26 16:25:51 shuki 3 2
c /usr/local/obj
e
s 00004/00001/00030
d D 1.2 88/04/19 15:47:08 shuki 2 1
c 
e
s 00031/00000/00000
d D 1.1 88/04/14 13:59:34 shuki 1 0
c date and time created 88/04/14 13:59:34 by shuki
e
u
U
f e 0
t
T
I 1
CFLAGS	= -g
I 10
FFLAGS	= -g
E 10

D 10
BIN	= /usr/local/
#LSU	= /users/shuki/su/lsu/libsu.a ../lCC/libCC.a
D 5
LSU	= -lsu -lCC
E 5
I 5
LSU	= -lsu
E 10
I 10
BDIR	= /usr/local/
LDIR    = /usr/local/lib/
BDIR	= ../bin/
LSU	= ../lsu/libsu.a
E 10
E 5

I 3
D 10
O = /usr/local/obj/
E 10
I 10
X = $(BDIR)pldb $(BDIR)tube $(BDIR)vipen $(BDIR)sunpen $(BDIR)pixpen $(BDIR)pspen
X = $(BDIR)pldb $(BDIR)tube $(BDIR)vipen $(BDIR)falpen
E 10

I 4
D 9
X = $(BIN)pldb $(BIN)tube $(BIN)vipen $(BIN)sunpen
E 9
I 9
D 10
X = $(BIN)pldb $(BIN)tube $(BIN)vipen $(BIN)sunpen $(BIN)pixpen $(BIN)pspen
E 9

E 10
E 4
E 3
.SUFFIXES: .c .f .o
.c.o:	;cc -g -c $*.c

D 2
all	:$(BIN)pldb $(BIN)tube $(BIN)vipen
E 2
I 2
D 4
all	:$(BIN)pldb $(BIN)tube $(BIN)vipen $(BIN)sunpen
E 4
I 4
all	:$X
I 9

D 10
$(BIN)pspen pspen	:$Opspen.o
	cc $(CFLAGS) $Omainpen.o $Opspen.o $(LSU) -o $(BIN)pspen
$Opspen.o	:pspen.c
	cc $(CFLAGS) -c pspen.c
	mv pspen.o $O
E 10
I 10
$(BDIR)pspen pspen	:pspen.o
	cc $(CFLAGS) mainpen.o pspen.o $(LSU) -o $(BDIR)pspen
E 10
E 9
E 4

I 6
D 10
$(BIN)pixpen pixpen	:$Opixpen.o
	cc $(CFLAGS) $Omainpen.o $Opixpen.o $(LSU) -lpixrect -o $(BIN)pixpen
$Opixpen.o	:pixpen.c
	cc $(CFLAGS) -c pixpen.c
	mv pixpen.o $O
E 10
I 10
$(BDIR)pixpen pixpen	:pixpen.o
	cc $(CFLAGS) mainpen.o pixpen.o $(LSU) -lpixrect -o $(BDIR)pixpen
E 10

E 6
D 3
$(BIN)sunpen sunpen	:sunpen.o
	cc $(CFLAGS) mainpen.o sunpen.o $(LSU) -lcgi -lsunwindow -lpixrect -lm -o $(BIN)sunpen
E 3
I 3
D 10
$(BIN)sunpen sunpen	:$Osunpen.o
	cc $(CFLAGS) $Omainpen.o $Osunpen.o $(LSU) -lcgi -lsunwindow -lpixrect -lm -o $(BIN)sunpen
$Osunpen.o	:sunpen.c
	cc $(CFLAGS) -c sunpen.c
	mv sunpen.o $O
E 10
I 10
$(BDIR)sunpen sunpen	:sunpen.o
	cc $(CFLAGS) mainpen.o sunpen.o $(LSU) -lcgi -lsunwindow -lpixrect -lm -o $(BDIR)sunpen
E 10
E 3
E 2

D 3
$(BIN)vipen vipen:	vipen.o mainpen.o
	cc $(CFLAGS) mainpen.o vipen.o $(LSU) -o $(BIN)vipen
E 3
I 3
D 10
$(BIN)vipen vipen:	$Ovipen.o $Omainpen.o
	cc $(CFLAGS) $Omainpen.o $Ovipen.o $(LSU) -o $(BIN)vipen
$Ovipen.o	:vipen.c
	cc $(CFLAGS) -c vipen.c
	mv vipen.o $O
E 10
I 10
$(BDIR)falpen falpen:	falpen.o mainpen.o
	cc $(CFLAGS) mainpen.o falpen.o $(LSU) -o $(BDIR)falpen
E 10
E 3

D 3
$(BIN)pldb pldb:	pldb.o mainpen.o
	cc $(CFLAGS) mainpen.o pldb.o $(LSU) -o $(BIN)pldb
E 3
I 3
D 10
$(BIN)pldb pldb:	$Opldb.o $Omainpen.o
	cc $(CFLAGS) $Omainpen.o $Opldb.o $(LSU) -o $(BIN)pldb
$Opldb.o	:pldb.c
	cc $(CFLAGS) -c pldb.c
	mv pldb.o $O
E 10
I 10
$(BDIR)vipen vipen:	vipen.o mainpen.o
	cc $(CFLAGS) mainpen.o vipen.o $(LSU) -o $(BDIR)vipen
E 10
E 3

D 3
$(BIN)hppen hppen:	mainpen.o hppen.c
E 3
I 3
D 10
$Omainpen.o	:mainpen.c
	cc $(CFLAGS) -c mainpen.c
	mv mainpen.o $O
E 10
I 10
$(BDIR)pldb pldb:	pldb.o mainpen.o
	cc $(CFLAGS) mainpen.o pldb.o $(LSU) -o $(BDIR)pldb
E 10

D 10
$(BIN)hppen hppen:	$Omainpen.o $Ohppen.c
E 10
I 10
$(BDIR)hppen hppen:	mainpen.o hppen.c
E 10
E 3
	cc $(CFLAGS) -c hppen.c -DHPTERM
D 3
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -lddhpterm -lsb1 -lsb2 -o $(BIN)hppen
E 3
I 3
D 10
	cc $(CFLAGS) $Omainpen.o $Ohppen.o $(LSU) -lddhpterm -lsb1 -lsb2 -o $(BIN)hppen
E 10
I 10
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -lddhpterm -lsb1 -lsb2 -o $(BDIR)hppen
E 10
E 3

D 3
$(BIN)hpen300 hpen300:	mainpen.o hppen.c
	cc $(CFLAGS) -c hppen.c -DHP300H
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -ldd300h -lsb1 -lsb2 -o $(BIN)hpen300
E 3
I 3
D 10
$(BIN)hpen300 hpen300:	$Omainpen.o $Ohppen.c
	cc $(CFLAGS) -c $Ohppen.c -DHP300H
	cc $(CFLAGS) $Omainpen.o $Ohppen.o $(LSU) -ldd300h -lsb1 -lsb2 -o $(BIN)hpen300
E 10
I 10
$(BDIR)hpen300 hpen300:	mainpen.o hppen.c
	cc $(CFLAGS) -c hppen.c -DHP300H
	cc $(CFLAGS) mainpen.o hppen.o $(LSU) -ldd300h -lsb1 -lsb2 -o $(BDIR)hpen300
E 10
E 3

D 10
$(BIN)tube tube	:tube.csh
	cp tube.csh $(BIN)/tube
	chmod 775 $(BIN)/tube
E 10
I 10
$(BDIR)tube tube	:tube.csh
	cp tube.csh $(BDIR)/tube
	chmod 775 $(BDIR)/tube
E 10

clean:
D 4
	-/bin/rm -f *.o
E 4
I 4
	-/bin/rm -f *.o $X
E 4
E 1
