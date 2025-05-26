#include "usu.h"
#include "usgrid.h"
#include "subc.h"
#include "par.h"

char *sdoc = 
"GRID2LMK - grid to Landmark program					\n"
"\n"
"grid2lmk <grid.file > landmark.file [parameters]			\n" 
"\n"
"Required parameters:							\n"
"grid.file=         name of grid file 					\n"
"landmark.file=     name of Landmark pick file 				\n"
"Optional parameters:							\n"
"3D master grid parameters:						\n"
"s1=o1                 inline coordinate of 1st corner of 3D grid	\n"
"l1=o2                 crossline coordinate of 1st corner of 3D grid	\n"
"x1=s1                 x coordinate of 1st corner of 3D master grid	\n"
"y1=y1                 y coordinate of 1st corner of 3D master grid	\n"
"s2=o1+(n1-1)*d1       inline coordinate of 2nd corner of 3D grid	\n"
"l2=l1                 crossline coordinate of 2nd corner of 3D grid	\n"
"x2=s2                 x coordinate of 2nd corner of 3D master grid	\n"
"y2=y2                 y coordinate of 2nd corner of 3D master grid	\n"
"s3=s1                 inline coordinate of 3rd corner of 3D grid	\n"
"l3=s1+(n2-1)*d2       crossline coordinate of 3rd corner of 3D grid	\n"
"x3=s3                 x coordinate of 3rd corner of 3D master grid	\n"
"y3=y3                 y coordinate of 3rd corner of 3D master grid	\n"
"			(See Note 1 for details)			\n"
"                      where o1, o2, n1, n2, d1, d2 are from grid header \n"
"remove=0              do not out landmark pick when its grid value is \n"
"                      equal to nullval specified (0=output 1=no)	\n"
"nullval=0.            null value used to reject grids in output	\n"
"\n"
"NOTES:						 			\n"
"1. s and l positions of an input trace are computed using the three    \n"
"   master-grid corner positions                                        \n"
"                                                                       \n"
"        | y                                                            \n"
"        |   .l        * (x4,y4)                                        \n"
"        |    .     .    .                                              \n"
"        |     .  .       .         . s                                 \n"
"       (x3,y3) *          .      .                                     \n"
"        |        .          .  .                                       \n"
"        |         .          * (x2,y2)                                 \n"
"        |          .       .                                           \n"
"        |            .  .                                              \n"
"        |             * (x1,y1)                                        \n"
"        |                                                              \n"
"        |--------------------------------- x                           \n"
"                                                                       \n"
"   (x1,y1) has the smallest s value and the smallest l value           \n"
"              (s1 is usually =0.0, l1 is usually =0.0)                 \n"
"   (x2,y2) has the largest s value and the smallest l value            \n"
"              (s2 is usually =(ncdppl-1)*ds, l2 is usually =0.0)      \n"
"   (x3,y3) has the smallest s value and the largest l value            \n"
"              (s3 is usually =0.0, l3 is usually =(nlines-1)*dl       \n"
"   where ncdppl is number of cdp per line, and ds is cdp spacing,	\n"
"   dl is line spacing. However, if s and l in the grid header		\n"
"   are trace number and line number, instead of distance, ds and dl	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	8/25/94   		\n"
;


main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

	usghed usgh;
	float s, l, x, y, t;
	int ierr;
	float *grids;
	int i1, i2;
	int n1, n2;
	float o1, o2, d1, d2;
	float x1, y1, s1, l1;
	float x2, y2, s2, l2;
	float x3, y3, s3, l3;
	int remove;
	float nullval=0.;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* read in the grid header */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err("non standard grid header input ");
	o1 = usgh.o1;
	o2 = usgh.o2;
	d1 = usgh.d1;
	d2 = usgh.d2;
	n1 = usgh.n1;
	n2 = usgh.n2;

	/* get input parameters */
	if (!getparfloat("s1",&s1)) s1 = o1;
	if (!getparfloat("l1",&l1)) l1 = o2;
	if (!getparfloat("x1",&x1)) x1 = s1;
	if (!getparfloat("y1",&y1)) y1 = l1;
	if (!getparfloat("s2",&s2)) s2 = s2 + (n1-1)*d1;
	if (!getparfloat("l2",&l2)) l2 = l1;
	if (!getparfloat("x2",&x2)) x2 = s2;
	if (!getparfloat("y2",&y2)) y2 = l2;
	if (!getparfloat("s3",&s3)) s3 = s1;
	if (!getparfloat("l3",&l3)) l3 = l1 + (n2-1)*d2;
	if (!getparfloat("x3",&x3)) x3 = s3;
	if (!getparfloat("y3",&y3)) y3 = l3;
	if (!getparfloat("nullval",&nullval)) nullval = 0.0;
	if (!getparint("remove",&remove)) remove = 0;

	fprintf(stderr," grid s-l-x-y locations \n"); 
	fprintf(stderr," s1=%f l1=%f x1=%f y1=%f \n",s1,l1,x1,y1); 
	fprintf(stderr," s2=%f l2=%f x2=%f y2=%f \n",s2,l2,x2,y2); 
	fprintf(stderr," s3=%f l3=%f x3=%f y3=%f \n",s3,l3,x3,y3); 
	fprintf(stderr," o1=%f o2=%f d1=%f d2=%f n1=%d n2=%d\n",
		o1,o2,d1,d2,n1,n2);

	/* memory allocations */
        grids = (float*)malloc(n1*n2*sizeof(float));
	efread(grids,sizeof(float),n1*n2,infp);

	for(i2=0;i2<n2;i2++) {
		l = o2 + i2 * d2;
		for(i1=0;i1<n1;i1++) {
			s = o1 + i1*d1;
			t = grids[i2*n1+i1];
			if(t!=nullval || remove==0) {
				sl2xy(s1,l1,x1,y1,s2,l2,x2,y2,
					s3,l3,x3,y3,s,l,&x,&y);
				fprintf(outfp,
		"                    %10.2f%10.2f%12.2f%12.2f%12.4f   \n",
					l,s,x,y,t);
			}
		}
	}

	exit(0);
}
