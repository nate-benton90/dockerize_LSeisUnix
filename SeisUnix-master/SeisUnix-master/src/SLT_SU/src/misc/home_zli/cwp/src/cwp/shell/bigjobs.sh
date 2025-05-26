#! /bin/sh
# bigjobs - show 6 biggest jobs now running
# Usage: bigjobs
# Jack K. Cohen, 1986

PATH=/bin:/usr/bin

tmp1=/tmp/bigjb.$$.1
tmp2=/tmp/bigjb.$$.2

trap 'rm -f $tmp1 $tmp2; exit 1' 0 1 2 15

ps -av | cut -c1-6,58-67 | sed 's/^ *//' | sort >$tmp1
ps -af | awk '{print $2, $1, $8, $9, $10, $11}' | sort >$tmp2
join $tmp1 $tmp2 |
egrep -v 'COMMAND|bigjobs|root|7q|' |
sort +1 -nr |
awk '
	BEGIN {
		{print "USER	%CPU	%MEM	JOB"}
	}
	{print $4"	"$2"	"$3"	"$5,$6,$7,$8}
' |
sed 7q
