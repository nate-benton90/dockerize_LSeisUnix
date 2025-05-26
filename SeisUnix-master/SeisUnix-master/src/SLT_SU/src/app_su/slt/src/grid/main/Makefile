
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

PROGS =             \
   grid2curve       \
   grid2hg          \
   grid2horz        \
   grid2lmk         \
   grid2su          \
   grid3dto2d       \
   grid4d           \
   gridatlayer      \
   gridclean        \
   gridclip         \
   griddeform       \
   gridderi         \
   gridfilter       \
   gridheader       \
   gridmerge        \
   gridop2          \
   gridrotate       \
   gridrvse         \
   grids3           \
   gridsalt         \
   gridsalthrz      \
   gridsc3d         \
   gridsediment     \
   gridsedrate      \
   gridshift        \
   gridslice        \
   gridstack        \
   gridsubs         \
   gridthickness    \
   gridtrsp         \
   gridtzconv       \
   gridvelo         \
   gridwind         \
   lmk2grid         \
   lmkwind          \
   regrid3d         \




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

lint:
	for I in $(PROGS);      \
           do                   \
        lint -errhdr=%all $(CFLAGS) $${I}.c; \
        done

