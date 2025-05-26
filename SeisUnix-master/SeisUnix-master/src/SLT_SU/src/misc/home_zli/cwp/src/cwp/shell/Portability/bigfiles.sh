#! /bin/sh
# bigfiles - show large files in current tree
# Usage: bigfiles [size]
# Jack K. Cohen, 1988
#
# Note: all this damn ___MARKER_FILE stuff is because if find comes up
#      ls will list everything in the current directory!
#      /etc/skulker on PS/2 might show a better way.

PATH=/bin:/usr/bin
cmd=`basename $0`
tmp=___MARKER_FILE

# Arrange to remove tmp at shell termination (0) or signal
# Internal trap to ignore 0 is to avoid double remove in case of signal
trap "rm -f $tmp; trap '' 0; exit 1" 0 1 2 3 15


touch ___MARKER_FILE
case $# in
0)
	(find . -type f -size +1000 -print ; echo ___MARKER_FILE) | xargs ls -s
;;
1)
	(find . -type f -size +$1 -print ; echo ___MARKER_FILE) | xargs ls -s
;;
*)
	echo "$cmd [size_in_blocks]" 1>&2 ; exit 1
esac
(find . -type d -size +5 -print ; echo ___MARKER_FILE) | xargs ls -ld
rm ___MARKER_FILE
