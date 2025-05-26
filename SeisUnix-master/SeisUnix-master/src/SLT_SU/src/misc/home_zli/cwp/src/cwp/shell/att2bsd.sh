#! /bin/sh
# att2bsd - make files in sub-tree inherit the group of their
#          directories (i.e., BSD style instead of default SV standard)
#
# LIKELY BUG: Overflow of buffers, so should not gaily be run from root (/)
#
# RELEASE: This shell may be copied as long as this header is included.
#          The author assumes no responsibility for evil consequences.
#
# BUG: Does not take care of the current directory.  Do that one by hand.
#
# $Author: jkc $
# $Source: /src/system/local/RCS/att2bsd,v $
# $Revision: 1.4 $ ; $Date: 89/02/16 12:13:30 $

BIN=/usr/local/cwp/bin

for i in `ls`
do
	if
		[ -d $i ]
	then
		/bin/chmod g+s $i
		cd $i
		$BIN/att2bsd
		cd ..
	fi
done
exit 0
