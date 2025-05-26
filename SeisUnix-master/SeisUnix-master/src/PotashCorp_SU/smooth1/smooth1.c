/* sutrcount.c */
/* B.Nemeth */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"
#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {"                                                     ",
"                                                                     ",
" SMOOTH1 - smooth a 1D array                                         ",
"                                                                     ",
"   smooth1 < infile > outfile n1=                                    ",
"                                                                     ",
"   Required parameter                                                ",
"                                                                     ",
"   n1= number of x y values                                          ",
"                                                                     ",
"   Optional parameters                                               ",
"                                                                     ",
"   m=1		Smoothing method 1 Gaussian                           ",
"                                2 damped least squars                ",
"                                3 Savitzky-Golay least squares       ",
"                                4 running rectangular window         ",
"                                5 running triangle window            ",
"                                6 median            		      ",
"  f=5		Filter length in points                               ",
"  stinc=1	Increment of the x values; see doc below.             ",
"                                                                     ",
"  The input file must have to format of x y ascii values             ",
"  If the array is segmented, i.e x is not continous than each        ",
"  segment is smoothed separatly                                      ",
"  The array is considered continous if x(i+1)=x(i)+stinc             ",
NULL};

int main( int argc, char *argv[] )
{
	int n1;		/* number of x y values */
	int stinc;	/* x increment  */
	int f;		/* filter length */
	int m;		/* filter method flag */
	float *x;	/* array of x index values */
	float *y; 	/* array of y values */

        /* Initialize */
        initargs(argc, argv);
        requestdoc(1);

    
	MUSTGETPARINT("n1",&n1);
	if( !getparint("stinc",&stinc)) stinc=1;
	if( !getparint("f",&f)) f=5;
	if( !getparint("m",&m)) m=1;
	
	/* allocate arrays */
	x = ealloc1float(n1);
	y = ealloc1float(n1);
	
	/* Read data into the arrays */
	{ int i;
		for(i=0;i<n1;i++) {
			fscanf(stdin," %f %f\n",&x[i],&y[i]);
		}
	}
	/* smooth */
	sm_st(x,y,n1,f,stinc,m);
	
	/* Write out */
	{ int i;
		for(i=0;i<n1;i++) {
			fprintf(stdout," %10.3f %10.3f\n",x[i],y[i]);
		}
	}
	
	free1float(x);
	free1float(y);
   	return EXIT_SUCCESS;
}
