#! /bin/sh
# replace: replace str1 in files with str2, in place
# Kernighan and Pike - page 155

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

case $# in
0|1|2)
	echo 'Usage: replace str1 str2 files' 1>&2
	exit 1
esac

left="$1"; right="$2"; shift; shift

for i
do
	overwrite $i sed "s@$left@$right@g" $i
done

exit 0
