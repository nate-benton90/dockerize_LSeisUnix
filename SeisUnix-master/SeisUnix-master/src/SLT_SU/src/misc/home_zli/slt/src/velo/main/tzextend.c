#include "par.h"

char *sdoc = 
"TZEXTEND - T-Z pairs extend program 					\n"
"\n"
"tzextend <tz.file [parameters] >tz.ext.file			\n" 
"\n"
"Required parameters:							\n"
"tz.file=           file name of time, depth pairs    \n"
"tz.ext.file=       file name of extended time, depth pairs 			\n"
"a=                 extrapolation coefficient				\n"
"b=                 extrapolation coefficient				\n"
"c=                 extrapolation coefficient				\n"
"                   z_ext = a + b * t_ext + c * t_ext**2	\n"
"tmin=              minimum extended time					\n"
"tmax=              maximum extended time					\n"
"dt=                increment of extended time				\n"
"Optional parameters										\n"
"orig=1             1=original t-z pairs will be preserved	\n"
"                   0=replaced with computed t-z values		\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	4/4/96  		\n"
;


main(int argc, char **argv)
{
	char buf[81];
	float a, b, c, tmin, tmax, dt;
	float *t, *z, tt, zz;
	float tmp;
	int np, orig, nt, it;

   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */

	if( !getparfloat("a",&a) ) err(" a missing");
	if( !getparfloat("b",&b) ) err(" b missing");
	if( !getparfloat("c",&c) ) err(" c missing");
	if( !getparfloat("tmin",&tmin) ) err(" tmin missing");
	if( !getparfloat("tmax",&tmax) ) err(" tmax missing");
	if( !getparfloat("dt",&dt) ) err(" dt missing");
	if( !getparint("orig",&orig) ) orig = 1;

	tmp = (tmax-tmin)/dt + 1.5;
	nt = tmp;

	t = (float*) malloc(nt*sizeof(float));
	z = (float*) malloc(nt*sizeof(float));

	for(it=0;it<nt;it++) {
		t[it] = tmin + it*dt;
		z[it] = a + b * t[it] + c * t[it] * t[it];
	}
	
	if(orig==0) {
		for(it=0;it<nt;it++) printf(" %g %g \n",t[it],z[it]);
	} else {
		gets(buf);
		np = 0;
		do {
			sscanf(buf,"%g %g\n",&tt,&zz);
			if(tt>tmin && np==0) {
				for(it=0;it<nt;it++) {
					if(t[it]<tt) printf(" %g %g \n",t[it],z[it]);
				}
			} else {
				printf(" %g %g \n",tt,zz);
			}
			np = np + 1;
		} while(gets(buf));
		fprintf(stderr," total of %d t-z pairs read from input file \n",np);
		if(tt<tmax) {
				for(it=0;it<nt;it++) {
					if(t[it]>tt) printf(" %g %g \n",t[it],z[it]);
				}
		}
	}

	free(t);
	free(z);
	exit(0);
}
