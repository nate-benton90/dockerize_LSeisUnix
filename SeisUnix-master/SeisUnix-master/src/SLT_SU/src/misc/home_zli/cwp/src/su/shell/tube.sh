#!/bin/sh
# tube - device independent pen for vplot
# Usage: ... | vplot_program | tube
#
# $Author: jkc $
# $Source: /src/su/shell/RCS/tube,v $
# $Revision: 2.1 $ ; $Date: 88/07/10 13:13:40 $

cmd=`basename $0`
case $VPEN in
tekpen)
	tekpen $*
;;
pspen)
	pspen $*
;;
x11pen)
	x11pen $*
;;
*)
	echo "$cmd: VPEN variable is not set" 1>&2
	echo "ksh/sh example: export VPEN=pspen (or x11pen or tekpen)" 1>&2
	echo "csh example: setenv VPEN  pspen (or x11pen or tekpen)" 1>&2
	exit 1
;;
esac

exit 0
