h48967
s 00000/00000/00030
d D 1.2 88/11/15 14:01:18 shuki 2 1
c 
e
s 00030/00000/00000
d D 1.1 88/04/14 14:04:07 shuki 1 0
c date and time created 88/04/14 14:04:07 by shuki
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
E 1
