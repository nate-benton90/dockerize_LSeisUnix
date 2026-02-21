# Master makefile for ./su

include $(CWPROOT)/src/Makefile.config

INSTALL:
	cd apertures ; $(MAKE)
	cd cellular_automata ; $(MAKE)
	cd data_conversion ; $(MAKE)
	cd ordinary_differential_equations ; $(MAKE)
	cd parameter_file_utilities ; $(MAKE)
	cd ray_theory ; $(MAKE)
	cd refraction ; $(MAKE)
	cd resampling ; $(MAKE)
	cd smoothing ; $(MAKE)
	cd statistics ; $(MAKE)
	cd velocity_model_building ; $(MAKE)

remake:
	cd apertures ; $(MAKE) remake
	cd cellular_automata ; $(MAKE)
	cd data_conversion ; $(MAKE) remake
	cd ordinary_differential_equations ; $(MAKE) remake
	cd parameter_file_utilities ; $(MAKE) remake
	cd ray_theory ; $(MAKE) remake
	cd refraction ; $(MAKE) remake
	cd resampling ; $(MAKE) remake
	cd smoothing ; $(MAKE) remake
	cd statistics ; $(MAKE) remake
	cd velocity_model_building ; $(MAKE) remake

