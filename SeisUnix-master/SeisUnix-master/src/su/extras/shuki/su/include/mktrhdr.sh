#!/bin/sh
/lib/cpp <su.h |
sed '
	/sutrace/,/Sutrace/!d
	/{/d
	/}/d
	/*/d
	/;/!d
	s/;//
' |
awk ' {
	printf "\tstrcpy(trhdr[%d].key,\"%s\");\n",NR-1,$2
	printf "\tstrcpy(trhdr[%d].type,\"%s\");\n",NR-1,$1
	printf "\ttrhdr[%d].offs = (char*)&tr.%s - (char*)&tr;\n\n",NR-1,$2
} END {
	printf "\n\tTR_NK = %d;\n",NR
} '
