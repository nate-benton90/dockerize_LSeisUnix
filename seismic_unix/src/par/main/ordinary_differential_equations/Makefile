# Makefile for ...par/main
# Just say "make"

include $(CWPROOT)/src/Makefile.config

D = $I/cwp.h $I/par.h $L/libcwp.a $L/libpar.a

LFLAGS= $(PRELFLAGS) -L$L -lpar -lcwp -lm $(POSTLFLAGS)

PROGS =			\
	$B/logisticfit	\
	$B/lorenz	\
	$B/rossler	\
	$B/sir_epidemic	\
	$B/sird_epidemic	\
	$B/seir_epidemic	\
	$B/verhulst	\
	$B/voltlotka

INSTALL:	$(PROGS)
	@-rm -f INSTALL
	@touch $@

$(PROGS):	$(CTARGET)
	$(CC) $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	$(MCHMODLINE)
	@echo $(@F) installed in $B

remake:
	@-rm -f $(PROGS) INSTALL
	@$(MAKE) INSTALL

clean	::
	rm -f junk* core a.out *.o
