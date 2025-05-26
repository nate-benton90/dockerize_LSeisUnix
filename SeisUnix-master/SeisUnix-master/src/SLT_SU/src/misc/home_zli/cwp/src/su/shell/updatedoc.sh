#! /bin/sh
# updatedoc - put self-docs in su/doc & stripped versions in su/doc/Stripped
# Usage: updatedoc
# Copyright 1988 by Jack K. Cohen
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/updatedoc,v $
# $Revision: 1.5 $ ; $Date: 88/06/07 07:20:37 $

ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su
SRC=$SU/main
DOC=$SU/doc
STRIP=$DOC/Stripped
HEAD=$DOC/Headers

PATH=/bin:/usr/bin:$BIN


>$STRIP/HEADERS
>$HEAD/HEADERS
for i in $SRC/*.c
do
	name=`basename $i .c`

	sed -n '/self documentation/,/^$/p' $i |
	tee $DOC/$name |
	sed '
		/\*\*\*\*\*/d
		/^char \*sdoc/d
		/^string sdoc/d
		/^"\;$/d
		/^"\\n";$/d
		/;/d
		s/[	 ]*\\n\\//
		s/"[	 ]*\\n"//
		s/^" //
		s/^"//
		s/[	 ]*\\n"//
		/^$/d
	' >$STRIP/$name
	
	sed 1q $STRIP/$name >>$HEAD/HEADERS
done

exit 0
