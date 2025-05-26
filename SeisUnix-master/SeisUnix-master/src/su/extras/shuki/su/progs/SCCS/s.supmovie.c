h57593
s 00002/00002/00108
d D 1.5 88/11/15 14:03:30 shuki 5 4
c 
e
s 00007/00008/00103
d D 1.4 88/11/06 09:52:40 shuki 4 3
c 
e
s 00002/00000/00109
d D 1.3 88/05/25 14:54:25 shemer 3 2
c with SccsId[]
e
s 00019/00009/00090
d D 1.2 88/05/12 11:36:13 shuki 2 1
c 
e
s 00099/00000/00000
d D 1.1 88/05/11 10:16:52 shuki 1 0
c date and time created 88/05/11 10:16:52 by shuki
e
u
U
t
T
I 1
/*
D 4
 * sypmovie - pack segy trace data into chars for movie
E 4
I 4
 * supmovie - pack data into unsigned chars for movie
E 4
 */

/*********************** self documentation **********************/
char *sdoc = "\
                                                                \n\
sypmovie gpow=0.5 <segy_file >Parfile [out=/data/USER/_sypm]	\n\
                                                                \n\
";
/*****************************************************************/

I 2
#include <stdio.h>
#include <math.h>
E 2
#include "../include/su.h"

int xargc;
char **xargv;
bool verbose=true;
I 3
char *SccsId[]="%W%\t%G%\n";

E 3
D 4

E 4
main(ac,av)
int ac; char **av;
{
D 5
	char out[50];
E 5
I 5
	char *out;
E 5
	unsigned char *ctr;
D 2
	float max, trial, gpow, scale;
	int ntr, fdout, n1,n2,n3;
E 2
I 2
	float max, trial, gpow, scale, maxfac;
	int ntr,n1,n2,n3;
E 2
	int i;
	int infd,outfd;
	Sutrace tr;
	Subhed bh;

	/* Set up standard SU args */
	xargc = ac; xargv = av;
	infd = input();
D 4
	outfd = STDOUT;
E 4
I 4
/* 	outfd = STDOUT; */
E 4

	/* Get parameters */
D 2
	gpow = 0.5; fgetpar("gpow", &gpow);
E 2
I 2
	gpow = 1.0;      fgetpar("gpow",&gpow);
	maxfac = 1.0;    fgetpar("maxfac",&maxfac);
E 2
D 5
	if(!sgetpar("out",out)) strcpy(out,"/scr/shuki/_supmovie");
E 5
I 5
	out="/scr/shuki/_supmovie"; sgetpar("out",&out);
E 5

D 2
	fdout = creat(out,0644);
	if(fdout<0) err("Can't creat %s\n",out);
E 2
I 2
	outfd = creat(out,0644);
	if(outfd<0) err("Can't creat %s\n",out);
E 2

	apass(infd,-1);

	getbh(infd,&bh);

	tr.data = (float*)malloc(bh.ns*bh.esize);

	/* Main loop over segy traces */
	for(max=0.0,ntr=0;gettr(infd,&tr)!=0;ntr++) { 

		/* Power transform to decrease dynamic range */
D 4
		for ( i = 0; i < tr.ns; i++ ) {
E 4
I 4
		for ( i = 0; i < bh.ns; i++ ) {
E 4
			tr.data[i] = (float) (SGN(tr.data[i]) *
			  pow((double) ABS(tr.data[i]), (double) gpow));
		}

		if(max==0.0) {
D 4
			for (max=0.0,i=0;i<tr.ns;i++) { 
E 4
I 4
			for (max=0.0,i=0;i<bh.ns;i++) { 
E 4
				if ( (trial = ABS(tr.data[i])) > max ) { 
					max = trial;
				}
			}
D 2
			if(max>0.0) scale = 255.0/max;
E 2
I 2
/* 			fprintf(stderr,"%d max=%f\n",ntr,max); */
			max /= maxfac;
			if(max>0.0) scale = 127.0/max;
E 2
			else scale = 0.0;
		}

D 2
/* 				fprintf(stderr,"%d max=%f\n",ntr,max); */
E 2

		/* Point output trace at the trace data and pack.
		   Since the chars take less room than the floats,
		   we don't overwrite.
		*/
		ctr = (unsigned char *) tr.data;
D 4
		for ( i = 0 ; i < tr.ns; i++ ) { 
E 4
I 4
		for ( i = 0 ; i < bh.ns; i++ ) { 
E 4

/* 				fprintf(stderr,"trace %d, [%d] %f -> ",ntr,i,tr.data[i]); */

			tr.data[i] *= scale;
I 2
			tr.data[i] += 127.0;
E 2

/* 				fprintf(stderr,"%f ",tr.data[i]); */

D 2
			ctr[i] = (unsigned char) tr.data[i];
E 2
I 2
			if(tr.data[i]<0.0)
				tr.data[i] = 0.0;
			else if(tr.data[i]>255.0)
				tr.data[i] = 255.0;
E 2

I 2
			ctr[i] = (unsigned char) (tr.data[i]);

E 2
/* 				fprintf(stderr,"%d\n",ctr[i]); */

		}

D 2
		if( write(fdout,tr.data,tr.ns)!=tr.ns) err("Write error");
E 2
I 2
D 4
		if( write(outfd,tr.data,tr.ns)!=tr.ns) err("Write error");
E 4
I 4
		if( write(outfd,tr.data,bh.ns)!=bh.ns) err("Write error");
E 4
E 2

	}

D 4
	n1 = tr.ns;
E 4
I 4
	n1 = bh.ns;
E 4
	n2 = ntr;
	n3 = 1;
	printf("n1=%d n2=%d n3=%d\nin=%s\n",n1,n2,n3,out);

	exit(0);
}
E 1
