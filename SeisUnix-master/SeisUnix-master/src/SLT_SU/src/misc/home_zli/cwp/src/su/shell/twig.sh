#! /bin/sh
# twig - window and sutwig on terminal
# Usage: twig <filename
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/twig,v $
# $Revision: 1.8 $ ; $Date: 89/01/31 22:18:57 $
#
# Bug: Replace spaces in titles, labels with _ because sutwig doesn't
#      seem to interpret strings with quotes put in by sed.


BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN

cmd=`basename $0`

# self-doc if no args and stdin not redirected
if
	[ $# -eq 0 -a -t 0 ]
then
        echo "Usage: $cmd <stdin [min= count= j=] [sutwig options] | pen" 1>&2
	exit 1
fi


# Set sizes by terminal type
case $TERM in
masscomp*)
	sizex=13.8
	sizet=11
	fill=1
;;
tek100)
	sizex=9.4
	sizet=6.2
	fill=1
;;
*) # picked for falco
	sizex=12.0
	sizet=7.0
	fill=0
;;
esac

min="min=1"
j="j=1"
o=""
file=""
for i
do
	case $i in
	min=*)
		min=$i
		shift
	;;
	count=*)
		count=$i
		shift
	;;
	j=*)
		j=$i
		shift
	;;
	*=*\ *)
		ii=`echo $i | sed 's/ /_/g'`
		o="$o $ii"
		shift
	;;
	*=*)
		o="$o $i"
		shift
	;;
	*)
		file=$i
	;;
	esac
done

case $# in
0)	# Correct usage: cmd <file | pen  or ... | cmd | pen ...
	suwind $min $count $j |
	sutwig sizex=$sizex sizet=$sizet fill=$fill label1="" label2="" $o
;;
1)	# Also accept usage: cmd filename
	suwind <$file $min $count $j |
	sutwig sizex=$sizex sizet=$sizet fill=$fill label1="" label2="" $o
;;
*)
        echo "Usage: $cmd <stdin [min= count= j=] [sutwig options] | pen" 1>&2
	exit 1
;;
esac

exit 0
