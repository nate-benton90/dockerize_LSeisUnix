#! /bin/sh
# suname - get name line from self-docs
# Usage: suname [name]
# Copyright 1988 by Jack K. Cohen
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/suname,v $
# $Revision: 1.2 $ ; $Date: 88/11/30 17:49:52 $

ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su
NAMES=$SU/doc/Headers/HEADERS

PATH=/bin:/usr/bin:$BIN


cmd=`basename $0`

case $# in
0)
	more $NAMES
;;
*)
	grep -i $* $NAMES
;;
esac

echo ""
echo "For more information type: \"program_name <CR>\" or \"man program_name\""
exit 0
