# Makefile for ...par/main
# Just say "make"

include $(CWPROOT)/src/Makefile.config

D = $I/cwp.h $I/par.h $L/libcwp.a $L/libpar.a

LFLAGS= $(PRELFLAGS) -L$L -lpar -lcwp -lm $(POSTLFLAGS)

PROGS =			\
	$B/a2b		\
	$B/a2i		\
	$B/b2a		\
	$B/farith	\
	$B/float2ibm	\
	$B/ftnstrip	\
	$B/ftnunstrip	\
	$B/h2b		\
	$B/i2a		\
	$B/ibm2float	\
	$B/recast	\
	$B/swapbytes	\
	$B/utmconv

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
