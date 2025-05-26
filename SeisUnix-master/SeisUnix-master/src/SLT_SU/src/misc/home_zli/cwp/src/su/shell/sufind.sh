#! /bin/sh
# sufind - get info from self-docs
# Usage: sufind [-v -n]
# Copyright 1988 by Jack K. Cohen
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/sufind,v $
# $Revision: 1.16 $ ; $Date: 88/11/18 20:23:12 $

ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su
DOC=$SU/doc/Stripped

PATH=/bin:/usr/bin:$BIN
BIN=/usr/local/cwp/bin


cmd=`basename $0`

case $# in
0)
	echo "$cmd - get info from self-docs about SU programs"
    	echo "Usage: $cmd [-v -n] string" 1>&2
	echo "\"$cmd string\" gives brief synopses" 1>&2
	echo "\"$cmd -v string\" verbose hunt for relevant items" 1>&2
	echo "\"$cmd -n name_fragment\" searches for command name" 1>&2
	exit 1
;;
esac

case $1 in
-v)
	shift
	case $# in
	0)
		echo "Need a string" 1>&2
		echo "Usage: $cmd [-v -n] string" 1>&2; exit 1
	;;
	*)
		cd $DOC
		echo ""
		grep -i "$1" *
	;;
	esac
;;
-n)
	shift
	string=`echo $1 | tr "[a-z]" [A-Z]"`
	cd $DOC
	echo ""
	grep "$string" * | sed "s/^.*$1.*://"
;;
*)
	cd $DOC
	echo ""
	if
		grep -i "$1" * >/dev/null
	then
		for i in `grep -li "$1" *`
		do
			cat $i | sed 4q
		done
	fi
;;
esac

echo ""
echo "For more information type: \"program_name <CR>\" or \"man program_name\""
exit 0
