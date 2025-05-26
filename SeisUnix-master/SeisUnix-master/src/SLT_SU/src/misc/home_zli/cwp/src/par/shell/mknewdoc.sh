#! /bin/sh
# mknewdoc - change self-docs from old form to ANSI
# Usage: mknewdoc file(s)
#
# $Author: jkc $
# $Source$
# $Revision$ ; $Date$

# this is for emergency use if porting to strict ANSI C system that
# doesn't support \newline line continuation.

# works only with Jack's coding style and conventions

# changes sdoc string to ANSI style
# sdocs will likely wrap a few lines after mknewdoc application

# changes declaration of main to ANSI style
# does NOT change declarations of any embedded subs!


savedir=/usr/tmp
tmpfile=$$.mknew

for i
do
	cp $i $savedir/$$.$i

	sed -n '1,/self documentation/p' $i >$tmpfile

	sed -n '/sdoc/,/^"\;$/p' $i |
	sed '
		s/sdoc = "\\/sdoc =/
		/\\n\\$/s/^/" /
		/\\n\\$/s//\\n"/
		/^"\;/s//;/
	' >>$tmpfile

	sed -n '/end self doc/,$p' $i |
	sed '
		s/main(argc, argv)/main(int argc, char **argv)/
		/int argc;/d
	' >>$tmpfile

	mv $tmpfile $i
done

exit 0
