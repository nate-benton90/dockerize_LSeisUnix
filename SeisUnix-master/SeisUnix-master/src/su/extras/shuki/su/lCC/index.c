#include <stdio.h>
char *index(s,t)
char *s;
int t;
{
	while(*s++) {
		if( *s==(char)t) return(s);
	}
	return(NULL);
}
