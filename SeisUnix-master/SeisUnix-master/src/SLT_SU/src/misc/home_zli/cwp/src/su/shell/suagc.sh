#! /bin/sh 
# suagc - interface to sugain for backward compatibility
# Author: Jack

BIN=/usr/local/cwp/bin
PATH=/bin:/usr/bin:$BIN
cmd=`basename $0`

sugain gagc=1 $*

exit 0
