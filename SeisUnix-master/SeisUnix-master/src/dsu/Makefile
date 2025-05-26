# Master makefile for ./dsu

include $(CWPROOT)/src/Makefile.config

INSTALL:
	cd lib        ; $(MAKE)
	cd gui        ; $(MAKE)
	cd main       ; $(MAKE)
	cd main_dst   ; $(MAKE)
	cd graphics   ; $(MAKE)

remake:
	cd lib        ; $(MAKE) remake
	cd gui        ; $(MAKE) remake
	cd main       ; $(MAKE) remake
	cd main_dst   ; $(MAKE) remake
	cd graphics   ; $(MAKE) remake
