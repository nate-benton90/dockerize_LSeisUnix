ARCH = $(shell uname -srm | sed 's/.[0-9]*-[^ ]*//' | sed 's/ /_/g' )


DIRS =                                  \
     app_su                             \

all:
	for I in $(DIRS);               \
           do                           \
           cd $${I};                    \
           TOP=$(SLTROOT)/src/$${I};     \
           export TOP;                  \
           make;                        \
           cd $(SLTROOT)/src;            \
        done

clean: 
	for I in $(DIRS);               \
           do                           \
           cd $${I};                    \
           TOP=$(SLTROOT)/src/$${I};     \
           export TOP;                  \
           make clean;                  \
           cd $(SLTROOT)/src;            \
        done

realclean: clean
	find . -name \*.a -print -exec -rm -f {} \;
	find . -name \*.o -print -exec -rm -f {} \;
	rm -rf $(ARCH)/*
	for I in $(DIRS);               \
           do                           \
           cd $${I};                    \
           make realclean;              \
           cd $(SLTROOT)/src;            \
        done

install: 
	for I in $(DIRS);               \
           do                           \
           cd $${I};                    \
           TOP=$(SLTROOT)/src/$${I};     \
           export TOP;                  \
           make install;                \
           cd $(SLTROOT)/src;            \
        done

