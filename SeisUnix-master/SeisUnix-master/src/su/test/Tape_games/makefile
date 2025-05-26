SHELL = /bin/sh
I = /usr/local/include
OPTC = O
CFLAGS= -$(OPTC) -I$I

X =	tapewrite taperead

all	:$X
	touch $@

remake	:
	-touch *.c
	make all
	
$X	:$$@.c
	-cc $(CFLAGS) $@.c -o $@
	@touch $@


# should do -pux
lint:
	@for i in $X				;\
	do				 	 \
		file=`basename $$i`.c		;\
		echo ""				;\
		echo "*****   $$file:"		;\
		lint -ux $$file			;\
	done

list:
	@ls *.c | pr -5 -t -l22 | p 

clean:
	rm -f a.out junk* JUNK* core

printall:
	@for i in *.c			;\
	do				 \
		croff $$i		;\
	done

print:	*.c
	@for i in $?			;\
	do				 \
		echo "printing $$i ..."	;\
		croff $$i		;\
	done
	touch $@
