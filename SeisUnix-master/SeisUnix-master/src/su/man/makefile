# $Author: jkc $
# $Source: /src/su/man/RCS/makefile,v $
# $Revision: 1.2 $ ; $Date: 88/06/07 20:28:40 $
#
# Note:  Masscomp designates /usr/man/manl for local man pages and uses
#	 a .l suffix, hence the copy to the .l file in the install loop
#        Both M and the copy and following commands may need to
#        be changed for non-Masscomp machines.  See the comments below.

.PRECIOUS	: print

# Change this to man directory for local man pages
M = /usr/man/manl

# The sources have a .1 extension
MAN1 =	suintro	segyhdrs	segyread	segywrite	\
	su3dchart	suabshw	suaddhead	suaddnoise	\
	suagc	suamp	suband	suchart	suchw	sudiff	suenv	\
	suedit	suflip	sufft	sugain	sugazmig	sugethw	\
	suhilb	suifft	sumax	sumute	sunmo	supack1	supack2	\
	supow	surange	sushw	sustrip	sutwig	suunpack1	\
	suunpack2	suus	suwind	suwig

# The sources have a .3 extension
MAN3 =	sulib							\
	askdoc	atohe	atoie	atole	atope	atoue	atove	\
	conv_float	dmatrix	doagc	doclip	doepow	dogpow	\
	dopbal	doqbal	doqclip	doscale	dotpow	dotrap	dvector	\
	err	fgetpar	fgettr	fputtr	free_dmatrix		\
	free_dvector	free_imatrix	free_ivector		\
	free_matrix	free_vector	gain	gethdval	\
	gethval	getkey	getname	hgetpar igetpar	imatrix	ivector	\
	matrix	maxgetpar	pgetpar	puthdval	puthval	\
	quant	selfdoc	sgetpar	statfil	statprint	syserr	\
	syswarn ugetpar	vector	vgetpar	warn	zgetpar		\
	veclib							\
	dotpr	maxmgv	maxv	meamgv	meanv	measqv	minmgv	\
	minv	rmsqv	svdiv	sve	svemg	svesq	svs	\
	vaam	vabs	vadd	vam	vatan	vatn2	vcos	\
	vdiv	vexp	vfill	vfix	vflt	vfrac	vint	\
	vln	vlog	vma	vmax	vmaxmg	vmin	vminmg	\
	vmma	vmmsb	vmov	vmsa	vmsb	vmul	vneg	\
	vsadd	vsbm	vsbsbm	vsdiv	vsin	vsma	vsmsa	\
	vsmsb	vsmul	vspow	vsq	vsqrt	vssq	vssqrt	\
	vsub	vswap

# The sources have a .9 extension
MAN9 =	sushell							\
	3dchart	chart	lookpar	maxdiff	recip	sufind	tube	\
	unglitch	updatedoc	qtwig	qqtwig	qwig	\
	qqwig

all:	$(MAN1) $(MAN3) $(MAN9)

# Copy to .l file is Masscomp convention; adapt to local needs
$(MAN1)	: $$@.1
	@cp $@.1 $@.l
	@chmod 664 $@.l
	@mv -f $@.l $M
	@echo "$@ man page installed"
$(MAN3)	: $$@.3
	@cp $@.3 $@.l
	@chmod 664 $@.l
	@mv -f $@.l $M
	@echo "$@ man page installed"
	@case $@ in						 \
	fgettr)							 \
		ln $M/fgettr.l $M/fgettr1.l			;\
		ln $M/fgettr.l $M/fgettr2.l			;\
		ln $M/fgettr.l $M/gettr.l			;\
		ln $M/gettr.l  $M/gettr1.l			;\
		ln $M/gettr.l  $M/gettr2.l			 \
	;;							 \
	fputtr)							 \
		ln $M/fputtr.l $M/fputtr1.l			;\
		ln $M/fputtr.l $M/fputtr2.l			;\
		ln $M/fputtr.l $M/puttr.l			;\
		ln $M/puttr.l  $M/puttr1.l			;\
		ln $M/puttr.l  $M/puttr2.l			 \
	;;							 \
	*) # do nothing						 \
	;;						 	 \
	esac
$(MAN9)	: $$@.9
	@cp $@.9 $@.l
	@chmod 664 $@.l
	@mv -f $@.l $M
	@echo "$@ man page installed"

printall:
	@for i in *.[139]		;\
	do				 \
		hroff -man $$i		;\
	done

print:	*.[139]
	@for i in $?			;\
	do				 \
		echo "printing $$i ..."	;\
		hroff -man $$i		;\
	done
	touch $@

list:
	@ls *.[139] | pr -5 -t -l22 | p 

count:
	@ls *.[139] | wc -l
