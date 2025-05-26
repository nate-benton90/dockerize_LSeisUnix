#! /bin/sh
# maxdiff - find absolute maximum difference in two segy data sets
# Usage: maxdiff file1 file2
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/maxdiff,v $
# $Revision: 1.8 $ ; $Date: 88/11/09 23:46:34 $

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`

case $# in
2)
	sudiff $1 $2 | sumax
;;
*)
	echo "Usage: \"$cmd file1 file2\""
;;
esac

exit 0
