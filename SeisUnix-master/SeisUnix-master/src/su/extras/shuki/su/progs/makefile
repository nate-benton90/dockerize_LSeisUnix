#
# makefile for the su/progs directory
#

# DEFINE CONSTANTS
BDIR	= /usr/local/bin
LDIR	= /usr/local/lib/
BDIR	= ../bin/
LDIR    = ./
BIN	= $(BDIR)
PO	= 664
PX	= 775
#LSU	= $Llibsu.a $LlibCC.a
LSU	= ../lsu/libsu.a
LP	= ../lcplot/libcplot.a
LV	= ../lvec/libvec.a
FLIBS   = -lF77 -lI77 -lU77 -lF77
FLIBS   = -lF77 -lI77 -lU77
FLIBS   = 

CFLAGS=	-g
FFLAGS=	-g

X = $(BIN)sutpow $(BIN)suplot $(BIN)supr $(BIN)chart $(BIN)plot \
    $(BIN)susort $(BIN)sunmo $(BIN)sustack $(BIN)sudmo $(BIN)suahed \
    $(BIN)sumax $(BIN)susub $(BIN)suhis $(BIN)suwind $(BIN)suchw \
    $(BIN)sushw $(BIN)suedit $(BIN)sucat $(BIN)subal $(BIN)suvmute \
    $(BIN)suteep $(BIN)sutee $(BIN)subpus $(BIN)sugpow $(BIN)sufilt \
    $(BIN)sutrfft $(BIN)w4read $(BIN)w4write   $(BIN)supef \
    $(BIN)suagc $(BIN)sufilf $(BIN)sutwind \
    $(BIN)syread $(BIN)sutrfftm $(BIN)susbhw $(BIN)supva \
    $(BIN)sufilr $(BIN)suinfo $(BIN)sulog $(BIN)suilog $(BIN)sufft \
    $(BIN)eraseh $(BIN)supefmem $(BIN)supmovie


# GENERAL MAKE ENTRIES
all	:$X
	@echo made

remake	:
	make all
	
clean cleanup:
	-/bin/rm $X

# SHELL SCRIPTS (ALPHABETIC)

$(BIN)chart chart	:chart.csh
	cp chart.csh $(BIN)chart
	chmod $(PX) $@

$(BIN)plot plot	:plot.csh
	cp plot.csh $(BIN)plot
	chmod $(PX) $@

$(BIN)suhis suhis	:suhis.csh
	cp suhis.csh $(BIN)suhis
	chmod $(PX) $@


# APPLICATION PROGRAMS (ALPHABETIC)

$(BIN)suagc suagc: mainseq.o agc.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o agc.o $(LSU) $(LV) -lm -o $(BIN)suagc
	chmod $(PX) $@
$(BIN)syread syread: syread.o $(LSU) $(LV)
	cc $(CFLAGS) syread.o $(LSU) $(LV) -o $(BIN)syread
	chmod $(PX) $@
$(BIN)suahed suahed: suahed.o $(LSU)
	cc $(CFLAGS) suahed.o $(LSU) -o $(BIN)suahed
	chmod $(PX) $@
$(BIN)subal subal: mainseq.o bal.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o bal.o $(LSU) $(LV) -lm -o $(BIN)subal
	chmod $(PX) $@
$(BIN)subpus subpus: subpus.o $(LSU) $(LV)
	cc $(CFLAGS) subpus.o $(LSU) $(LV) -lm -o $(BIN)subpus
	chmod $(PX) $@
$(BIN)sucat sucat:	sucat.o $(LSU)
	cc $(CFLAGS) sucat.o $(LSU) -o $(BIN)sucat
	chmod $(PX) $@
$(BIN)suchw suchw: mainseq.o chw.o $(LSU)
	cc $(CFLAGS) mainseq.o chw.o $(LSU) -lm -o $(BIN)suchw
	chmod $(PX) $@
$(BIN)sudmo sudmo: sudmo.o getco.o putco.o zpad2.o trigs.o $(LSU)
	cc $(CFLAGS) sudmo.o getco.o putco.o zpad2.o trigs.o $(LSU) $(LV) -lm -o $(BIN)sudmo
	chmod $(PX) $@
$(BIN)suedit suedit: suedit.o $(LSU)
	cc $(CFLAGS) suedit.o $(LSU) -lm -o $(BIN)suedit
	chmod $(PX) $@
$(BIN)eraseh eraseh: mainseq.o eraseh.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o eraseh.o $(LSU) $(LV)  -lm -o $(BIN)eraseh
	chmod $(PX) $@
$(BIN)sufilf sufilf: mainseq.o filf.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filf.o $(LSU) $(LV) -lm -o $(BIN)sufilf
	chmod $(PX) $@
$(BIN)sufilr sufilr: mainseq.o filr.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filr.o $(LSU) $(LV) -lm -o $(BIN)sufilr
	chmod $(PX) $@
$(BIN)sufilt sufilt: mainseq.o filt.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filt.o $(LSU) $(LV) -lm -o $(BIN)sufilt
	chmod $(PX) $@
$(BIN)sugpow sugpow:mainseq.o gpow.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o gpow.o $(LSU) $(LV) -lm -o $(BIN)sugpow
	chmod $(PX) $@
$(BIN)suinfo suinfo:	suinfo.o $(LSU)
	cc $(CFLAGS) suinfo.o $(LSU) -lm -o $(BIN)suinfo
	chmod $(PX) $@
$(BIN)sulog sulog: mainseq.o log.o $(LSU)
	cc $(CFLAGS) mainseq.o log.o $(LSU) $(LV) -lm -o $(BIN)sulog
	chmod $(PX) $@
$(BIN)suilog suilog: mainseq.o ilog.o $(LSU)
	cc $(CFLAGS) mainseq.o ilog.o $(LSU) $(LV) -lm -o $(BIN)suilog
	chmod $(PX) $@
$(BIN)sumax sumax: mainseq.o max.o $(LSU)
	cc $(CFLAGS) mainseq.o max.o $(LSU) $(LV) -lm -o $(BIN)sumax
	chmod $(PX) $@
$(BIN)sunmo sunmo: mainseq.o nmo.o nmosubs.o  $(LSU)
	cc $(CFLAGS) mainseq.o nmo.o nmosubs.o $(LSU) -lm -o $(BIN)sunmo
	chmod $(PX) $@
$(BIN)supef supef: mainseq.o pef.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o pef.o $(LSU) $(LV) -lm -o $(BIN)supef
	chmod $(PX) $@
$(BIN)supefmem supefmem: mainseq.o pefmem.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o pefmem.o $(LSU) $(LV) -lm -o $(BIN)supefmem
	chmod $(PX) $@
$(BIN)suplot suplot:	mainseq.o plot.o $(LP) $(LSU)
	cc $(CFLAGS) mainseq.o plot.o $(LP) $(LSU) $(LV) -lm -o $(BIN)suplot
	chmod $(PX) $@
$(BIN)supr supr:	mainseq.o pr.o $(LSU)
	cc $(CFLAGS) mainseq.o pr.o $(LSU) -o $(BIN)supr
	chmod $(PX) $@
$(BIN)supva supva: supva.o getcdf.o putcdf.o nmosubs.o  $(LSU)
	cc $(CFLAGS) supva.o getcdf.o putcdf.o nmosubs.o $(LSU) $(LV) $(FLIBS) -lm -o $(BIN)supva
	chmod $(PX) $@
$(BIN)susbhw susbhw: mainseq.o sbhw.o $(LSU)
	cc $(CFLAGS) mainseq.o sbhw.o $(LSU) -lm -o $(BIN)susbhw
	chmod $(PX) $@
$(BIN)sushw sushw: mainseq.o shw.o $(LSU)
	cc $(CFLAGS) mainseq.o shw.o $(LSU) -lm -o $(BIN)sushw
	chmod $(PX) $@
$(BIN)susort susort: susort.o $(LSU)
	cc $(CFLAGS) susort.o $(LSU) -o $(BIN)susort
	chmod $(PX) $@
$(BIN)sustack sustack: sustack.o $(LSU) $(LV)
	cc $(CFLAGS) sustack.o $(LSU) $(LV) -lm -o $(BIN)sustack
	chmod $(PX) $@
$(BIN)susub susub: susub.o $(LSU) $(LV)
	cc $(CFLAGS) susub.o $(LSU) $(LV) -lm -o $(BIN)susub
	chmod $(PX) $@
$(BIN)sutee sutee: sutee.o $(LSU)
	cc $(CFLAGS) sutee.o $(LSU) -lm -o $(BIN)sutee
	chmod $(PX) $@
$(BIN)suteep suteep: suteep.o $(LSU)
	cc $(CFLAGS) suteep.o $(LSU) -lm -o $(BIN)suteep
	chmod $(PX) $@
$(BIN)sutpow sutpow: mainseq.o tpow.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o tpow.o $(LSU) $(LV) -lm -o $(BIN)sutpow
	chmod $(PX) $@
$(BIN)sufft sufft: mainseq.o fft.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o fft.o $(LSU) $(LV) -lm -o $(BIN)sufft
	chmod $(PX) $@
$(BIN)sutrfft sutrfft: mainseq.o trfft.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o trfft.o $(LSU) $(LV) -lm -o $(BIN)sutrfft
	chmod $(PX) $@
$(BIN)sutrfftm sutrfftm: mainseq.o trfftm.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o trfftm.o $(LSU) $(LV) -lm -o $(BIN)sutrfftm
	chmod $(PX) $@
$(BIN)sutwind sutwind: mainseq.o twind.o $(LSU)
	cc $(CFLAGS) mainseq.o twind.o $(LSU) -o $(BIN)sutwind
	chmod $(PX) $@
$(BIN)suvmute suvmute: mainseq.o vmute.o $(LSU)
	cc $(CFLAGS) mainseq.o vmute.o $(LSU) -lm -o $(BIN)suvmute
	chmod $(PX) $@
$(BIN)w4read w4read: w4read.o $(LSU)
	cc $(CFLAGS) w4read.o $(LSU) -o $(BIN)w4read
	chmod $(PX) $@
$(BIN)suvarin suvarin:	mainseq.o varin.o $(LP) $(LSU)
	cc $(CFLAGS) mainseq.o varin.o $(LSU) $(LV) -lpixrect -lm -o $(BIN)suvarin
	chmod $(PX) $@
$(BIN)suwind suwind: mainseq.o wind.o $(LSU)
	cc $(CFLAGS) mainseq.o wind.o $(LSU) -o $(BIN)suwind
	chmod $(PX) $@
$(BIN)w4write w4write: w4write.o $(LSU)
	cc $(CFLAGS) w4write.o $(LSU) -lm -o $(BIN)w4write
	chmod $(PX) $@
$(BIN)supmovie supmovie: supmovie.o $(LSU) $(LV)
	cc $(CFLAGS) supmovie.o $(LSU) $(LV) -lm -o $(BIN)supmovie
	chmod $(PX) $@
