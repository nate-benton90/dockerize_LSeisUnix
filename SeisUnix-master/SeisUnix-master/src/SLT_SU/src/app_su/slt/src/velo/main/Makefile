
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

PROGS =                 \
   aresam               \
   asc2grid             \
   ascii2hvel           \
   ascsm                \
   csvel                \
   dt2pp                \
   hvel2vs3d            \
   hveladdxy            \
   hvelconvert          \
   hvelmod              \
   hvelsort             \
   hvelwind             \
   linefit              \
   lmkassign            \
   lmktzconv            \
   log2hl               \
   logpath              \
   logz2t               \
   plotvs3d             \
   qcvprint             \
   qcvs3d               \
   traceline2xy         \
   tzextend             \
   vavg2depth           \
   vavg2vint            \
   velf2hvel            \
   velf2velo            \
   velf2vgrid           \
   velo2velf            \
   vdisplay             \
   vgrid2hvel           \
   vgrid2sc3d           \
   vgrid2velf           \
   vgrid2vs3d           \
   vgrid2vsurf          \
   vgrid3d              \
   vgupdate             \
   vint2mw              \
   vint2vavg            \
   vint2vrms            \
   vmerge               \
   vrms2vint            \
   vs3dedit             \
   vs3dsalt             \
   vs3dshift            \
   vsmoo3d              \
   vt2vz                \
   vz2vt                \
   wgc2hvel             \
   xy2traceline         \



all: $(PROGS) 

install: $(PROGS)
	@ if [ ! -d $(SLTROOT)/$(ARCH)/bin ];     \
           then                          \
           mkdir $(SLTROOT)/$(ARCH)/bin;          \
        fi
	@for I in ${PROGS};               \
           do                            \
           echo "Installing $${I} in $(SLTROOT)/$(ARCH)/bin"; \
	   rm -f $(SLTROOT)/$(ARCH)/bin/$${I};    \
	   cp $${I} $(SLTROOT)/$(ARCH)/bin/$${I}; \
        done

clean:
	rm -f *.o $(PROGS)
