#include "usu.h"
#include "usgrid.h"
#include "subc.h"
#include "par.h"

char *sdoc = 
"GRIDFILTER - grid filter program					\n"
"\n"
"gridfilter <input.file > output.file [parameters]			\n" 
"\n"
"Required parameters:							\n"
"input.file            name of input 3D grid file 			\n"
"output.file           name of filtered 3D grid file 			\n"
"Required parameters:							\n"
"NONE \n"
"Optional parameters: 	\n"
"op=0                  0=median 1=mean 		\n"
"w1=5                  window length (in samples) along 1st dimension \n"
"w2=5                  window length (in samples) along 2nd dimension \n"
"w3=5                  window length (in samples) along 3rd dimension \n"
"si1=1                 starting index of 1st dimension to apply filter \n"
"ni1=n1                ending index of 1st dimension to apply filter \n"
"si2=1                 starting index of 2nd dimension to apply filter \n"
"ni2=n2                ending index of 2nd dimension to apply filter \n"
"si3=1                 starting index of 3rd dimension to apply filter \n"
"ni3=n3                ending index of 3rd dimension to apply filter \n"
"\n"
"NOTES:						 			\n"
"   1. n1, n2, n3 are obtained from the gridheader of the input grid. \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	10/4/2000   		\n"
;


main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;
	usghed usgh;
	int ierr;

	float *grid, *g, *g1;

	int i1, i2, i3;
	int k1, k2, k3;
	int h1, h2, h3;
	int n1, n2, n3;
	int j1, j2, j3;
	int m1, m2, m3;
	int i;
	float gmin, gmax;
	float tmp;
	int op, w1, w2, w3;
	int nz, iz;

	int si1, si2, si3, ni1, ni2, ni3;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* large than 2 GB files */
	file2g(infp);
	file2g(infp);

	/* read in the grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err("non standard grid header input ");

	/* get the dimensions of input grid */
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3;

	/* get input parameters */
	if (!getparint("op",&op)) op = 0;
	if (!getparint("w1",&w1)) w1 = 5;
	if (!getparint("w2",&w2)) w2 = 5;
	if (!getparint("w3",&w3)) w3 = 5;
	if(w1<1) w1 = 1; if(w1>n1) w1 = n1;
	if(w2<1) w2 = 1; if(w2>n2) w2 = n2;
	if(w3<1) w3 = 1; if(w3>n3) w3 = n3;

	if (!getparint("si1",&si1)) si1 = 1; if(si1<1)si1=1; if(si1>n1)si1=n1;
	if (!getparint("si2",&si2)) si2 = 1; if(si2<1)si2=1; if(si2>n2)si2=n2;
	if (!getparint("si3",&si3)) si3 = 1; if(si3<1)si3=1; if(si3>n3)si3=n3;
	if (!getparint("ni1",&ni1)) ni1 = n1; if(ni1<1)ni1=1;if(ni1>n1)ni1=n1;
	if (!getparint("ni2",&ni2)) ni2 = n2; if(ni2<1)ni2=1;if(ni2>n2)ni2=n2;
	if (!getparint("ni3",&ni3)) ni3 = n3; if(ni3<1)ni3=1;if(ni3>n3)ni3=n3;

	/* memory allocations */
	if(n1*n2*n3>1024*1024*1024/4) 
		err(" input grid too big; subsample recommended \n");

        grid = (float*)emalloc(n1*n2*n3*sizeof(float));
        g = (float*)emalloc(w1*w2*w3*sizeof(float));
	g1 = (float*) emalloc(n1*sizeof(float));

	fseek64(infp,0,0);
	efread(grid,sizeof(float),n1*n2*n3,infp);

	h1 = w1/2;
	h2 = w2/2;
	h3 = w3/2;

	nz = w1*w2*w3;
	iz = (50*nz/100.);

	si1 = si1 - 1;
	si2 = si2 - 1;
	si3 = si3 - 1;

	for (i3=0;i3<n3;i3++) {
	for (i2=0;i2<n2;i2++) {

	    k3 = i3 - h3;
	    k2 = i2 - h2;

	    for(i1=0;i1<n1;i1++) g1[i1] = grid[i1+i2*n1+i3*n1*n2];

	    if( i2>=si2 && i2<ni2 && i3>=si3 && i3<ni3 ) {

	    for (i1=si1-1;i1<ni1;i1++) {

	        i = 0;
		k1 = i1 - h1;

		for (j3=k3;j3<k3+w3;j3++) {
		    m3=j3; 
		    if(m3<0)m3=0;
		    if(m3>n3-1)m3=n3-1;

		    for (j2=k2;j2<k2+w2;j2++) {
		    	m2=j2; 
		        if(m2<0)m2=0;
		        if(m2>n2-1)m2=n2-1;

			for (j1=k1;j1<k1+w1;j1++) {
		    	    m1=j1; 
		            if(m1<0)m1=0;
		            if(m1>n1-1)m1=n1-1;

			    g[i] = grid[m1+m2*n1+m3*n1*n2];
			    i = i + 1;

			}
		    }
		}
		if(op==0) {
			qkfind(iz,nz,g);
			g1[i1] = g[iz];
		} else if(op==1) {
			tmp = 0.;
			for(i=0;i<nz;i++) {
				tmp = tmp + g[i];
			}
			g1[i1] = tmp/nz;
		}
	    }
	    }
	    if(i2==0 && i3==0) {
		gmin = g1[0];
		gmax = g1[0];
	    }
	    for(i1=0;i1<n1;i1++) {
		if(gmin>g1[i1]) gmin = g1[i1];
		if(gmax<g1[i1]) gmax = g1[i1];
	    }
	    fwrite(g1,sizeof(float),n1,outfp);
	}
	}

	/* update the output gridheader header */
	usgh.gmin = gmin;
	usgh.gmax = gmax;

	/* output the grid header */
	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err("output grid header error ");


	free(grid);
	free(g);
	free(g1);

	exit(0);

}
