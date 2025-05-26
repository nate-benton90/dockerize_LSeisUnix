#! /bin/sh
# mkolddoc - change self-docs from ANSI to pre-ANSI
# Usage: mkolddoc file(s)
#
# $Author: jkc $
# $Source$
# $Revision$ ; $Date$

# this is for emergency use if porting to pre-ANSI C system that
# doesn't support "..." "..." string concatenation, etc.

# works only with Dave's coding style and conventions

# changes sdoc string to pre-ANSI style
# changes declaration of main to pre-ANSI style
# does NOT change declarations of any embedded subs!


savedir=/usr/tmp
tmpfile=$$.mkold

for i
do
	cp $i $savedir/$$.$i

	sed -n '1,/^$/p' $i |
	sed '
		s/char \*sdoc =/&"\\/
		/^"\\n";$/s//";/
		/^".*\\n"$/s/\\n"/\\n\\/
		/\\n\\$/s/^"//
	' >$tmpfile

	sed -n '/^$/,$p' $i |
	sed '
		s/^{/int argc; char **argv; {/
		s/main (int argc, char \*\*argv)/main(argc, **argv)/
		s/main(int argc, char \*\*argv)/main(argc, **argv)/
	' >>$tmpfile

	mv $tmpfile $i
done

exit 0
