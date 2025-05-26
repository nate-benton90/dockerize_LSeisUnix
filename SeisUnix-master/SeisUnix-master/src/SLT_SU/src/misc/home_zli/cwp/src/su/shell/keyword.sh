#! /bin/sh
# keyword - look at segy.h
# Usage: keyword [string]
# Copyright 1985 by Jack K. Cohen

ROOT=/usr/local/cwp
BIN=$ROOT/bin
SU=$ROOT/src/su

PATH=/bin:/usr/bin:$BIN
PAGE_PROGRAM=pg


cmd=`basename $0`

# self-doc on - only
if
	[ -t -a X$1 = X- ]
then
    	echo "Usage: $cmd [string]" 1>&2; exit 1
fi

start=${1:-tracl}
exec $PAGE_PROGRAM +/$start $SU/include/segy.h
