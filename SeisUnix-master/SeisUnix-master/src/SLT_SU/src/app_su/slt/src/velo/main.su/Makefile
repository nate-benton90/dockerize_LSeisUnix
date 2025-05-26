# Makefile for ...par/main
# Just say "make"

include $(CWPROOT)/src/Makefile.config

D = $I/cwp.h $I/par.h $L/libcwp.a $L/libpar.a

LFLAGS= $(PRELFLAGS) -L$L -lpar -lcwp -lm $(POSTLFLAGS)

PROGS =			\
$B/a2vs3d		\
$B/aresam		\
$B/asc2grid		\
$B/ascii2hvel		\
$B/ascsm		\
$B/csvel		\
$B/dt2pp		\
$B/fubar		\
$B/hvel2vs3d		\
$B/hveladdxy		\
$B/hvelconvert		\
$B/hvelmod		\
$B/hvelsort		\
$B/hvelwind		\
$B/linefit		\
$B/lmkassign		\
$B/lmktzconv		\
$B/log2hl		\
$B/logpath		\
$B/logz2t		\
$B/plotvs3d		\
$B/qcvprint		\
$B/qcvs3d		\
$B/traceline2xy		\
$B/tzextend		\
$B/vavg2depth		\
$B/vavg2vint		\
$B/vdisplay		\
$B/veff2vs3d		\
$B/velf2hvel		\
$B/velf2velo		\
$B/velf2vgrid		\
$B/velf2vs3d		\
$B/velo2velf		\
$B/vgrid2hfile		\
$B/vgrid2hvel		\
$B/vgrid2sc3d		\
$B/vgrid2velf		\
$B/vgrid2vs3d		\
$B/vgrid2vsurf		\
$B/vgrid3d		\
$B/vgupdate		\
$B/vint2mw		\
$B/vint2vavg		\
$B/vint2vrms		\
$B/vmerge		\
$B/vrms2vint		\
$B/vs3dedit		\
$B/vs3dfilter		\
$B/vs3dsalt		\
$B/vs3dshift		\
$B/vsmoo3d		\
$B/vt2vz		\
$B/vz2vt		\
$B/wgc2hvel		\
$B/xy2traceline

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
