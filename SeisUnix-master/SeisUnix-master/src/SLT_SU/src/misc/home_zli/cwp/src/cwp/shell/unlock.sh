#! /bin/sh
# unlock - unlock an RCS file
# Usage: unlock file ...
# Jack K. Cohen, 1988

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN
cmd=`basename $0`

case $# in
0)	echo Usage: $cmd file ... 1>&2; exit 1
esac

echo ""
echo Permission will be asked before overwriting current versions
echo ""
for i
do
	rcs -u $i
	co $i
done

exit 0
