
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

HDRS =              \
      bhdr.h        \
      cwp.h         \
      garnish.h     \
      hdr.h         \
      header.h      \
      par.h         \
      picking.h     \
      segy.h        \
      su.h          \
      suport.h      \
      xplot.h       \
      xwindow.h     \
	
all: $(HDRS)
	@ if [ ! -d $(TOP)/include ]; \
           then \
           mkdir $(TOP)/include;\
        fi
	rm -f $(TOP)/include/*.h
	cp $(HDRS) $(TOP)/include

	
install: $(HDRS)
	@ if [ ! -d $(SLTROOT)/$(ARCH)/include ]; \
           then \
           mkdir $(SLTROOT)/$(ARCH)/include;\
        fi
	(cd $(SLTROOT)/$(ARCH)/include; rm -f $(HDRS) )
	cp $(HDRS) $(SLTROOT)/$(ARCH)/include

