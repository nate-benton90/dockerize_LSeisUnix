#include "usu.h"
#include "usgrid.h"
#include "subc.h"
#include "par.h"

char *sdoc = 
"GRIDROTATE - grid rotation program					\n"
"\n"
"gridrotate <input.file > output.file [parameters]			\n" 
"\n"
"Required parameters:							\n"
"input.file        name of input old 3D grid  			\n"
"output.file       name of rotated new 3D grid file 			\n"
"Required parameters:							\n"
"x1=               old trace number at 1st corner of rotated master grid\n"
"y1=               old line number at 1st corner of rotated master grid	\n"
"x2=               old trace number at 2nd corner of rotated master grid\n"
"y2=               old line number at 2nd corner of rotated master grid	\n"
"x3=               old trace number at 3rd corner of rotated master grid\n"
"y3=               old line number at 3rd corner of rotated master grid	\n"
"s1=               new trace number at 1st corner of rotated master grid \n"
"                  (minimum trace number of the rotated master grid) \n"
"l1=               new line number at 1st corner of rotated master grid \n"
"                  (minimum line number of the rotated master grid) \n"
"s2=               new trace number at 2nd corner of rotated master grid \n"
"                  (maximum trace number of the rotated master grid) \n"
"l2=               new line number at 2nd corner of rotated master grid \n"
"                  (note l2 must be the same as l1)   \n"
"s3=               new trace number at 3rd corner of rotated master grid \n"
"                  (note s3 must be the same as s1)   \n"
"l3=               new line number at 3rd corner of rotated master grid \n"
"                  (maximum line number of the rotated master grid) \n"
"os=               new trace number of first trace of output rotated 3D grid \n"
"ns=               number of traces per line of output rotated 3D grid \n"
"ds=               trace number increment of output rotated 3D grid \n"
"ol=               new line number of first trace of output rotated 3D grid \n"
"nl=               number of lines of output rotated 3D grid \n"
"dl=               line number increment of output rotated 3D grid \n"
"Optional parameters: 	\n"
"x0=o2             old trace number of first trace of the input 3d grid \n"
"dx=d2             old trace number increment of the input 3d grid \n"
"y0=o3             old line number of first trace of the input 3d grid \n"
"dy=d3             old line number increment of the input 3d grid \n"
"                  (o2,d2,o3,d3 are from the gridheader of the input \n"
"                  3D grid) \n"
"ocdp2=os          new trace number of first trace of output 3D grid \n"
"dcdp2=ds          trace number increment of output 3D grid \n"
"oline3=ol         new line number of first line of output 3D grid \n"
"dline3=dl         line number increment of output 3D grid \n"
"\n"
"NOTES:						 			\n"
"   1. The three corners can be at the output 3D grid or at the output 3D \n"
"      master grid. First corner is always minimum trace and minimum line. \n"
"      Second corner is always maximum trace and minimum line. \n"
"      Third corner is always minimum trace and maximum line. \n"
"   2. Bilinear interpolation is used. \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	9/13/2000   		\n"
;


int main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout;

	float x1, y1, s1, l1;
	float x2, y2, s2, l2;
	float x3, y3, s3, l3;
	float x0,dx,y0,dy;

	usghed usgh;

	float s, l, x, y;
	int ierr;

	float *g00,*g10,*g01,*g11;
	float *grid;

	int i1;
	int n1, n2, n3;
	float o1, o2, o3, d1, d2, d3;
	int is, il;
	float os, ol, ds, dl;
	int ns, nl;

	int ix,iy,ixp1,iyp1;
	float resx, resy;
	float gmin, gmax;

	long long i64;

	float ocdp2, dcdp2, oline3, dline3;

    	/* initialization */
    	initargs(argc,argv);
    	askdoc(1);

	/* read in the grid header */

	file2g(infp);
	file2g(outfp);

	/* get the dimensions of input grid */
	ierr = fgetusghdr(infp, &usgh);
	if(ierr!=0) err("non standard grid header input ");
	o1 = usgh.o1;
	o2 = usgh.o2;
	o3 = usgh.o3;
	d1 = usgh.d1;
	d2 = usgh.d2;
	d3 = usgh.d3;
	n1 = usgh.n1;
	n2 = usgh.n2;
	n3 = usgh.n3;

	/* get input parameters */
	if (!getparfloat("x1",&x1)) err(" x1 missing \n");
	if (!getparfloat("y1",&y1)) err(" y1 missing \n");
	if (!getparfloat("x2",&x2)) err(" x2 missing \n");
	if (!getparfloat("y2",&y2)) err(" y2 missing \n");
	if (!getparfloat("x3",&x3)) err(" x3 missing \n");
	if (!getparfloat("y3",&y3)) err(" y3 missing \n");

	if (!getparfloat("s1",&s1)) err(" s1 missing \n");
	if (!getparfloat("l1",&l1)) err(" l1 missing \n");
	if (!getparfloat("s2",&s2)) err(" s2 missing \n");
	if (!getparfloat("l2",&l2)) err(" l2 missing \n");
	if (!getparfloat("s3",&s3)) err(" s3 missing \n");
	if (!getparfloat("l3",&l3)) err(" l3 missing \n");

	/* get ouptut 2nd and 3rd dimensions */
	if (!getparfloat("os",&os)) err(" os missing \n");
	if (!getparfloat("ds",&ds)) err(" ds missing \n");
	if (!getparint("ns",&ns)) err(" ns missing \n");
	if (!getparfloat("ol",&ol)) err(" ol missing \n");
	if (!getparfloat("dl",&dl)) err(" dl missing \n");
	if (!getparint("nl",&nl)) err(" nl missing \n");

	if (!getparfloat("x0",&x0)) x0 = o2;
	if (!getparfloat("dx",&dx)) dx = d2;
	if (!getparfloat("y0",&y0)) y0 = o3;
	if (!getparfloat("dy",&dy)) dy = d3;

	/* get new trace and line number of output grid */ 
	if (!getparfloat("ocdp2",&ocdp2)) ocdp2 = os;
	if (!getparfloat("dcdp2",&dcdp2)) dcdp2 = ds;
	if (!getparfloat("oline3",&oline3)) oline3 = ol;
	if (!getparfloat("dline3",&dline3)) dline3 = dl;

	fprintf(stderr," grid x-y-s-l locations \n"); 
	fprintf(stderr," s1=%f l1=%f x1=%f y1=%f \n",s1,l1,x1,y1); 
	fprintf(stderr," s2=%f l2=%f x2=%f y2=%f \n",s2,l2,x2,y2); 
	fprintf(stderr," s3=%f l3=%f x3=%f y3=%f \n",s3,l3,x3,y3); 
	fprintf(stderr," os=%f ds=%f ns=%d \n",os,ds,ns); 
	fprintf(stderr," ol=%f dl=%f nl=%d \n",ol,dl,nl); 

	/* memory allocations */
        grid = (float*)malloc(n1*sizeof(float));
        g00 = (float*)malloc(n1*sizeof(float));
        g01 = (float*)malloc(n1*sizeof(float));
        g10 = (float*)malloc(n1*sizeof(float));
        g11 = (float*)malloc(n1*sizeof(float));

	fseek64(infp,0,0);

	fprintf(stderr," start to output ... \n");

	for(il=0;il<nl;il++) {
		/* new line location */
		l = ol + il * dl;
		for(is=0;is<ns;is++) {
			/* new trace location */
			s = os + is * ds; 

			/* new coordinates to old coordinates conversion */
			sl2xy(s1,l1,x1,y1,s2,l2,x2,y2,
			      s3,l3,x3,y3,s,l,&x,&y);

			fprintf(stderr," output s=%f l=%f input x=%f y=%f \n",s,l,x,y);

			x = (x - x0)/dx;
			y = (y - y0)/dy;

			/* find the input trace and line indices */

			ix = x;
			resx = x - ix;
			ixp1 = ix + 1;
			if(ix<0) ix=0;
			if(ix>n2-1) ix=n2-1;
			if(ixp1<0) ixp1=0;
			if(ixp1>n2-1) ixp1=n2-1;

			iy = y;
			resy = y - iy;
			iyp1 = iy + 1;
			if(iy<0) iy=0;
			if(iy>n3-1) iy=n3-1;
			if(iyp1<0) iyp1=0;
			if(iyp1>n3-1) iyp1=n3-1;

			/* read in 4 traces around the output location */
			i64 = (iy*n2+ix);
			i64 = i64*n1*sizeof(float);
			fseek64(infp,i64,0);
			fread(g00,sizeof(float),n1,infp);

			i64 = (iy*n2+ixp1);
			i64 = i64*n1*sizeof(float);
			fseek64(infp,i64,0);
			fread(g10,sizeof(float),n1,infp);

			i64 = (iyp1*n2+ix);
			i64 = i64*n1*sizeof(float);
			fseek64(infp,i64,0);
			fread(g01,sizeof(float),n1,infp);

			i64 = (iyp1*n2+ixp1);
			i64 = i64*n1*sizeof(float);
			fseek64(infp,i64,0);
			fread(g11,sizeof(float),n1,infp);

			/* linear interpolation in x */
			for(i1=0;i1<n1;i1++) {
				g00[i1] = g00[i1] + resx*(g10[i1]-g00[i1]);
				g11[i1] = g01[i1] + resx*(g11[i1]-g01[i1]);
			} 

			/* linear interpolation in y */
			for(i1=0;i1<n1;i1++) {
				grid[i1] = g00[i1] + resy*(g11[i1]-g00[i1]);
			}

			/* calculate min and max output grid values */
			if(is==0 && il==0) {
				gmin = grid[0];
				gmax = grid[0];
			}

			for(i1=0;i1<n1;i1++) {
				if(gmin>grid[i1]) gmin = grid[i1];
				if(gmax<grid[i1]) gmax = grid[i1];
			}

			/* ouput new grid */
			fwrite(grid,sizeof(float),n1,outfp);
		}
	}

	/* update the output gridheader header */
	usgh.o2 = os;
	usgh.d2 = ds;
	usgh.n2 = ns;
	usgh.o3 = ol;
	usgh.d3 = dl;
	usgh.n3 = nl;
	usgh.ocdp2 = ocdp2;
	usgh.dcdp2 = dcdp2;
	usgh.oline3 = oline3;
	usgh.dline3 = dline3;
	usgh.gmin = gmin;
	usgh.gmax = gmax;

	/* output the grid header */
	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err("output grid header error ");


	free(grid);
	free(g00);
	free(g10);
	free(g01);
	free(g11);

	fprintf(stderr,"  output done \n");

	exit(0);

}
