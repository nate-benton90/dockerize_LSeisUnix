OF = main.o fill.o template.o envtest.o
OF2 = generator.o compare.o breakdown.o
CFLAGS = -g
#LFLAGS = -lcurses
#LFLAGS = -lcur -lcurses
#LFLAGS = -ltermcap -lcurses_p
LFLAGS =   -lcurses -ltermcap

main.o: main.c lists.h
	cc -c $(CFLAGS) main.c

fill.o: fill.c lists.h
	cc -c $(CFLAGS) fill.c

template.o: template.c lists.h
	cc -c $(CFLAGS) template.c

envtest.o: envtest.c
	cc -c $(CFLAGS) envtest.c

compare.o: compare.c lists.h
	cc -c $(CFLAGS) compare.c

generator.o: generator.c lists.h
	cc -c $(CFLAGS) generator.c

breakdown.o: breakdown.c lists.h
	cc -c $(CFLAGS) breakdown.c

easysu: $(OF) lists.h
	cc $(CFLAGS) $(OF)  $(LFLAGS)  -o easysu

generate: $(OF2) lists.h
	cc $(CFLAGS) -o generate $(OF2)
