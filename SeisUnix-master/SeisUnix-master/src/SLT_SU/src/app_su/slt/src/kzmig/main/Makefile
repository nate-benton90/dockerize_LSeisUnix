
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

PROGS =                 \
      kmmerge           \
      kzmig             \
      kzplanner         \

all: $(PROGS) 

kmmerge: kmmerge.o
	$(CC) -o $@ $< -L../lib -lkzmig $(LDLIBS)

kzmig: kzmig.o
	$(CC) -o $@ $< -L../lib -lkzmig $(LDLIBS)

kzplanner: kzplanner.o
	$(CC) -o $@ $< -L../lib -lkzmig $(LDLIBS)

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
