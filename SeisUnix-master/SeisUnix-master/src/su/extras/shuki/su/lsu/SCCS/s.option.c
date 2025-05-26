h29990
s 00000/00000/00017
d D 1.2 88/11/15 14:01:54 shuki 2 1
c 
e
s 00017/00000/00000
d D 1.1 88/04/14 13:47:50 shuki 1 0
c date and time created 88/04/14 13:47:50 by shuki
e
u
U
f e 0
t
T
I 1
extern int xargc;
extern char **xargv;

option(key)
char *key;
{
	int i,c;

	for(i=xargc-1;i>0;i--) {
		c = *xargv[i];
		if( (c=='-') && (strcmp(key,xargv[i]+1)) )
			return(-i);
		if( (c=='+') && (strcmp(key,xargv[i]+1)) )
			return(i);
	}
	return(0);
}
E 1
