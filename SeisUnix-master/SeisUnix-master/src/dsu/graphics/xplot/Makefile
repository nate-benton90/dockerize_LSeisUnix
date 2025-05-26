# Makefile for ...su/graphics/x

include $(CWPROOT)/src/dsu/Makefile.config

#
# Source codes in
DSUMAIN = $(CWPROOT)/src/su/graphics/xplot


PROGS  =        $B/suximage     \
                $B/suxwigb      \
                $B/suxgraph     \
                $B/suxmax       \
                $B/suxmovie


INSTALL	:	$(PROGS)
	touch $@


$(PROGS):	$(CTARGET) $D 
	@echo installing $(@F) in $B
	-$(CC) -DDO_GLOBALS $(CFLAGS) $(DSUMAIN)/$(@F).c $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake	:
	-touch $(DSUMAIN)/*.c 
	-rm -f $(PROGS)
	$(MAKE) 
	
clean:
	rm -f a.out junk* JUNK* core
