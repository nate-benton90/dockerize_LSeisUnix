#! /bin/sh
# merge2 - put 2 standard size PostScript plots on one page (side by side)
# Usage: merge2 leftfig rightfig
# Author: Craig
# Note: Translation values are hard-coded numbers that work well for 
#	standard size (8.5 x 11) figures.

cmd=`basename $0`

case $# in
	2) # OK
	;;
	*) # echo some documentation
		echo "MERGE 2 figures onto one page (side by side)" 1>&2
		echo "Usage: $cmd leftfig rightfig" 1>&2
		exit 1
	;;
esac

psmerge in=$1 scale=0.5,1.0 translate=0.5,0.0 \
	in=$2 scale=0.5,1.0 translate=4.25,0.0
