#! /bin/sh
# merge2v - put 2 standard size PostScript plots on one page (stacked)
# Usage: merge2v upperfig lowerfig
# Author: Craig
# Note: Translation values are hard-coded numbers that work well for 
#	standard size (8.5 x 11) figures.

cmd=`basename $0`

case $# in
	2) # OK
	;;
	*) # echo some documentation
		echo "MERGE 2 figures onto one page (stacked Vertically)" 1>&2
		echo "Usage: $cmd upperfig lowerfig" 1>&2
		exit 1
	;;
esac

psmerge in=$1 scale=1.0,0.5 translate=0.0,5.25 \
	in=$2 scale=1.0,0.5 translate=0.0,0.25
