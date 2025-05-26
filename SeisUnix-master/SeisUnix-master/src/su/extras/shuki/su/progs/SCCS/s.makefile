h03480
s 00098/00348/00090
d D 1.25 88/11/15 14:03:16 shuki 25 24
c 
e
s 00010/00001/00428
d D 1.24 88/11/06 09:52:35 shuki 24 23
c 
e
s 00000/00008/00429
d D 1.23 88/09/14 13:05:07 shuki 23 22
c eraseh appeared twice
e
s 00000/00002/00437
d D 1.22 88/07/26 16:04:42 valer 22 21
c 
e
s 00018/00009/00411
d D 1.21 88/07/24 15:42:06 valer 21 20
c 
e
s 00010/00001/00419
d D 1.20 88/07/17 14:49:41 moshe 20 19
c 
e
s 00008/00001/00412
d D 1.19 88/07/17 12:06:01 shemer 19 18
c 
e
s 00034/00001/00379
d D 1.18 88/06/27 07:14:03 shuki 18 17
c 
e
s 00012/00003/00368
d D 1.17 88/06/20 08:44:02 shemer 17 16
c 
e
s 00016/00012/00355
d D 1.16 88/06/19 10:49:21 shemer 16 15
c 
e
s 00015/00007/00352
d D 1.15 88/06/06 13:12:27 shuki 15 14
c Cancel ns in trace headers
e
s 00023/00013/00336
d D 1.14 88/05/29 14:48:47 root 14 13
c sutrfftm adding
e
s 00002/00010/00347
d D 1.13 88/05/25 14:54:20 shemer 13 12
c with SccsId[]
e
s 00023/00026/00334
d D 1.12 88/05/25 06:53:07 shuki 12 11
c umainseq
e
s 00002/00006/00358
d D 1.11 88/05/24 16:47:31 valer 11 10
c umainsec.c instead of mainsecv.c
e
s 00012/00016/00352
d D 1.10 88/05/24 08:49:48 valer 10 9
c replacement mainseqv.c by umainseq.c
e
s 00027/00012/00341
d D 1.9 88/05/23 10:19:45 shuki 9 8
c umainseq
e
s 00009/00001/00344
d D 1.8 88/05/19 07:02:48 shuki 8 7
c 
e
s 00009/00001/00336
d D 1.7 88/05/18 12:18:39 valer 7 6
c sufilf is added
e
s 00000/00000/00337
d D 1.6 88/05/17 10:45:23 shuki 6 5
c 
e
s 00013/00001/00324
d D 1.5 88/05/17 08:03:50 shemer 5 4
c 
e
s 00000/00000/00325
d D 1.4 88/05/16 15:06:34 valer 4 3
c 
e
s 00032/00001/00293
d D 1.3 88/05/16 06:42:47 shuki 3 2
c 
e
s 00008/00001/00286
d D 1.2 88/05/15 11:41:05 moshe 2 1
c 
e
s 00287/00000/00000
d D 1.1 88/05/15 11:32:34 moshe 1 0
c date and time created 88/05/15 11:32:34 by moshe
e
u
U
f e 0
t
T
I 1
#
# makefile for the su/progs directory
#

# DEFINE CONSTANTS
D 25
BIN	= /usr/local/
L	= /usr/local/lib/
O	= /usr/local/obj/
E 25
I 25
BDIR	= /usr/local/
LDIR	= /usr/local/lib/
BDIR	= ../bin/
LDIR    = ./
BIN	= $(BDIR)
E 25
PO	= 664
PX	= 775
D 3
LSU	= $Llibsu.a $LlibCC.a
E 3
I 3
#LSU	= $Llibsu.a $LlibCC.a
D 25
LSU	= $Llibsu.a
E 3
LP	= $Llibcplot.a
LV	= $Llibvec.a
E 25
I 25
LSU	= ../lsu/libsu.a
LP	= ../lcplot/libcplot.a
LV	= ../lvec/libvec.a
FLIBS   = -lF77 -lI77 -lU77 -lF77
FLIBS   = -lF77 -lI77 -lU77
FLIBS   = 

E 25
CFLAGS=	-g
FFLAGS=	-g

D 25
X =	$(BIN)sutpow $(BIN)suplot $(BIN)supr $(BIN)chart $(BIN)plot \
E 25
I 25
X = $(BIN)sutpow $(BIN)suplot $(BIN)supr $(BIN)chart $(BIN)plot \
E 25
D 15
	$(BIN)susort $(BIN)sunmo $(BIN)sustack $(BIN)sudmo $(BIN)suahed \
	$(BIN)sumax $(BIN)susub $(BIN)suhis $(BIN)suwind $(BIN)suchw \
	$(BIN)sushw $(BIN)suedit $(BIN)sucat $(BIN)subal $(BIN)suvmute \
	$(BIN)suteep $(BIN)sutee $(BIN)subpus $(BIN)sugpow $(BIN)sufilt \
D 13
	$(BIN)sunmm $(BIN)sutrfft $(BIN)w4read $(BIN)w4write $(BIN)supef \
E 13
I 13
        $(BIN)sutrfft $(BIN)w4read $(BIN)w4write   $(BIN)supef \
E 13
D 2
	$(BIN)suvarin
E 2
I 2
D 5
	$(BIN)suvarin $(BIN)suagc
E 5
I 5
D 7
	$(BIN)suvarin $(BIN)suagc $(BIN)supva
E 7
I 7
D 8
	$(BIN)suvarin $(BIN)suagc $(BIN)supva $(BIN)sufilf
E 8
I 8
D 9
	$(BIN)suvarin $(BIN)suagc $(BIN)supva $(BIN)sufilf $(BIN)sutwind
E 9
I 9
D 14
	$(BIN)suvarin $(BIN)suagc $(BIN)supva $(BIN)sufilf $(BIN)sutwind \
	$(BIN)syread
E 14
I 14
	$(BIN)suvarin $(BIN)suagc $(BIN)sufilf $(BIN)sutwind \
	$(BIN)syread $(BIN)sutrfftm
E 15
I 15
    $(BIN)susort $(BIN)sunmo $(BIN)sustack $(BIN)sudmo $(BIN)suahed \
    $(BIN)sumax $(BIN)susub $(BIN)suhis $(BIN)suwind $(BIN)suchw \
    $(BIN)sushw $(BIN)suedit $(BIN)sucat $(BIN)subal $(BIN)suvmute \
    $(BIN)suteep $(BIN)sutee $(BIN)subpus $(BIN)sugpow $(BIN)sufilt \
    $(BIN)sutrfft $(BIN)w4read $(BIN)w4write   $(BIN)supef \
D 25
    $(BIN)suvarin $(BIN)suagc $(BIN)sufilf $(BIN)sutwind \
E 25
I 25
    $(BIN)suagc $(BIN)sufilf $(BIN)sutwind \
E 25
D 16
    $(BIN)syread $(BIN)sutrfftm $(BIN)susbhw
E 16
I 16
D 17
    $(BIN)syread $(BIN)sutrfftm $(BIN)susbhw $(BIN)supva
E 17
I 17
    $(BIN)syread $(BIN)sutrfftm $(BIN)susbhw $(BIN)supva \
D 18
    $(BIN)sufilr
E 18
I 18
D 19
    $(BIN)sufilr $(BIN)suinfo $(BIN)sulog $(BIN)suilog $(BIN)sufft
E 19
I 19
D 20
D 21
    $(BIN)sufilr $(BIN)suinfo $(BIN)sulog $(BIN)suilog $(BIN)sufft $(BIN)sutrace
E 21
I 21
    $(BIN)sufilr $(BIN)suinfo $(BIN)sulog $(BIN)suilog $(BIN)sufft \
D 24
    $(BIN)eraseh $(BIN)supefmem
E 24
I 24
    $(BIN)eraseh $(BIN)supefmem $(BIN)supmovie
E 24
E 21
E 20
I 20
D 22
    $(BIN)sufilr $(BIN)suinfo $(BIN)sulog $(BIN)suilog $(BIN)sufft \
    $(BIN)sutrace $(BIN)eraseh
E 22
E 20
E 19
E 18
E 17
E 16
E 15
E 14
E 9
E 8
E 7
E 5
E 2

I 14

E 14
# GENERAL MAKE ENTRIES
all	:$X
	@echo made

remake	:
D 25
	-(cd $O; /bin/rm -f *.o)
E 25
	make all
	
clean cleanup:
D 25
	-(cd $O; /bin/rm -f *.o)
E 25
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
I 2

D 25
$(BIN)suagc suagc: $Omainseq.o $Oagc.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Oagc.o $(LSU) $(LV) -lm -o $(BIN)suagc
E 25
I 25
$(BIN)suagc suagc: mainseq.o agc.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o agc.o $(LSU) $(LV) -lm -o $(BIN)suagc
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Oagc.o	:agc.c
	cc $(CFLAGS) -c agc.c
	mv agc.o $O
	chmod $(PO) $@
E 2

I 9
D 17
$(BIN)syread syread: $Osyread.o $(LSU)
	cc $(CFLAGS) $Osyread.o $(LSU) -o $(BIN)syread
E 17
I 17
$(BIN)syread syread: $Osyread.o $(LSU) $(LV)
	cc $(CFLAGS) $Osyread.o $(LSU) $(LV) -o $(BIN)syread
E 25
I 25
$(BIN)syread syread: syread.o $(LSU) $(LV)
	cc $(CFLAGS) syread.o $(LSU) $(LV) -o $(BIN)syread
E 25
E 17
	chmod $(PX) $@
D 25
$Osyread.o	:syread.c
	cc $(CFLAGS) -c syread.c
	mv syread.o $O
	chmod $(PO) $@

E 9
$(BIN)suahed suahed: $Osuahed.o $(LSU)
	cc $(CFLAGS) $Osuahed.o $(LSU) -o $(BIN)suahed
E 25
I 25
$(BIN)suahed suahed: suahed.o $(LSU)
	cc $(CFLAGS) suahed.o $(LSU) -o $(BIN)suahed
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osuahed.o	:suahed.c
	cc $(CFLAGS) -c suahed.c
	mv suahed.o $O
	chmod $(PO) $@

$(BIN)subal subal: $Omainseq.o $Obal.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Obal.o $(LSU) $(LV) -lm -o $(BIN)subal
E 25
I 25
$(BIN)subal subal: mainseq.o bal.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o bal.o $(LSU) $(LV) -lm -o $(BIN)subal
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Obal.o	:bal.c
	cc $(CFLAGS) -c bal.c
	mv bal.o $O
	chmod $(PO) $@

$(BIN)subpus subpus: $Osubpus.o $(LSU) $(LV)
	cc $(CFLAGS) $Osubpus.o $(LSU) $(LV) -lm -o $(BIN)subpus
E 25
I 25
$(BIN)subpus subpus: subpus.o $(LSU) $(LV)
	cc $(CFLAGS) subpus.o $(LSU) $(LV) -lm -o $(BIN)subpus
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osubpus.o	:subpus.c
	cc $(CFLAGS) -c subpus.c
	mv subpus.o $O
	chmod $(PO) $@

$(BIN)sucat sucat:	$Osucat.o $(LSU)
	cc $(CFLAGS) $Osucat.o $(LSU) -o $(BIN)sucat
E 25
I 25
$(BIN)sucat sucat:	sucat.o $(LSU)
	cc $(CFLAGS) sucat.o $(LSU) -o $(BIN)sucat
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osucat.o	:sucat.c
	cc $(CFLAGS) -c sucat.c
	mv sucat.o $O
	chmod $(PO) $@

$(BIN)suchw suchw: $Omainseq.o $Ochw.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Ochw.o $(LSU) -lm -o $(BIN)suchw
E 25
I 25
$(BIN)suchw suchw: mainseq.o chw.o $(LSU)
	cc $(CFLAGS) mainseq.o chw.o $(LSU) -lm -o $(BIN)suchw
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Ochw.o	:chw.c
	cc $(CFLAGS) -c chw.c
	mv chw.o $O
	chmod $(PO) $@

$(BIN)sudmo sudmo: $Osudmo.o $Ogetco.o $Oputco.o $Ozpad2.o $Otrigs.o $(LSU)
	cc $(CFLAGS) $Osudmo.o $Ogetco.o $Oputco.o $Ozpad2.o $Otrigs.o $(LSU) $(LV) -lF77 -lI77 -lm -o $(BIN)sudmo
E 25
I 25
$(BIN)sudmo sudmo: sudmo.o getco.o putco.o zpad2.o trigs.o $(LSU)
	cc $(CFLAGS) sudmo.o getco.o putco.o zpad2.o trigs.o $(LSU) $(LV) -lm -o $(BIN)sudmo
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osudmo.o	:sudmo.c
	cc $(CFLAGS) -c sudmo.c
	mv sudmo.o $O
	chmod $(PO) $@
$Ogetco.o	:getco.c
	cc $(CFLAGS) -c getco.c
	mv getco.o $O
	chmod $(PO) $@
$Oputco.o	:putco.c
	cc $(CFLAGS) -c putco.c
	mv putco.o $O
	chmod $(PO) $@
$Ozpad2.o	:zpad2.c
	cc $(CFLAGS) -c zpad2.c
	mv zpad2.o $O
	chmod $(PO) $@
$Otrigs.o	:trigs.c
	cc $(CFLAGS) -c trigs.c
	mv trigs.o $O
	chmod $(PO) $@

$(BIN)suedit suedit: $Osuedit.o $(LSU)
	cc $(CFLAGS) $Osuedit.o $(LSU) -lm -o $(BIN)suedit
E 25
I 25
$(BIN)suedit suedit: suedit.o $(LSU)
	cc $(CFLAGS) suedit.o $(LSU) -lm -o $(BIN)suedit
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osuedit.o	:suedit.c
	cc $(CFLAGS) -c suedit.c
	mv suedit.o $O
I 20
	chmod $(PO) $@

$(BIN)eraseh eraseh: $Omainseq.o $Oeraseh.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Oeraseh.o $(LSU) $(LV)  -lm -o $(BIN)eraseh
E 25
I 25
$(BIN)eraseh eraseh: mainseq.o eraseh.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o eraseh.o $(LSU) $(LV)  -lm -o $(BIN)eraseh
E 25
	chmod $(PX) $@
D 25
$Oeraseh.o	:eraseh.c
	cc $(CFLAGS) -c eraseh.c
	mv eraseh.o $O
E 20
I 7
	chmod $(PO) $@

I 21
D 23
$(BIN)eraseh eraseh: $Omainseq.o $Oeraseh.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Oeraseh.o $(LSU) $(LV)  -lm -o $(BIN)eraseh
	chmod $(PX) $@
$Oeraseh.o	:eraseh.c
	cc $(CFLAGS) -c eraseh.c
	mv eraseh.o $O
	chmod $(PO) $@

E 23
E 21
D 10
$(BIN)sufilf sufilf: $Omainseqv.o $Ofilf.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseqv.o $Ofilf.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilf
E 10
I 10
D 12
$(BIN)sufilf sufilf: $Oumainseq.o $Ofilf.o $(LSU) $(LV)
	cc $(CFLAGS) $Oumainseq.o $Ofilf.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilf
E 12
I 12
$(BIN)sufilf sufilf: $Omainseq.o $Ofilf.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Ofilf.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilf
E 25
I 25
$(BIN)sufilf sufilf: mainseq.o filf.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filf.o $(LSU) $(LV) -lm -o $(BIN)sufilf
E 25
E 12
E 10
	chmod $(PX) $@
D 25
$Ofilf.o	:filf.c
	cc $(CFLAGS) -c filf.c
	mv filf.o $O
I 17
	chmod $(PO) $@

$(BIN)sufilr sufilr: $Omainseq.o $Ofilr.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Ofilr.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilr
E 25
I 25
$(BIN)sufilr sufilr: mainseq.o filr.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filr.o $(LSU) $(LV) -lm -o $(BIN)sufilr
E 25
	chmod $(PX) $@
D 25
$Ofilr.o	:filr.c
	cc $(CFLAGS) -c filr.c
	mv filr.o $O
E 17
E 7
	chmod $(PO) $@

D 10
$(BIN)sufilt sufilt: $Omainseqv.o $Ofilt.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseqv.o $Ofilt.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilt
E 10
I 10
D 12
$(BIN)sufilt sufilt: $Oumainseq.o $Ofilt.o $(LSU) $(LV)
	cc $(CFLAGS) $Oumainseq.o $Ofilt.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilt
E 12
I 12
$(BIN)sufilt sufilt: $Omainseq.o $Ofilt.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Ofilt.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufilt
E 25
I 25
$(BIN)sufilt sufilt: mainseq.o filt.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o filt.o $(LSU) $(LV) -lm -o $(BIN)sufilt
E 25
E 12
E 10
I 3
	chmod $(PX) $@
E 3
D 25
$Ofilt.o	:filt.c
	cc $(CFLAGS) -c filt.c
	mv filt.o $O
	chmod $(PO) $@

$(BIN)sugpow sugpow:$Omainseq.o $Ogpow.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Ogpow.o $(LSU) $(LV) -lm -o $(BIN)sugpow
E 25
I 25
$(BIN)sugpow sugpow:mainseq.o gpow.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o gpow.o $(LSU) $(LV) -lm -o $(BIN)sugpow
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Ogpow.o	:gpow.c
	cc $(CFLAGS) -c gpow.c
	mv gpow.o $O
	chmod $(PO) $@

I 18
$(BIN)suinfo suinfo:	$Osuinfo.o $(LSU)
	cc $(CFLAGS) $Osuinfo.o $(LSU) -lm -o $(BIN)suinfo
E 25
I 25
$(BIN)suinfo suinfo:	suinfo.o $(LSU)
	cc $(CFLAGS) suinfo.o $(LSU) -lm -o $(BIN)suinfo
E 25
	chmod $(PX) $@
D 25
$Osuinfo.o    :suinfo.c
	cc $(CFLAGS) -c suinfo.c
	mv suinfo.o $O
	chmod $(PO) $@

$(BIN)sulog sulog: $Omainseq.o $Olog.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Olog.o $(LSU) $(LV) -lm -o $(BIN)sulog
E 25
I 25
$(BIN)sulog sulog: mainseq.o log.o $(LSU)
	cc $(CFLAGS) mainseq.o log.o $(LSU) $(LV) -lm -o $(BIN)sulog
E 25
	chmod $(PX) $@
D 25
$Olog.o	:log.c
	cc $(CFLAGS) -c log.c
	mv log.o $O
	chmod $(PO) $@

$(BIN)suilog suilog: $Omainseq.o $Oilog.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Oilog.o $(LSU) $(LV) -lm -o $(BIN)suilog
E 25
I 25
$(BIN)suilog suilog: mainseq.o ilog.o $(LSU)
	cc $(CFLAGS) mainseq.o ilog.o $(LSU) $(LV) -lm -o $(BIN)suilog
E 25
	chmod $(PX) $@
D 25
$Oilog.o	:ilog.c
	cc $(CFLAGS) -c ilog.c
	mv ilog.o $O
	chmod $(PO) $@

E 18
D 10
$(BIN)sumax sumax: $Omainseqn.o $Omax.o $(LSU)
	cc $(CFLAGS) $Omainseqn.o $Omax.o $(LSU) $(LV) -lm -o $(BIN)sumax
E 10
I 10
D 12
$(BIN)sumax sumax: $Oumainseq.o $Omax.o $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Omax.o $(LSU) $(LV) -lm -o $(BIN)sumax
E 12
I 12
$(BIN)sumax sumax: $Omainseq.o $Omax.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Omax.o $(LSU) $(LV) -lm -o $(BIN)sumax
E 25
I 25
$(BIN)sumax sumax: mainseq.o max.o $(LSU)
	cc $(CFLAGS) mainseq.o max.o $(LSU) $(LV) -lm -o $(BIN)sumax
E 25
E 12
E 10
I 3
	chmod $(PX) $@
E 3
D 25
$Omax.o	:max.c
	cc $(CFLAGS) -c max.c
	mv max.o $O
	chmod $(PO) $@

D 13
$(BIN)sunmo sunmo: $Omainseq.o $Onmo.o $Onmosubs.o $(LSU)
E 13
I 13
$(BIN)sunmo sunmo: $Omainseq.o $Onmo.o $Onmosubs.o  $(LSU)
E 13
	cc $(CFLAGS) $Omainseq.o $Onmo.o $Onmosubs.o $(LSU) -lm -o $(BIN)sunmo
E 25
I 25
$(BIN)sunmo sunmo: mainseq.o nmo.o nmosubs.o  $(LSU)
	cc $(CFLAGS) mainseq.o nmo.o nmosubs.o $(LSU) -lm -o $(BIN)sunmo
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Onmo.o	:nmo.c
	cc $(CFLAGS) -c nmo.c
	mv nmo.o $O
D 13
	chmod $(PO) $@

$(BIN)sunmm sunmm: $Omainseq.o $Onmo1.o $Onmosubs.o  $(LSU)
	cc $(CFLAGS) $Omainseq.o $Onmo1.o $Onmosubs.o $(LSU) -lm -o $(BIN)sunmm
I 3
	chmod $(PX) $@
E 3
$Onmo1.o	:nmo1.c
	cc $(CFLAGS) -c nmo1.c
	mv nmo1.o $O
E 13
	chmod $(PO) $@

D 9
$(BIN)supef supef: $Omainseqv.o $Opef.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseqv.o $Opef.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)supef
E 9
I 9
D 12
$(BIN)supef supef: $Oumainseq.o $Opef.o $(LSU) $(LV)
	cc $(CFLAGS) $Oumainseq.o $Opef.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)supef
E 12
I 12
$(BIN)supef supef: $Omainseq.o $Opef.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Opef.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)supef
E 25
I 25
$(BIN)supef supef: mainseq.o pef.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o pef.o $(LSU) $(LV) -lm -o $(BIN)supef
E 25
E 12
E 9
I 3
	chmod $(PX) $@
E 3
D 25
$Opef.o	:pef.c
	cc $(CFLAGS) -c pef.c
	mv pef.o $O
	chmod $(PO) $@

I 21
$(BIN)supefmem supefmem: $Omainseq.o $Opefmem.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Opefmem.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)supefmem
E 25
I 25
$(BIN)supefmem supefmem: mainseq.o pefmem.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o pefmem.o $(LSU) $(LV) -lm -o $(BIN)supefmem
E 25
	chmod $(PX) $@
D 25
$Opefmem.o	:pefmem.c
	cc $(CFLAGS) -c pefmem.c
	mv pefmem.o $O
	chmod $(PO) $@

E 21
D 10
$(BIN)suplot suplot:	$Omainseqn.o $Oplot.o $(LP) $(LSU)
	cc $(CFLAGS) $Omainseqn.o $Oplot.o $(LP) $(LSU) $(LV) -lm -o $(BIN)suplot
E 10
I 10
D 12
$(BIN)suplot suplot:	$Oumainseq.o $Oplot.o $(LP) $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Oplot.o $(LP) $(LSU) $(LV) -lm -o $(BIN)suplot
E 12
I 12
$(BIN)suplot suplot:	$Omainseq.o $Oplot.o $(LP) $(LSU)
	cc $(CFLAGS) $Omainseq.o $Oplot.o $(LP) $(LSU) $(LV) -lm -o $(BIN)suplot
E 25
I 25
$(BIN)suplot suplot:	mainseq.o plot.o $(LP) $(LSU)
	cc $(CFLAGS) mainseq.o plot.o $(LP) $(LSU) $(LV) -lm -o $(BIN)suplot
E 25
E 12
E 10
I 3
	chmod $(PX) $@
E 3
D 25
$Oplot.o	:plot.c
	cc $(CFLAGS) -c plot.c
	mv plot.o $O
	chmod $(PO) $@

#$(BIN)supmovie supmovie:	$Opmovie.o $(LP) $(LSU)
#	cc $(CFLAGS) $Osupmovie.o $(LP) $(LSU) $(LV) -lm -o $(BIN)supmovie
E 25
I 25
$(BIN)supr supr:	mainseq.o pr.o $(LSU)
	cc $(CFLAGS) mainseq.o pr.o $(LSU) -o $(BIN)supr
E 25
I 3
	chmod $(PX) $@
E 3
D 25
#$Opmovie.o	:supmovie.c
#	cc $(CFLAGS) -c supmovie.c
#	mv supmovie.o $O

D 9
$(BIN)supr supr:	$Omainseqn.o $Opr.o $(LSU)
	cc $(CFLAGS) $Omainseqn.o $Opr.o $(LSU) -o $(BIN)supr
E 9
I 9
D 12
$(BIN)supr supr:	$Oumainseq.o $Opr.o $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Opr.o $(LSU) -o $(BIN)supr
E 12
I 12
$(BIN)supr supr:	$Omainseq.o $Opr.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Opr.o $(LSU) -o $(BIN)supr
E 25
I 25
$(BIN)supva supva: supva.o getcdf.o putcdf.o nmosubs.o  $(LSU)
	cc $(CFLAGS) supva.o getcdf.o putcdf.o nmosubs.o $(LSU) $(LV) $(FLIBS) -lm -o $(BIN)supva
E 25
E 12
E 9
I 3
	chmod $(PX) $@
E 3
D 25
$Opr.o	:pr.c
	cc $(CFLAGS) -c pr.c
	mv pr.o $O
I 5
	chmod $(PO) $@

D 14
$(BIN)supva supva: $Osupva.o $Ogetcdf.o  $(LSU)
	cc $(CFLAGS) $Osupva.o $Ogetcdf.o  $(LSU) $(LV) -lF77 -lI77 -lm -o $(BIN)supva
	chmod $(PX) $@
$Osupva.o	:supva.c
	cc $(CFLAGS) -c supva.c
	mv supva.o $O
	chmod $(PO) $@
$Ogetcdf.o	:getcdf.c
	cc $(CFLAGS) -c getcdf.c
	mv getcdf.o $O
E 5
	chmod $(PO) $@
E 14
I 14
D 16
#$(BIN)supva supva: $Osupva.o $Ogetcdf.o  $(LSU)
#	cc $(CFLAGS) $Osupva.o $Ogetcdf.o  $(LSU) $(LV) -lF77 -lI77 -lm -o $(BIN)supva
#	chmod $(PX) $@
#$Osupva.o	:supva.c
#	cc $(CFLAGS) -c supva.c
#	mv supva.o $O
#	chmod $(PO) $@
#$Ogetcdf.o	:getcdf.c
#	cc $(CFLAGS) -c getcdf.c
#	mv getcdf.o $O
#	chmod $(PO) $@
E 16
I 16
$(BIN)supva supva: $Osupva.o $Ogetcdf.o $Oputcdf.o $Onmosubs.o  $(LSU)
	cc $(CFLAGS) $Osupva.o $Ogetcdf.o $Oputcdf.o  $Onmosubs.o   $(LSU) $(LV) -lF77 -lI77 -lm -o $(BIN)supva
E 25
I 25
$(BIN)susbhw susbhw: mainseq.o sbhw.o $(LSU)
	cc $(CFLAGS) mainseq.o sbhw.o $(LSU) -lm -o $(BIN)susbhw
E 25
	chmod $(PX) $@
D 25
$Osupva.o	:supva.c
	cc $(CFLAGS) -c supva.c
	mv supva.o $O
	chmod $(PO) $@
$Ogetcdf.o	:getcdf.c
	cc $(CFLAGS) -c getcdf.c
	mv getcdf.o $O
	chmod $(PO) $@
$Oputcdf.o	:putcdf.c
	cc $(CFLAGS) -c putcdf.c
	mv putcdf.o $O
	chmod $(PO) $@
E 16
I 15

$(BIN)susbhw susbhw: $Omainseq.o $Osbhw.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Osbhw.o $(LSU) -lm -o $(BIN)susbhw
E 25
I 25
$(BIN)sushw sushw: mainseq.o shw.o $(LSU)
	cc $(CFLAGS) mainseq.o shw.o $(LSU) -lm -o $(BIN)sushw
E 25
	chmod $(PX) $@
D 25
$Osbhw.o	:sbhw.c
	cc $(CFLAGS) -c sbhw.c
	mv sbhw.o $O
	chmod $(PO) $@
E 15
E 14

$(BIN)sushw sushw: $Omainseq.o $Oshw.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Oshw.o $(LSU) -lm -o $(BIN)sushw
E 25
I 25
$(BIN)susort susort: susort.o $(LSU)
	cc $(CFLAGS) susort.o $(LSU) -o $(BIN)susort
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Oshw.o	:shw.c
	cc $(CFLAGS) -c shw.c
	mv shw.o $O
	chmod $(PO) $@

$(BIN)susort susort: $Osusort.o $(LSU)
	cc $(CFLAGS) $Osusort.o $(LSU) -o $(BIN)susort
E 25
I 25
$(BIN)sustack sustack: sustack.o $(LSU) $(LV)
	cc $(CFLAGS) sustack.o $(LSU) $(LV) -lm -o $(BIN)sustack
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osusort.o	:susort.c
	cc $(CFLAGS) -c susort.c
	mv susort.o $O
	chmod $(PO) $@

$(BIN)sustack sustack: $Osustack.o $(LSU) $(LV)
	cc $(CFLAGS) $Osustack.o $(LSU) $(LV) -lm -o $(BIN)sustack
E 25
I 25
$(BIN)susub susub: susub.o $(LSU) $(LV)
	cc $(CFLAGS) susub.o $(LSU) $(LV) -lm -o $(BIN)susub
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osustack.o	:sustack.c
	cc $(CFLAGS) -c sustack.c
	mv sustack.o $O
	chmod $(PO) $@

$(BIN)susub susub: $Osusub.o $(LSU) $(LV)
	cc $(CFLAGS) $Osusub.o $(LSU) $(LV) -lm -o $(BIN)susub
E 25
I 25
$(BIN)sutee sutee: sutee.o $(LSU)
	cc $(CFLAGS) sutee.o $(LSU) -lm -o $(BIN)sutee
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osusub.o	:susub.c
	cc $(CFLAGS) -c susub.c
	mv susub.o $O
	chmod $(PO) $@

$(BIN)sutee sutee: $Osutee.o $(LSU)
	cc $(CFLAGS) $Osutee.o $(LSU) -lm -o $(BIN)sutee
E 25
I 25
$(BIN)suteep suteep: suteep.o $(LSU)
	cc $(CFLAGS) suteep.o $(LSU) -lm -o $(BIN)suteep
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osutee.o	:sutee.c
	cc $(CFLAGS) -c sutee.c
	mv sutee.o $O
	chmod $(PO) $@

$(BIN)suteep suteep: $Osuteep.o $(LSU)
	cc $(CFLAGS) $Osuteep.o $(LSU) -lm -o $(BIN)suteep
E 25
I 25
$(BIN)sutpow sutpow: mainseq.o tpow.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o tpow.o $(LSU) $(LV) -lm -o $(BIN)sutpow
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Osuteep.o	:suteep.c
	cc $(CFLAGS) -c suteep.c
	mv suteep.o $O
	chmod $(PO) $@

D 9
$(BIN)sutpow sutpow: $Omainseq.o $Otpow.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Otpow.o $(LSU) $(LV) -lm -o $(BIN)sutpow
E 9
I 9
D 12
$(BIN)sutpow sutpow: $Oumainseq.o $Otpow.o $(LSU) $(LV)
	cc $(CFLAGS) $Oumainseq.o $Otpow.o $(LSU) $(LV) -lm -o $(BIN)sutpow
E 12
I 12
$(BIN)sutpow sutpow: $Omainseq.o $Otpow.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Otpow.o $(LSU) $(LV) -lm -o $(BIN)sutpow
E 25
I 25
$(BIN)sufft sufft: mainseq.o fft.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o fft.o $(LSU) $(LV) -lm -o $(BIN)sufft
E 25
E 12
E 9
I 3
	chmod $(PX) $@
E 3
D 25
$Otpow.o	:tpow.c
	cc $(CFLAGS) -c tpow.c
	mv tpow.o $O
I 18
	chmod $(PO) $@


$(BIN)sufft sufft: $Omainseq.o $Offt.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Offt.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sufft
E 25
I 25
$(BIN)sutrfft sutrfft: mainseq.o trfft.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o trfft.o $(LSU) $(LV) -lm -o $(BIN)sutrfft
E 25
	chmod $(PX) $@
D 25
$Offt.o       :fft.c
	cc $(CFLAGS) -c fft.c
	mv fft.o $O
E 18
	chmod $(PO) $@

D 11
$(BIN)sutrfft sutrfft: $Omainseqv.o $Otrfft.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseqv.o $Otrfft.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sutrfft
E 11
I 11
D 12
$(BIN)sutrfft sutrfft: $Oumainseq.o $Otrfft.o $(LSU) $(LV)
	cc $(CFLAGS) $Oumainseq.o $Otrfft.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sutrfft
E 12
I 12
$(BIN)sutrfft sutrfft: $Omainseq.o $Otrfft.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Otrfft.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sutrfft
E 25
I 25
$(BIN)sutrfftm sutrfftm: mainseq.o trfftm.o $(LSU) $(LV)
	cc $(CFLAGS) mainseq.o trfftm.o $(LSU) $(LV) -lm -o $(BIN)sutrfftm
E 25
E 12
E 11
I 3
	chmod $(PX) $@
E 3
D 25
$Otrfft.o       :trfft.c
	cc $(CFLAGS) -c trfft.c
	mv trfft.o $O
I 8
	chmod $(PO) $@

I 14
$(BIN)sutrfftm sutrfftm: $Omainseq.o $Otrfftm.o $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Otrfftm.o $(LSU) $(LV) -lI77 -lF77 -lm -o $(BIN)sutrfftm
E 25
I 25
$(BIN)sutwind sutwind: mainseq.o twind.o $(LSU)
	cc $(CFLAGS) mainseq.o twind.o $(LSU) -o $(BIN)sutwind
E 25
	chmod $(PX) $@
D 25
$Otrfftm.o       :trfftm.c
	cc $(CFLAGS) -c trfftm.c
	mv trfftm.o $O
	chmod $(PO) $@

#$(BIN)sutwind sutwind: $Osutwind.o $(LSU)
E 14
D 9
$(BIN)sutwind sutwind: $Osutwind.o $(LSU)
	cc $(CFLAGS) $Osutwind.o $(LSU) -o $(BIN)sutwind
E 9
I 9
#$(BIN)sutwind sutwind: $Osutwind.o $(LSU)
#	cc $(CFLAGS) $Osutwind.o $(LSU) -o $(BIN)sutwind
D 12
$(BIN)sutwind sutwind: $Oumainseq.o $Otwind.o $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Otwind.o $(LSU) -o $(BIN)sutwind
E 12
I 12
$(BIN)sutwind sutwind: $Omainseq.o $Otwind.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Otwind.o $(LSU) -o $(BIN)sutwind
E 25
I 25
$(BIN)suvmute suvmute: mainseq.o vmute.o $(LSU)
	cc $(CFLAGS) mainseq.o vmute.o $(LSU) -lm -o $(BIN)suvmute
E 25
E 12
E 9
	chmod $(PX) $@
D 9
$Osutwind.o	:sutwind.c
	cc $(CFLAGS) -c sutwind.c
	mv sutwind.o $O
E 9
I 9
D 25
$Otwind.o	:twind.c
	cc $(CFLAGS) -c twind.c
	mv twind.o $O
I 19
	chmod $(PO) $@
D 21
$(BIN)sutrace sutrace: $Omainseq.o $Omaint.o  $(LSU) $(LV)
	cc $(CFLAGS) $Omainseq.o $Omaint.o  $(LSU) $(LV) -lm -o $(BIN)sutrace
	chmod $(PX) $@
$Omaint.o	:maint.c
	cc $(CFLAGS) -c maint.c
	mv maint.o $O
E 19
E 9
E 8
	chmod $(PO) $@

E 21
$(BIN)suvmute suvmute: $Omainseq.o $Ovmute.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Ovmute.o $(LSU) -lm -o $(BIN)suvmute
E 25
I 25
$(BIN)w4read w4read: w4read.o $(LSU)
	cc $(CFLAGS) w4read.o $(LSU) -o $(BIN)w4read
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Ovmute.o	:vmute.c
	cc $(CFLAGS) -c vmute.c
	mv vmute.o $O
	chmod $(PO) $@

$(BIN)w4read w4read: $Ow4read.o $(LSU)
	cc $(CFLAGS) $Ow4read.o $(LSU) -o $(BIN)w4read
E 25
I 25
$(BIN)suvarin suvarin:	mainseq.o varin.o $(LP) $(LSU)
	cc $(CFLAGS) mainseq.o varin.o $(LSU) $(LV) -lpixrect -lm -o $(BIN)suvarin
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Ow4read.o	:w4read.c
	cc $(CFLAGS) -c w4read.c
	mv w4read.o $O
	chmod $(PO) $@

D 10
$(BIN)suvarin suvarin:	$Omainseqn.o $Ovarin.o $(LP) $(LSU)
	cc $(CFLAGS) $Omainseqn.o $Ovarin.o $(LSU) $(LV) -lpixrect -lm -o $(BIN)suvarin
E 10
I 10
D 12
$(BIN)suvarin suvarin:	$Oumainseq.o $Ovarin.o $(LP) $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Ovarin.o $(LSU) $(LV) -lpixrect -lm -o $(BIN)suvarin
E 12
I 12
$(BIN)suvarin suvarin:	$Omainseq.o $Ovarin.o $(LP) $(LSU)
	cc $(CFLAGS) $Omainseq.o $Ovarin.o $(LSU) $(LV) -lpixrect -lm -o $(BIN)suvarin
E 25
I 25
$(BIN)suwind suwind: mainseq.o wind.o $(LSU)
	cc $(CFLAGS) mainseq.o wind.o $(LSU) -o $(BIN)suwind
E 25
E 12
E 10
I 3
	chmod $(PX) $@
E 3
D 25
$Ovarin.o	:varin.c
	cc $(CFLAGS) -c varin.c
	mv varin.o $O
	chmod $(PO) $@

D 10
$(BIN)suwind suwind: $Omainseq.o $Owind.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Owind.o $(LSU) -o $(BIN)suwind
E 10
I 10
D 12
$(BIN)suwind suwind: $Oumainseq.o $Owind.o $(LSU)
	cc $(CFLAGS) $Oumainseq.o $Owind.o $(LSU) -o $(BIN)suwind
E 12
I 12
$(BIN)suwind suwind: $Omainseq.o $Owind.o $(LSU)
	cc $(CFLAGS) $Omainseq.o $Owind.o $(LSU) -o $(BIN)suwind
E 25
I 25
$(BIN)w4write w4write: w4write.o $(LSU)
	cc $(CFLAGS) w4write.o $(LSU) -lm -o $(BIN)w4write
E 25
E 12
E 10
I 3
	chmod $(PX) $@
E 3
D 25
$Owind.o	:wind.c
	cc $(CFLAGS) -c wind.c
	mv wind.o $O
	chmod $(PO) $@
 
$(BIN)w4write w4write: $Ow4write.o $(LSU)
	cc $(CFLAGS) $Ow4write.o $(LSU) -lm -o $(BIN)w4write
E 25
I 25
$(BIN)supmovie supmovie: supmovie.o $(LSU) $(LV)
	cc $(CFLAGS) supmovie.o $(LSU) $(LV) -lm -o $(BIN)supmovie
E 25
I 3
	chmod $(PX) $@
E 3
D 25
$Ow4write.o	:w4write.c
	cc $(CFLAGS) -c w4write.c
	mv w4write.o $O
	chmod $(PO) $@

I 9
D 12
$Oumainseq.o	:umainseq.c
	cc $(CFLAGS) -c umainseq.c
	mv umainseq.o $O
D 11
	chmod $(PO) $@
E 9
$Omainseqv.o	:mainseqv.c
	cc $(CFLAGS) -c mainseqv.c
	mv mainseqv.o $O
E 11
	chmod $(PO) $@
E 12
$Omainseq.o	:mainseq.c
	cc $(CFLAGS) -c mainseq.c
	mv mainseq.o $O
D 10
	chmod $(PO) $@
$Omainseqn.o	:mainseqn.c
	cc $(CFLAGS) -c mainseqn.c
	mv mainseqn.o $O
E 10
	chmod $(PO) $@
I 12

E 12
$Onmosubs.o	:nmosubs.f
	f77 $(FFLAGS) -c nmosubs.f
	mv nmosubs.o $O
	chmod $(PO) $@
I 24

$(BIN)supmovie supmovie: $Osupmovie.o $(LSU) $(LV)
	cc $(CFLAGS) $Osupmovie.o $(LSU) $(LV) -lm -o $(BIN)supmovie
	chmod $(PX) $@
$Osupmovie.o	:supmovie.c
	cc $(CFLAGS) -c supmovie.c
	mv supmovie.o $O
	chmod $(PO) $@

E 25
E 24
E 1
