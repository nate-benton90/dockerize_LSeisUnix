h34683
s 00000/00000/00017
d D 1.2 88/11/15 14:01:19 shuki 2 1
c 
e
s 00017/00000/00000
d D 1.1 88/04/14 14:04:08 shuki 1 0
c date and time created 88/04/14 14:04:08 by shuki
e
u
U
f e 0
t
T
I 1
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
E 1
