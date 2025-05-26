char *sdoc =
"GRIDSLICE - obtain grid values along a landmark horizon \n"
"\n"
"gridslice <infile [required parameters] >outfile \n"
"\n"
"Required Parameters:\n"
"infile=        name of the input 3D grid file	\n"
"outfile=       name of the output landmark file	\n"
"lmkfile=       name of the input landmark file defining the horizon \n"
"Optional Parameters: \n"
"xlpos=2        column position of landmark file defining output trace  \n"
"ylpos=1        column position of landmark file defining output line  \n"
"tzpos=5        column position of landmark file defining output t or z \n"
"gridpos=5      column position of output grid value \n"
"o1, d1, n1     staring, increment and number of t/z samples in the grid \n"
"o2, d2, n2     staring, increment and number of traces in the grid \n"
"o3, d3, n3     staring, increment and number of lines in the grid \n"
"Notes:			\n"
" 1. o1, d1, n1, o2, d2, n2, o3, d3 and n3 will be defaulted to the input \n"
"    grid header values, if not given \n"
"\n"
"AUTHOR:  Zhiming Li,         12/9/96			\n"
"\n";
#include "usgrid.h"
#include "par.h"

int main(int argc, char **argv)
{
	usghed ugh;
	FILE *infp,*outfp,*lmkfp;
	char *infile, *outfile, *lmkfile; 
	float *grids, *grid;
	int ierr, nf, nc;
	char *cbuf; 
	float *fbuf;
	int xlpos=2,ylpos=1,tzpos=5, gridpos=5;

	int n1,n2,n3;
	float d1,d2,d3;
	float o1,o2,o3;

	float xl, yl, tz, tmp, sc, val;
	int itmp; 

	/* hook up getpar */
	initargs(argc,argv);
	askdoc(0);

	/* get parameters */
	if(getparstring("infile",&infile)) {
		infp = efopen(infile,"r");
	} else {
		infp = stdin;
	}
	file2g(infp);
	ierr = fgetusghdr(infp,&ugh);
	ierr = fgetusghdr(infp,&ugh);
    if(ierr!=0) err(" input grid header error ");
	if(getparstring("outfile",&outfile)) {
		outfp = efopen(outfile,"w");
	} else {
		outfp = stdout;
	}
	if(!getparstring("lmkfile",&lmkfile)) err(" lmkfile missing ");
	lmkfp = efopen(lmkfile,"r");

	if( !getparint("xlpos",&xlpos) ) xlpos=2; xlpos -= 1;
	if( !getparint("ylpos",&ylpos) ) ylpos=1; ylpos -= 1;
	if( !getparint("tzpos",&tzpos) ) tzpos=5; tzpos -= 1;
	if( !getparint("gridpos",&gridpos) ) gridpos=5; gridpos -= 1;

	if( !getparint("n1",&n1) ) n1 = ugh.n1;
	if( !getparint("n2",&n2) ) n2 = ugh.n2;
	if( !getparint("n3",&n3) ) n3 = ugh.n3;
	if( !getparfloat("o1",&o1) ) o1 = ugh.o1;
	if( !getparfloat("o2",&o2) ) o2 = ugh.o2;
	if( !getparfloat("o3",&o3) ) o3 = ugh.o3;
	if( !getparfloat("d1",&d1) ) d1 = ugh.d1;
	if( !getparfloat("d2",&d2) ) d2 = ugh.d2;
	if( !getparfloat("d3",&d3) ) d3 = ugh.d3;

	nf = 10;
	nc = 200;
	/* memory allocations */
	grids = (float*) emalloc(n1*n2*n3*sizeof(float));
	grid = (float*) emalloc(n1*sizeof(float));
	fbuf = (float *) malloc(nf*sizeof(float));
	cbuf = (char *) emalloc(nc*sizeof(char));

	efread(grids,sizeof(float),n1*n2*n3,infp);

	fgets(cbuf,nc,lmkfp);
	do {
		sscanf(cbuf,"%f %f %f %f %f",
					&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],&fbuf[4]);
		xl = fbuf[xlpos];
		yl = fbuf[ylpos];
		tz = fbuf[tzpos];
		bilint_(&n1,&n2,&n3,&o2,&o3,&d2,&d3,&xl,&yl,grids,grid);
		tmp = (tz-o1)/d1;
		itmp = tmp;
		sc = tmp - itmp;
		if(itmp<0 || n1==1) {
			val = grid[0];
		} else if(itmp>=n1-1) {
			val = grid[n1-1];	
		} else {
			val = grid[itmp]*(1.-sc) + grid[itmp+1]*sc;
		}

		fbuf[gridpos] = val;
		fprintf(outfp,
			"                    %10.2f%10.2f%12.2f%12.2f%12.4f \n",
			fbuf[0],fbuf[1],fbuf[2],fbuf[3],fbuf[4]); 
		bzero(cbuf,nc);
	}while(fgets(cbuf,nc,lmkfp));

	free(grids);
	free(grid);
	free(cbuf);
	free(fbuf);
	
	exit(0);
}
