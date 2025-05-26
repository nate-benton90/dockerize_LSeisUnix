
ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )

include $(SLTROOT)/src/Config/Makefile.config.$(ARCH)

CFLAGS  += -I$(SEISPACT)/include -I./ -DAWARE
LDLIBS += $(SEISPACT)/lib/seilib.a

PROGS =                 \
   sucompress           \
   sucompress_merge     \
   sucompress_wind      \
   sudecomp             \

all: $(PROGS) 

$(PROGS) : bswapsub.o

bswapsub.o: bswapsub.c

install: $(PROGS)
	@ if [ ! -d $(SLTROOT)/$(ARCH)/bin ]; \
           then \
           mkdir $(SLTROOT)/$(ARCH)/bin;\
        fi
	cp $(PROGS) $(SLTROOT)/$(ARCH)/bin

clean:
	rm -f *.o $(PROGS)

lint:
	for I in $(PROGS);      \
           do                   \
        lint -errhdr=%all $(CFLAGS) $${I}.c; \
        done
