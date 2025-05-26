#! /bin/sh
# rcswho - who can access an RCS file
# Usage: rcswho file ...
# Jack K. Cohen, 1988

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN
cmd=`basename $0`

if
	[ -t -a X$1 = X- ]
then
	echo "Usage: $cmd [-] [file ...]" ; exit 1
fi

case $# in
0)
	for i in RCS/*,v
	do
		name=`echo $i | sed 's/,v//'`
		echo "$name: \c"
		rlog $i | sed -n 's/access list: *//p'
	done
;;
*)
	for i
	do
		echo "$i: \c"
		rlog $i | sed -n 's/access list: *//p'
	done
;;
esac

exit 0
