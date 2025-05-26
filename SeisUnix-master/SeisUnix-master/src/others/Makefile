# Makefile for ../seismic
# Just say "make"
# Note: wximplicit and extrap are fenced off because distributed separately

include $(CWPROOT)/src/Makefile.config

LFLAGS= -L$L -lpar -lcwp -lm

PROGS =	$B/velconv $B/kaperture $B/wkbj 

SUBDIRS =	extrap wximplicit invvxz

#INSTALL:	$(PROGS) $(SUBDIRS)
INSTALL:	$(PROGS)
	@touch $@

$(PROGS):	$(CTARGET)
	$(CC) $(CFLAGS) $? $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

#$(SUBDIRS):
#	@cd $?; make

remake:
	@-touch *.c
	@-rm -f $(PROGS)
	@$(MAKE)

clean	:
	rm -f junk* core a.out *.o
