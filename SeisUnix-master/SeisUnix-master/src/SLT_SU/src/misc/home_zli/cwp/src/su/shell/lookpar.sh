#! /bin/sh
# lookpar - show getpar lines in SU code with defines evaluated
# Usage: lookpar filename ...
#
# $Author: jkc $
# $Source: /usr/local/src/su/shell/RCS/lookpar.sh,v $
# $Revision: 1.2 $ ; $Date: 90/11/11 10:08:55 $

# Note: "cc -E" could be used on many (all?) systems instead of /lib/cpp

ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su
I=$ROOT/include

PATH=/bin:/usr/bin:$BIN


cmd=`basename $0`

case $# in
0)
	echo "Usage: $cmd file(s)" 1>&2; exit 1
;;
esac

/lib/cpp -I$I $* | grep getpar |
grep -v initgetpar | grep -v "*name" | grep -v "*s"
#grep -v extern | grep -v initgetpar | grep -v "*name" | grep -v "*s"
exit 0
