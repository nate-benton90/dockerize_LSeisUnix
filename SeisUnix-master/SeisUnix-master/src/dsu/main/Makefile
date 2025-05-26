# Makefile for ...su/main

include $(CWPROOT)/src/dsu/Makefile.config


PROGS =			\
        $B/bhedtopar    \
        $B/dt1tosu      \
	$B/segyclean	\
	$B/segyhdrs	\
	$B/segyread	\
	$B/segywrite	\
	$B/setbhed	\
	$B/suabshw	\
	$B/suacor	\
	$B/suaddhead	\
	$B/suaddnoise	\
	$B/suamp	\
	$B/suascii	\
	$B/subfilt	\
	$B/suchart	\
	$B/su3dchart	\
	$B/suchw	\
	$B/suconv	\
	$B/sudipfilt	\
	$B/sudivcor	\
	$B/sudivstack	\
	$B/sudmofk	\
	$B/sudmotx	\
	$B/suedit	\
	$B/sufdmod2	\
	$B/sufft	\
	$B/sufilter	\
	$B/suflip	\
	$B/sufrac	\
	$B/sugabor	\
	$B/sugain	\
	$B/sugazmig	\
	$B/suget	\
	$B/sugethw	\
	$B/suhilb	\
	$B/suifft	\
	$B/suilog	\
	$B/suimp2d	\
	$B/suimp3d	\
	$B/suinterp	\
	$B/sukdmig2d	\
	$B/sukill	\
	$B/sulog	\
	$B/sumax	\
	$B/sumigtk	\
	$B/sumigps	\
	$B/sumute	\
	$B/sunmo	\
	$B/sunull	\
	$B/suop		\
	$B/suop2	\
	$B/supack1	\
	$B/supack2	\
	$B/supaste	\
	$B/supef	\
	$B/supgc	\
	$B/suplane	\
	$B/suput	\
	$B/suradon	\
	$B/surange	\
	$B/suramp	\
	$B/surecip	\
	$B/surelan	\
	$B/suresamp	\
	$B/suresstat	\
	$B/sureduce	\
	$B/sushape	\
	$B/suspecfk	\
	$B/suspecfx	\
	$B/suspeck1k2	\
	$B/sushw	\
	$B/susort	\
	$B/suspike	\
	$B/sustack	\
	$B/sustatic	\
	$B/sustolt	\
	$B/sustrip	\
	$B/suswapbytes	\
	$B/susyncz	\
	$B/susynlv	\
	$B/susynlvcw	\
	$B/susynvxz	\
	$B/susynvxzcs	\
	$B/sutab	\
	$B/sutaper	\
	$B/sutsq	\
	$B/suttoz	\
	$B/suunpack1	\
	$B/suunpack2	\
	$B/suvcat	\
	$B/suvelan	\
	$B/suvibro	\
	$B/sutvband	\
	$B/suwind	\
	$B/suxedit	\
	$B/suxcor	\
	$B/suzero


INSTALL	:	$(PROGS)
	touch $@


$(PROGS):	$(CTARGET) $D 
	-$(CC) -DDO_GLOBALS $(CFLAGS) $(DSUMAIN)/$(@F).c $(LFLAGS) -o $@
	@$(MCHMODLINE)
	@echo $(@F) installed in $B

remake	:
	-touch $(DSUMAIN)/*.c 
	-rm -f $(PROGS)
	$(MAKE) 
	
clean:
	rm -f a.out junk* JUNK* core
