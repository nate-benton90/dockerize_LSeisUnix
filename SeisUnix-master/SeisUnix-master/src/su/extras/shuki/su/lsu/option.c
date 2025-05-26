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
