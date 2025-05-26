#! /bin/sh
# recip - sum opposing offsets in cdp sorted data
# Usage: recip <stdin >stdout
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/recip,v $
# $Revision: 1.8 $ ; $Date: 88/06/07 07:20:31 $

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`
tmp=/usr/tmp/$$.$cmd

# Arrange to remove tmp at shell termination (0) or signal
# Internal trap to ignore 0 is to avoid double remove in case of signal
trap "rm -f $tmp; trap '' 0; exit 1" 0 1 2 3 15

case $1 in
-)
    	echo "Usage: $cmd <stdin >stdout" 1>&2; exit 1
;;
esac

case $# in
0)	# Correct usage: cmd <file >stdout or ... | cmd ...
	suabshw >$tmp
	susort cdp offset <$tmp | surecip
;;
1)	# Also accept usage: cmd filename
	suabshw <$1 >$tmp
	susort cdp offset <$tmp | surecip
;;
*)
    	echo "Usage: $cmd <stdin >stdout" 1>&2; exit 1
;;
esac

exit 0
