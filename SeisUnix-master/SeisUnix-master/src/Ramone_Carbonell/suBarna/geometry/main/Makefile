# Makefile for ...su/main

include $(CWPROOT)/src/Makefile.config

D = $(L)/libcwp.a $(L)/libpar.a $(L)/libsu.a

LFLAGS= $(PRELFLAGS) -L$L -lsu -lpar -lcwp -lm $(POSTLFLAGS)

PROGS =			\
	$B/cdp2st    \
	$B/crkcmp    \
        $B/foldgraph	

INSTALL	:	$(PROGS)
	@-rm -f INSTALL
	@touch $@

$(PROGS):	$(CTARGET) $D 
	cc $(CFLAGS) $(@F).c $(LFLAGS) -o $@
	@chmod 755 $@
	@echo $(@F) installed in $B

remake	:
	-rm -f $(PROGS) INSTALL
	$(MAKE) 
	
clean:
	rm -f a.out junk* JUNK* core
