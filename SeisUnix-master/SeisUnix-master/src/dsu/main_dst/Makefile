# Makefile for PVM based applications

include $(CWPROOT)/src/dsu/Makefile.config

#
# Source codes in
DSUMAIN = .
RE = -Iinclude

PROGS =			\
        $B/sudrefmaster    \
        $B/sudrefslave
#$B/SeisMig2D    \
#$B/SeisPipe2D


INSTALL	:	$(PROGS)
	touch $@
	rm *.o


#$(PROGS):	$(CTARGET) $D  SeisTable tools
$(PROGS):	$(CTARGET) $D  tools
	-$(CC) -DDO_GLOBALS $(RE) $(CFLAGS) $(DSUMAIN)/$(@F).c tools.o $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake	:
	-touch *.c 
	-rm -f $(PROGS)
	$(MAKE) 
	
clean:
	rm -f a.out junk* JUNK* core

SeisTable: 
	$(CC) $(CFLAGS) -c SeisTable.c

tools: 
	$(CC) $(RE) $(CFLAGS) -c tools.c

transpose: 	transpose.o 
	$(CC) -o transpose $(CFLAGS) transpose.o -L$(CWPROOT)/lib -lsu -lpar -lcwp -lm
