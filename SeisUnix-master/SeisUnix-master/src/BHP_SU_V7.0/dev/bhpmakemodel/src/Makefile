# LICENSE FOR BHP SU Suite of Programs

# The following is the license that applies to the copy of the software hereby
# provided to Licensee. BHP's Software Manager may be contacted at the following
# address:

# Colorado School of Mines
# 1500 Illinois Street
# Golden, Colorado 80401
# Attention: John Stockwell
# e-mail: john@dix.mines.edu
# Telephone: 303-273-3049

# Copyright 2001 BHP Software. All rights reserved.

# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software") to deal
# in the Software, without restriction, except as hereinafter provided,
# including without limitation the rights to use, copy, modify merge,
# publish, and distribute the Software and to permit persons
# to whom the Software is furnished to do so, provided that the above
# copyright notice and this permission notice appear in all copies of the
# Software and that both the above copyright notice and this permission
# notice appear in supporting documentation. No charge may be made for
# any redistribution of the Software, including modified or merged versions
# of the Software. The complete source code must be included
# in any distribution. For an executable file, complete source code means the
# source code for all modules it contains.

# Modified or merged versions of the Software must be provided to the Software
# Manager, regardless of whether such modified or merged versions are
# distributed to others.

# THE SOFTWARE IS PROVIDED 'AS IS" WITHOUT WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
# MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
# NONINFRINGMENT OF THIRD PARTY RIGHTS. IN NO EVENT SHALL THE
# COPYRIGHT HOLDER INCLUDED IN THIS NOTICE BE LIABLE FOR ANY CLAIM OR
# ANY SPECIAL INDIRECT OR CONSEQUENTIAL DAMAGES, OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER
# IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
# ARISING OUT OF OR IN CONNECTION WITH THE USE OF PERFORMANCE OF
# THIS SOFTWARE.

# The name of the copyright holder shall not be used in advertising or
# otherwise to promote the use or other dealings in this Software, without
# prior written consent of the copyright holder.

##################################################################
##
#
# KEYWORDS:  $RCSfile: Makefile,v $
#            $Revision: 1.1 $
#            $Date: 2001/07/25 18:52:07 $
#            $Author: ahmilb $
#
#------------------------------------------------------------------
#
# HISTORY:
#
# $Log: Makefile,v $
# Revision 1.1  2001/07/25 18:52:07  ahmilb
# bhpio utility for cube files.
#
# Revision 1.1  2001/04/18 20:00:19  ahmilb
# New Hypercube BHPIO
#
# Revision 1.7  2001/03/02 21:16:54  ahglim
# makedepend changes
#
# Revision 1.6  2001/02/14 20:08:14  ahhayg
# First compile of BHP_su apps for IRIX.
#
# Revision 1.5  2001/02/09 22:28:48  ahglim
# compiled release V1_2 to run on Solaris
#
# Revision 1.4  2001/02/09 22:07:53  ahglim
# first working version on Linux (corrected ENDIAN problem)
# be careful -- changed bhpread default to BIG ENDIAN from native
#
# Revision 1.3  2001/02/07 19:55:45  ahglim
# added bhproffread, bhptemplate
# changed bhpio, bhpread, bhpwrite to handle ENDIAN issues
#
# Revision 1.2  2001/02/06 03:38:16  ahglim
# corrected comment problem
#
# Revision 1.1.1.1  2001/01/24 22:46:46  ahglim
# Initial import
#
#
#
###################################################################

include ../../Makefile_template_part1

PROG = bhpmakemodel
RCSfiles.c = ${PROG}.c
OBJS.c = obj/$(PLATFORMID)/${PROG}.o
PKG_LIB = $(CWPLIB) $(CLIB)

include ../../Makefile_template_part2
