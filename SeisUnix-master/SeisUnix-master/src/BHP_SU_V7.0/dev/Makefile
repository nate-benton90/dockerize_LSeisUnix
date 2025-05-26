##################################################################
##
#
# KEYWORDS:  $RCSfile: Makefile,v $
#            $Revision: 1.6 $
#            $Date: 2001/10/12 19:49:58 $
#            $Author: ahmilb $
#
#------------------------------------------------------------------
#
# HISTORY:
#
# $Log: Makefile,v $
# Revision 1.6  2001/10/12 19:49:58  ahmilb
# Use a single list of BHP-SU tools for all types of makes, so that
# multiple lists are  not maintained.
#
# Revision 1.5  2001/10/11 16:18:35  ahmilb
# Latest additions to BHP_SU tools
#
# Revision 1.4  2001/07/25 19:13:23  ahmilb
# Add new apps, change bhpread and bhpio to scripts.
#
# Revision 1.3  2001/02/07 19:55:45  ahglim
# added bhproffread, bhptemplate
# changed bhpio, bhpread, bhpwrite to handle ENDIAN issues
#
# Revision 1.2  2001/02/06 04:55:16  ahglim
# changed gmake to gmake so that it would work on SunOS
#
# Revision 1.1.1.1  2001/01/24 22:46:46  ahglim
# Initial import
#
#
#
###################################################################
#
# Master makefile for BHP seismic unix extensions
#


action = \
	cd       libcube/src ;  gmake  $@; \
	cd ../../libseq/src ;  gmake  $@; \
	cd ../../libapi/src ;  gmake  $@; \
	cd ../../libvel/src ;  gmake  $@; \
	cd ../../bhp3dnmo/src;  gmake  $@; \
	cd ../../bhpaliascube/src;  gmake  $@; \
	cd ../../bhpaliasseq/src;  gmake  $@; \
	cd ../../bhparchive/src;  gmake  $@; \
	cd ../../bhpbin/src;  gmake  $@; \
	cd ../../bhpchw/src;  gmake  $@; \
	cd ../../bhpcopyhdr/src;  gmake  $@; \
	cd ../../bhpcwt/src;  gmake  $@; \
	cd ../../bhpfiletype/src;  gmake  $@; \
	cd ../../bhpfill/src;  gmake  $@; \
	cd ../../bhphorizon/src;  gmake  $@; \
	cd ../../bhpintegrate/src;  gmake  $@; \
	cd ../../bhpio/src;  gmake  $@; \
	cd ../../bhpiocube/src;  gmake  $@; \
	cd ../../bhpioseq/src;  gmake  $@; \
	cd ../../bhploadhdr/src;  gmake  $@; \
	cd ../../bhpmakeevent/src;  gmake  $@; \
	cd ../../bhpmakehdr/src;  gmake  $@; \
	cd ../../bhpmakemodel/src;  gmake  $@; \
	cd ../../bhpmakerc/src;  gmake  $@; \
	cd ../../bhpmerge/src;  gmake  $@; \
	cd ../../bhpmod2seis/src;  gmake  $@; \
	cd ../../bhpmodel/src;  gmake  $@; \
	cd ../../bhpmodvel/src;  gmake  $@; \
	cd ../../bhpnavmerge/src;  gmake  $@; \
	cd ../../bhpread/src;  gmake  $@; \
	cd ../../bhpreadcube/src;  gmake  $@; \
	cd ../../bhpreadseq/src;  gmake  $@; \
	cd ../../bhprestore/src;  gmake  $@; \
	cd ../../bhproffread/src;  gmake  $@; \
	cd ../../bhproffwrite/src;  gmake  $@; \
	cd ../../bhpscan/src;  gmake  $@; \
	cd ../../bhpscancube/src;  gmake  $@; \
	cd ../../bhpscanseq/src;  gmake  $@; \
	cd ../../bhpslice/src;  gmake  $@; \
	cd ../../bhpsmooth/src;  gmake  $@; \
	cd ../../bhpstorehdr/src;  gmake  $@; \
	cd ../../bhpsu2usp/src;  gmake  $@; \
	cd ../../bhpswap/src;  gmake  $@; \
	cd ../../bhpsynth/src;  gmake  $@; \
	cd ../../bhpsynthetic/src;  gmake  $@; \
	cd ../../bhptd/src;  gmake  $@; \
	cd ../../bhptdh/src;  gmake  $@; \
	cd ../../bhptdv/src;  gmake  $@; \
	cd ../../bhptdvel/src;  gmake  $@; \
	cd ../../bhptemplate/src;  gmake  $@; \
	cd ../../bhptranspose/src;  gmake  $@; \
	cd ../../bhpusp2su/src;  gmake  $@; \
	cd ../../bhpvelutil/src;  gmake  $@; \
	cd ../../bhpwavelet/src;  gmake  $@; \
	cd ../../bhpwritecube/src;  gmake  $@; \
	cd ../../bhpwriteseq/src;  gmake  $@; \
	cd ../../newbhpread/src;  gmake  $@; \
	cd ../../runbhpio/src;  gmake  $@;

all:
	$(action)

alpha:
	$(action)

beta:
	$(action)

prod:
	$(action)

depend:
	$(action)

clean:
	$(action)
 
clobber:
	$(action)
