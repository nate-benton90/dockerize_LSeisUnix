#! /bin/sh 
# sudiff, susum, suprod, suquo - interface to suop2 for backward
#	compatibility
# Author: Jack

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN
cmd=`basename $0`

case $cmd in
sudiff)
	case $# in
	2) suop2 $1 $2 op=diff
	;;
	*) echo "Usage: sudiff file1 file2" 2>&1; exit 1
	;;
	esac
;;
susum)
	case $# in
	2) suop2 $1 $2 op=sum
	;;
	*) echo "Usage: susum file1 file2" 2>&1; exit 1
	;;
	esac
;;
suprod)
	case $# in
	2) suop2 $1 $2 op=prod
	;;
	*) echo "Usage: suprod file1 file2" 2>&1; exit 1
	;;
	esac
;;
suquo)
	case $# in
	2) suop2 $1 $2 op=quo
	;;
	*) echo "Usage: suquo file1 file2" 2>&1; exit 1
	;;
	esac
;;
esac

exit 0
