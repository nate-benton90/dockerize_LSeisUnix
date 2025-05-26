#! /bin/sh
# rmaxdiff - find percentage maximum difference in two segy data sets
# Usage: rmaxdiff file1 file2
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/rmaxdiff,v $
# $Revision: 1.2 $ ; $Date: 88/09/28 21:22:41 $

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`

case $# in
2)
	echo "max on first, maxdiff and percent:"
	denom=`sumax <$1`
	num=`sudiff $1 $2 | sumax`
	per=`percent $num $denom`
	echo $denom , $num , $per
;;
*)	# includes "cmd -"
	echo "Usage is \"rmaxdiff file1 file2\""
	echo
	echo "This shell can be used to compare the su data from"
	echo "a \"new\" and \"old\" version of an suprogram when the"
	echo "changes have affected the floating point operation"
	echo "order, so that the outputs are shown to differ by \"cmp\"."
	echo "If rmaxdiff produces an answer less than about 7.0e-5,"
	echo "one has some evidence that the changes have not harmed"
	echo "the program."
;;
esac

exit 0
