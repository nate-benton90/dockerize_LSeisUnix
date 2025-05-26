#!/bin/sh
/lib/cpp <su.h |
sed '
	/subhed/,/Subhed/!d
	/{/d
	/}/d
	/*/d
	/;/!d
	s/;//
' |
awk ' {
	#print
	n = index($2,"[")
	#print "n=",n
	if(n!=0) {
		key = substr($2,1,n-1)
	} else {
		key = $2
	}
	#print key
	printf "\tstrcpy(bhdr[%d].key,\"%s\");\n",NR-1,key
	printf "\tstrcpy(bhdr[%d].type,\"%s\");\n",NR-1,$1
	if(n!=0) {
		printf "\tbhdr[%d].offs = (char*)bh.%s - (char*)&bh;\n\n",NR-1,key
	} else {
		printf "\tbhdr[%d].offs = (char*)&bh.%s - (char*)&bh;\n\n",NR-1,key
	}
} END {
	printf "\n\tBH_NK = %d;\n",NR
} '
