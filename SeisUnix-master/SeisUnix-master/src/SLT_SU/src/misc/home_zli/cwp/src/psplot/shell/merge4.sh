#! /bin/sh
# merge4 - put 4 standard size PostScript plots on one page
# Usage: merge4 ulfig urfig llfig lrfig
# Author: Craig
# Note: Translation values are hard-coded numbers that work well for 
#	standard size (8.5 x 11) figures.

cmd=`basename $0`

case $# in
	4) # OK
	;;
	*) # echo some documentation
		echo "MERGE 4 figures onto one page" 1>&2
		echo "Usage: $cmd ulfig urfig llfig lrfig" 1>&2
		exit 1
	;;
esac

psmerge in=$1 scale=0.5,0.5 translate=0.5,5.25 \
	in=$2 scale=0.5,0.5 translate=4.25,5.25 \
	in=$3 scale=0.5,0.5 translate=0.5,0.25 \
	in=$4 scale=0.5,0.5 translate=4.25,0.25
