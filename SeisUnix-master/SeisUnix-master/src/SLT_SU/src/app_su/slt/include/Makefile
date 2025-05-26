
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

HDRS =             \
      comva.h      \
      ghdr.h       \
      grid.h       \
      gridhd.h     \
      subc.h       \
      usgrid.h     \
      usgridhd.h   \
      usu.h        \
      velo.h       \
      wgc.h        \
      wgc4.h       \

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

