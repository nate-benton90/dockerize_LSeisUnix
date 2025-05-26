#! /bin/sh
# maxplot - graph trace maxima
# Usage: maxplot <stdin | pen
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/maxplot,v $
# $Revision: 1.6 $ ; $Date: 88/08/22 22:47:04 $

# Terrible kludge to get ntr (and non-portable). sumax should supply it

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`
tmp=/usr/tmp/$$.$cmd

# Arrange to remove tmp at shell termination (0) or signal
# Internal trap to ignore 0 is to avoid double remove in case of signal
trap "rm -f $tmp; trap '' 0; exit 1" 0 1 2 3 15


case $1 in
-)
    echo "Usage1: $cmd <stdin | tube " 1>&2; 
    echo "Usage2: $cmd <stdin hcopy=1 title="title" | hpen | lp " 1>&2;
    exit 1
;;
esac

case $# in
1)	# Correct usage: cmd <file or ... | cmd ...
	sumax >$tmp
	ntr=`ls -l $tmp | cut -c33-40`
	ntr=`expr $ntr / 4`
	suaddhead <$tmp ns=$ntr | sugraph $*
;;
*)	# Also accept usage: cmd filename
	# and leave room for sugraph parameters
	sumax $1 >$tmp
	ntr=`ls -l $tmp | cut -c33-40`
	ntr=`expr $ntr / 4`
	suaddhead <$tmp ns=$ntr | sugraph $*
;;
esac

exit 0
