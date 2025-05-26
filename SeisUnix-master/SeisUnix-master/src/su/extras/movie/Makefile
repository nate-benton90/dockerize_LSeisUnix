
#
# Makefile for movie program
#

GP = -lsu
GLIB = -lgp
B = /usr/local/bin

OBJ = main.o prep.o movie.o button.o glob.o

#CFLAGS = -DDEBUG -g
CFLAGS = -O

install: movie
	mv movie $B/movie
	mkdir /data/tmp

movie: $(OBJ)
	cc $(CFLAGS) $(OBJ) $(GLIB) $(GP) -lm -o movie

main.o: main.c movie.h
	cc $(CFLAGS) -c main.c

prep.o: prep.c movie.h
	cc $(CFLAGS) -c prep.c

movie.o: movie.c movie.h
	cc $(CFLAGS) -c movie.c

glob.o: glob.c movie.h
	cc $(CFLAGS) -c glob.c

button.o: button.c movie.h
	cc $(CFLAGS) -c button.c
makemovie: makemovie.c
	cc $(CFLAGS) makemovie.c -o makemovie 
