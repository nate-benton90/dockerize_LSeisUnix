
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

LIBS =                                    \
     cwp/src/cwp/lib                      \
     cwp/src/par/lib                      \
     cwp/src/su/lib                       \
     cwp/src/xplot/lib                    \
     slt/src/grid/lib                     \
     slt/src/su/lib                       \
     slt/src/velo/lib                     \
     slt/src/wgc/lib                      \
     slt/src/fxymig/lib                   \
     slt/src/ktmig/lib                    \
     slt/src/kzmig/lib                    \
     slt/src/rayt3d/lib                   \
     slt/src/umisc/lib                    \

BINS =                                    \
     slt/src/grid/main                    \
     slt/src/par/main                     \
     slt/src/pc/main                      \
     slt/src/su/main                      \
     slt/src/velo/main                    \
     slt/src/fxymig/main                  \
     slt/src/ktmig/main                   \
     slt/src/kzmig/main                   \
     slt/src/rayt3d/main                  \

#     slt/src/compress/main                \
      
HDRS =                                    \
     cwp/include                          \
     slt/include                          \

all: libs bins

libs: $(LIBS)
	for I in $(LIBS);                 \
           do                             \
           cd $${I};                      \
           make;                          \
           cd $(SLTROOT)/src/app_su;       \
        done

bins: $(BINS)
	for I in $(BINS);                 \
           do                             \
           cd $${I};                      \
           make;                          \
           cd $(SLTROOT)/src/app_su;       \
        done

clean: 
	for I in $(LIBS);                 \
           do                             \
           cd $${I};                      \
           make clean;                    \
           cd $(SLTROOT)/src/app_su;       \
        done
	for I in $(BINS);                 \
           do                             \
           cd $${I};                      \
           make clean;                    \
           cd $(SLTROOT)/src/app_su;       \
        done

realclean: clean
	rm -rf include $(ARCH)

install: $(SLTROOT)/$(ARCH)
	for I in $(HDRS);                 \
           do                             \
           cd $${I};                      \
           make install;                  \
           cd $(SLTROOT)/src/app_su;       \
        done
	for I in $(LIBS);                 \
           do                             \
           cd $${I};                      \
           make install;                  \
           cd $(SLTROOT)/src/app_su;       \
        done
	for I in $(BINS);                 \
           do                             \
           cd $${I};                      \
           make install;                  \
           cd $(SLTROOT)/src/app_su;       \
        done

$(SLTROOT)/$(ARCH):
	mkdir -p $(SLTROOT)/$(ARCH)/lib
	mkdir -p $(SLTROOT)/$(ARCH)/bin
	mkdir -p $(SLTROOT)/$(ARCH)/doc
	mkdir -p $(SLTROOT)/$(ARCH)/include
