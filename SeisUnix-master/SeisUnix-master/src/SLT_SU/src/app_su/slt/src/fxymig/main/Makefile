
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

PROGS =                 \
      ffm               \
      fxymcal           \
      fxymerge          \
      fxymig            \

all: $(PROGS) 

ffm: ffm.o
	$(CC) -o $@ $< -L../lib -lfxymig $(LDLIBS)

fxymcal: fxymcal.o
	$(CC) -o $@ $< -L../lib -lfxymig $(LDLIBS)

fxymerge: fxymerge.o
	$(CC) -o $@ $< -L../lib -lfxymig $(LDLIBS)

fxymig: fxymig.o
	$(CC) -o $@ $< -L../lib -lfxymig $(LDLIBS)

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
