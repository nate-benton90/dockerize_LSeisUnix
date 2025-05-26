CFLAGS = -g

hdrs.h	:mkhdrs
	mkhdrs > hdrs.h

mkhdrs	:mkhdrs.c su.h trhdr.h bhdr.h
	cc $(CFLAGS) mkhdrs.c -o mkhdrs

bhdr.h	:su.h mkbhdr.sh
	sh mkbhdr.sh > bhdr.h

trhdr.h	:su.h mktrhdr.sh
	sh mktrhdr.sh > trhdr.h

clean	:
	-/bin/rm -f mkhdrs *.o hdrs.h bhdr.h trhdr.h
