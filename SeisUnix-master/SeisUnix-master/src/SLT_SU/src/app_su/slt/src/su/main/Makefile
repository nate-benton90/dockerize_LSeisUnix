
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)


PROGS =              \
   mutepick          \
   mutesu            \
   segy2movie        \
   sgyin             \
   sgyout            \
   su2sl             \
   su3dto2d          \
   suareazero        \
   subalance         \
   sudetect          \
   sufill            \
   sufold            \
   sugrd2hdr         \
   sugrid2hdr        \
   suhistogram       \
   sumerge           \
   suphase           \
   supoptop          \
   supressure        \
   supstack          \
   surenum           \
   surnmo            \
   surunmix          \
   susalthrz         \
   suscale3d         \
   susedrate         \
   sutosmute         \
   sutrifilter       \
   sutzconv          \
   suvautop          \
   suveldump         \
   suxarb            \
   suxyfix           \

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

