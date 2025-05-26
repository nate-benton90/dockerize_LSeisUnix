
#include "usu.h"
#include "usgrid.h"
#include "su.h"
#include "segy.h"
#include "par.h"

char *sdoc = 
"SUFOLD - computer cdp fold of input data 				\n" 
"\n"
"sufold foldgrid= [parameters] <input-data 		\n" 
"\n"
"Required parameters:							\n"
"foldgrid=              name of grid file to store folds of input data	\n"
"ftrace=                first trace number within line to compute fold 	\n"
"dtrace=                increment of trace number to compute fold 	\n"
"ntrace=                number of traces per line to compute fold 	\n"
"fline=                 first line number to compute fold 	\n"
"dline=                 increment of line number to compute fold 	\n"
"nline=                 number of lines to compute fold 	\n"
"foffset=               first offset (absolute value) to compute fold 	\n"
"doffset=               increment of offset to compute fold 	\n"
"noffset=               number of offsets to compute fold 	\n"
"\n"
"Optional parameters:							\n"
"tracekey=tracl         segy key word defining trace number within line \n"
"linekey=tracr          segy key word defining line number \n"
" or use the following parameters and the (x,y) of source and receiver \n"
"x1=                    x coordinate of the 1st corner of 3D master grid \n"
"y1=                    y coordinate of the 1st corner of 3D master grid \n"
"t1=                    trace number of the 1st corner of 3D master grid \n"
"l1=                    line number of the 1st corner of 3D master grid \n"
"x2=                    x coordinate of the 2nd corner of 3D master grid \n"
"y2=                    y coordinate of the 2nd corner of 3D master grid \n"
"t2=                    trace number of the 2nd corner of 3D master grid \n"
"l2=                    line number of the 2nd corner of 3D master grid \n"
"x3=                    x coordinate of the 3rd corner of 3D master grid \n"
"y3=                    y coordinate of the 3rd corner of 3D master grid \n"
"t3=                    trace number of the 3rd corner of 3D master grid \n"
"l3=                    line number of the 3rd corner of 3D master grid \n"
"Note: \n"
"     1. Output grid is fold(trace,line,offset) with trace as the most \n"
"        rapidly axis. \n"
"     2. When x1,y1,...l3 are specified, tracekey and linekey are ignored.  \n"
"        The fold will be calculated using the source and receiver (x,y). \n"
"\n"
"AUTHOR:		Zhiming Li,       ,	2/24/99		\n"
;

main(int argc, char **argv)
{
	segytrace tr;
	usghed usgh;
	FILE *infp=stdin;
	FILE *fgfp;
	char *fgrid;
	float *grid;


	float x0,y0,dx,dy,f0,df;
	int nx,ny,nf;
	int ix, iy, iof, i;
	float x, y, offset;
	float fmin, fmax;

	String tracekey="tracl", linekey="tracr", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk, trstart, lnstart;

	float x1, x2, x3, y1, y2, y3, l1, l2, l3, t1, t2, t3;
	float t, l; 
	int ixy;

  	/* get parameters */
   	initargs(argc,argv);
   	askdoc(1);

   	if(!getparstring("foldgrid",&fgrid)) err(" foldgrid missing"); 
   	if(!getparfloat("ftrace",&x0)) err(" ftrace missing");
   	if(!getparfloat("dtrace",&dx)) err(" dtrace missing");
   	if(!getparint("ntrace",&nx)) err(" ntrace missing");
   	if(!getparfloat("fline",&y0)) err(" fline missing");
   	if(!getparfloat("dline",&dy)) err(" dline missing");
   	if(!getparint("nline",&ny)) err(" nline missing");
   	if(!getparfloat("foffset",&f0)) err(" foffset missing");
   	if(!getparfloat("doffset",&df)) err(" doffset missing");
   	if(!getparint("noffset",&nf)) err(" noffset missing");

	getparstring("tracekey",&tracekey);	
	getparstring("linekey",&linekey);
	if(getparfloat("x1",&x1) && getparfloat("x2",&x2) && getparfloat("x3",&x3) 
	&& getparfloat("y1",&y1) && getparfloat("y2",&y2) && getparfloat("y3",&y3) 
	&& getparfloat("t1",&t1) && getparfloat("t2",&t2) && getparfloat("t3",&t3) 
	&& getparfloat("l1",&l1) && getparfloat("l2",&l2) && getparfloat("l3",&l3)){
		ixy = 1;
	} else {
		ixy = 0;
	}

	/* memory allocations */
	grid = (float*) emalloc(nx*ny*nf*sizeof(float));
	bzero(grid,nx*ny*nf*sizeof(float));

	fprintf(stderr," \n");
	fprintf(stderr," sufold parameters \n");
	fprintf(stderr," =================== \n");


	if((fgfp = fopen(fgrid,"r"))!=NULL) {
		fprintf(stderr," open foldgrid=%s ... \n",fgrid);
		file2g(fgfp);
		if(fgetusghdr(fgfp,&usgh)!=0) err("error fgetusghdr");
		if(x0!=usgh.o1) err(" check foldgrid on o1");
		if(dx!=usgh.d1) err(" check foldgrid on d1");
		if(nx!=usgh.n1) err(" check foldgrid on n1");
		if(y0!=usgh.o2) err(" check foldgrid on o2");
		if(dy!=usgh.d2) err(" check foldgrid on d2");
		if(ny!=usgh.n2) err(" check foldgrid on n2");
		if(f0!=usgh.o3) err(" check foldgrid on o3");
		if(df!=usgh.d3) err(" check foldgrid on d3");
		if(nf!=usgh.n3) err(" check foldgrid on n3");
		fseek2g(fgfp,0,0);
		fread(grid,sizeof(float),nx*ny*nf,fgfp);
		fclose(fgfp);
	} else {
		fprintf(stderr," initilize foldgrid=%s ... \n",fgrid);
		bzero((char*)&usgh,100);
		usgh.scale = 1;
		usgh.dtype = 4;
		usgh.o1 = x0;
		usgh.d1 = dx;
		usgh.n1 = nx;
		usgh.o2 = y0;
		usgh.d2 = dy;
		usgh.n2 = ny;
		usgh.o3 = f0;
		usgh.d3 = df;
		usgh.n3 = nf;
		usgh.n4 = 1;
		usgh.n5 = 1;
	}

	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);
	
	/* make file size to be able to exceed 2 G */
	file2g(infp);

	/* read in first trace for nt and dt */
    if (!fgettr(infp,&tr))  err("can't get first trace");

	if(ixy==0) {
	fprintf(stderr," tracekey=%s linekey=%s \n",tracekey,linekey);
	} else {
	fprintf(stderr," x1=%g y1=%g t1=%g l1=%g \n",x1,y1,t1,l1);
	fprintf(stderr," x2=%g y2=%g t2=%g l2=%g \n",x2,y2,t2,l2);
	fprintf(stderr," x3=%g y3=%g t3=%g l3=%g \n",x3,y3,t3,l3);
	}
	fprintf(stderr," foldgrid=%s \n",fgrid);
	fprintf(stderr," ftrace=%g dtrace=%g ntrace=%d \n",x0,dx,nx);
	fprintf(stderr," fline=%g dline=%g nline=%d \n",y0,dy,ny);
	fprintf(stderr," foffset=%g doffset=%g noffset=%d \n",f0,df,nf);


	/* loop over input traces */
	do {
	/* compute velocity at output location */
	/* via bilinear interpolation */
		if(ixy==0) {
			gethval(&tr,indxtrk,&trkval);
			ix = vtoi(trktype,trkval);
			gethval(&tr,indxlnk,&lnkval);
			iy = vtoi(lnktype,lnkval);
			x = (ix - x0)/dx + 0.5;
			ix = x;
			y = (iy - y0)/dy + 0.5;
			iy = y;
		} else {
			x = 0.5*(tr.sx + tr.gx);
			y = 0.5*(tr.sy + tr.gy);
			if(tr.scalco>1) {
				x = x  * tr.scalco;
				y = y  * tr.scalco;
			} else if(tr.scalco<0) {
				x = - x  / tr.scalco;
				y = - y  / tr.scalco;
			}
			xy2sl(x1,y1,t1,l1,x2,y2,t2,l2,x3,y3,t3,l3,x,y,&t,&l);
/*
fprintf(stderr,"x=%g y=%g t=%g l=%g cdp=%d ep=%d\n",x,y,t,l,tr.cdp,tr.ep);
*/
			x = (t - x0)/dx + 0.5;
			ix = x;
			y = (l - y0)/dy + 0.5;
			iy = y;
		}

		offset = fabs(tr.offset);
		offset = (offset - f0)/df + 0.5;
		iof =  offset;


		if(ix>=0 && ix<nx &&
		   iy>=0 && iy<ny &&
		   iof>=0 && iof<nf) {
			
			grid[ix+iy*nx+iof*nx*ny] += 1.;
		}

	} while(fgettr(infp,&tr));

	fgfp = fopen(fgrid,"w");
	fseek2g(fgfp,0,0);
	fwrite(grid,sizeof(float),nx*ny*nf,fgfp);

	for(i=0;i<nx*ny*nf;i++) {
		if(fmin>grid[i]) fmin = grid[i];
		if(fmax<grid[i]) fmax = grid[i];
	}

	usgh.gmin = fmin;
	usgh.gmax = fmax;
	if(fputusghdr(fgfp,&usgh)!=0) err("error fputusghdr");

	free(grid);
	return 0;

}
