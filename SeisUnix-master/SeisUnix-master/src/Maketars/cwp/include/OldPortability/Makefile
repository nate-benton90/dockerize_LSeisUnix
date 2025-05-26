# Makefile for ..cwp/include
# Just say "make"

include $(CWPROOT)/src/Makefile.config

INCLUDE = $I/cwp.h $I/cwpport.h

INSTALL	: $(INCLUDE)
	touch $@

$(INCLUDE):	$$(@F)
	@cp $? $@
	@$(ICHMODLINE)
	@echo $(@F) installed in $I

remake	:
	@-touch *.h
	@$(MAKE)
