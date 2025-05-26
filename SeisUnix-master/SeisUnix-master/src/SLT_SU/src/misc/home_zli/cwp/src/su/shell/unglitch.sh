#! /bin/sh
# unglitch - zonk outliers in data
# Usage: unglitch filename
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/unglitch,v $
# $Revision: 1.4 $ ; $Date: 88/06/07 07:20:36 $

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`

CLIPLEVEL=0.99

case $1 in
-)
        echo "Usage: $cmd <stdin" 1>&2; exit 1
;;
esac

case $# in
0)	# Correct usage: cmd <file or ... | cmd ...
	sugain qclip=$CLIPLEVEL
;;
1)	# Also accept usage: cmd filename
	sugain <$1 qclip=$CLIPLEVEL
;;
*)
        echo "Usage: $cmd <stdin" 1>&2; exit 1
;;
esac

exit 0
