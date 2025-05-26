/*
 * supmovie - pack data into unsigned chars for movie
 */

/*********************** self documentation **********************/
char *sdoc = "\
                                                                \n\
sypmovie gpow=0.5 <segy_file >Parfile [out=/data/USER/_sypm]	\n\
                                                                \n\
";
/*****************************************************************/

#include <stdio.h>
#include <math.h>
#include "../include/su.h"

int xargc;
char **xargv;
bool verbose=true;
char *SccsId[]="@(#)supmovie.c	1.5\t11/15/88\n";

main(ac,av)
int ac; char **av;
{
	char *out;
	unsigned char *ctr;
	float max, trial, gpow, scale, maxfac;
	int ntr,n1,n2,n3;
	int i;
	int infd,outfd;
	Sutrace tr;
	Subhed bh;

	/* Set up standard SU args */
	xargc = ac; xargv = av;
	infd = input();
/* 	outfd = STDOUT; */

	/* Get parameters */
	gpow = 1.0;      fgetpar("gpow",&gpow);
	maxfac = 1.0;    fgetpar("maxfac",&maxfac);
	out="/scr/shuki/_supmovie"; sgetpar("out",&out);

	outfd = creat(out,0644);
	if(outfd<0) err("Can't creat %s\n",out);

	apass(infd,-1);

	getbh(infd,&bh);

	tr.data = (float*)malloc(bh.ns*bh.esize);

	/* Main loop over segy traces */
	for(max=0.0,ntr=0;gettr(infd,&tr)!=0;ntr++) { 

		/* Power transform to decrease dynamic range */
		for ( i = 0; i < bh.ns; i++ ) {
			tr.data[i] = (float) (SGN(tr.data[i]) *
			  pow((double) ABS(tr.data[i]), (double) gpow));
		}

		if(max==0.0) {
			for (max=0.0,i=0;i<bh.ns;i++) { 
				if ( (trial = ABS(tr.data[i])) > max ) { 
					max = trial;
				}
			}
/* 			fprintf(stderr,"%d max=%f\n",ntr,max); */
			max /= maxfac;
			if(max>0.0) scale = 127.0/max;
			else scale = 0.0;
		}


		/* Point output trace at the trace data and pack.
		   Since the chars take less room than the floats,
		   we don't overwrite.
		*/
		ctr = (unsigned char *) tr.data;
		for ( i = 0 ; i < bh.ns; i++ ) { 

/* 				fprintf(stderr,"trace %d, [%d] %f -> ",ntr,i,tr.data[i]); */

			tr.data[i] *= scale;
			tr.data[i] += 127.0;

/* 				fprintf(stderr,"%f ",tr.data[i]); */

			if(tr.data[i]<0.0)
				tr.data[i] = 0.0;
			else if(tr.data[i]>255.0)
				tr.data[i] = 255.0;

			ctr[i] = (unsigned char) (tr.data[i]);

/* 				fprintf(stderr,"%d\n",ctr[i]); */

		}

		if( write(outfd,tr.data,bh.ns)!=bh.ns) err("Write error");

	}

	n1 = bh.ns;
	n2 = ntr;
	n3 = 1;
	printf("n1=%d n2=%d n3=%d\nin=%s\n",n1,n2,n3,out);

	exit(0);
}
