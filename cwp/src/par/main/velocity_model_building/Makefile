# Makefile for ...par/main
# Just say "make"

include $(CWPROOT)/src/Makefile.config

D = $I/cwp.h $I/par.h $L/libcwp.a $L/libpar.a

LFLAGS= $(PRELFLAGS) -L$L -lpar -lcwp -lm $(POSTLFLAGS)

PROGS =			\
	$B/addrvl3d	\
	$B/makevel	\
	$B/randvel3d	\
	$B/stiff2vel	\
	$B/unif2	\
	$B/unif2aniso	\
	$B/unif2ti2	\
	$B/vel2stiff	\
	$B/velconv	\
	$B/vtlvz

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
