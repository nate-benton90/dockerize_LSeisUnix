typedef enum {false,true} bool;
int xargc; char **xargv;
main(ac,av)
int ac; char **av;
{
	int i;
	long l;
	unsigned short u;
	short h;
	float f;
	double z,v[64];
	char *s;
	bool b;
	int r;

	xargc = ac; xargv = av;

	(void)printf("Echo command line:\n");
	for(i=0;i<ac;i++)
		(void)printf("av[%d] = %s\n",i,av[i]);
	(void)printf("\n");
	
	(void)printf("maxgetpar ... ");
	r = maxgetpar();
	(void)printf("returned %d\n",r);

	(void)printf("igetpar( no_name )  ... ");
	r = igetpar("no_name", &i);
	(void)printf("returned %d\n",r);

	(void)printf("igetpar( i )  ... ");
	r = igetpar("i", &i);
	(void)printf("returned %d.	i=%d\n",r,i);

	(void)printf("lgetpar( l )  ... ");
	r = lgetpar("l", &l);
	(void)printf("returned %d.	l=%d\n",r,l);

	(void)printf("ugetpar( u )  ... ");
	r = ugetpar("u", &u);
	(void)printf("returned %d.	u=%d\n",r,u);

	(void)printf("hgetpar( h )  ... ");
	r = hgetpar("h", &h);
	(void)printf("returned %d.	h=%d\n",r,h);

	(void)printf("fgetpar( f )  ... ");
	r = fgetpar("f", &f);
	(void)printf("returned %d.	f=%f\n",r,f);

	(void)printf("zgetpar( z )  ... ");
	r = zgetpar("z", &z);
	(void)printf("returned %d.	z=%f\n",r,z);

	(void)printf("sgetpar( s )  ... ");
	r = sgetpar("s",&s);
	(void)printf("returned %d.	s=%s\n",r,s);

	(void)printf("bgetpar( b )  ... ");
	r = bgetpar("b",&b);
	(void)printf("returned %d.	b=%d\n",r,b);

	(void)printf("zgetpar( v )  ... ");
	r = zgetpar("v",v);
	(void)printf("returned %d.\n",r);
	for(i=0;i<r;i++)
		(void)printf("	v[%d]=%f\n",i,v[i]);
	exit(0);
}
