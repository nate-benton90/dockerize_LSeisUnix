#include <stdio.h>
int xargc;
char **xargv;
main(ac,av)
int ac; char **av;
{
	char *s="String";
	int i=1;
	short h=2;
	long l=3;
	float f=4.0;
	double d=5.0;

	xargv = av; xargc = ac;

	(void)fprintf(stderr,"s=%s i=%d h=%d l=%d f=%f d=%f\n",s,i,h,l,f,d);

	warn(__FILE__,__LINE__,"Testing warn: s=%s i=%d h=%d l=%d f=%f d=%f",s,i,h,l,f,d);

	err(__FILE__,__LINE__,"Testing err: s=%s i=%d h=%d l=%d f=%f d=%f",s,i,h,l,f,d);
}
