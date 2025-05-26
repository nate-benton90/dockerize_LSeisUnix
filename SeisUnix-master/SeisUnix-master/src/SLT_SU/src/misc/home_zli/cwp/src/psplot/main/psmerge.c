/* Copyright (c) Colorado School of Mines, 1990.
/* All rights reserved.                       */

char *sdoc =
"PSMERGE - MERGE PostScript files\n"
"\n"
"psmerge in= [optional parameters] >postscriptfile\n"
"\n"
"Required Parameters:\n"
"in=                    postscript file to merge\n"
"\n"
"Optional Parameters:\n"
"origin=0.0,0.0         x,y origin in inches\n"
"scale=1.0,1.0          x,y scale factors\n"
"rotate=0.0             rotation angle in degrees\n"
"translate=0.0,0.0      x,y translation in inches\n"
"\n"
"Notes:\n"
"More than one set of in, origin, scale, rotate, and translate\n"
"parameters may be specified.  Output x and y coordinates are\n"
"determined by:\n"
"         x = tx + (x-ox)*sx*cos(d) - (y-oy)*sy*sin(d)\n"
"         y = ty + (x-ox)*sx*sin(d) + (y-oy)*sy*cos(d)\n"
"where tx,ty are translate coordinates, ox,oy are origin coordinates,\n"
"sx,sy are scale factors, and d is the rotation angle.  Note that the\n"
"order of operations is shift (origin), scale, rotate, and translate.\n"
"\n"
"AUTHOR:  Dave Hale, Colorado School of Mines, 11/10/90\n"
"MODIFIED:  Dave Hale, Colorado School of Mines, 04/16/91\n"
"           added rotate parameter\n"
"\n";


#include "par.h"
#include "psplot.h"

#define MAXLINE 2048

main (argc,argv)
int argc; char **argv;
{
	int nin,iin,llx,lly,urx,ury,llxm,llym,urxm,urym;
	float sx,sy,tx,ty,ox,oy,d,fval[100];
	char *in,line[MAXLINE];
	FILE *infp,*outfp=stdout;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* determine number of input files */
	nin = countparname("in");
	if (nin<=0) err("must specify at least one input file");
	
	/* initialize bounding box limits */
	llxm = llym = INT_MAX;
	urxm = urym = INT_MIN;
	
	/* begin PostScript */
	beginps();
	newpage("1",1);
	
	/* loop over input files */
	for (iin=0; iin<nin; ++iin) {
	
		/* open file */
		getnparstring(iin+1,"in",&in);
		infp = efopen(in,"r");
		
		/* save graphics state */
		gsave();
		
		/* transform coordinates */
		if (getnparfloat(iin+1,"rotate",fval)==1) {
			d = fval[0];
		} else {
			d = 0.0;
		}
		if (getnparfloat(iin+1,"scale",fval)==2) {
			sx = fval[0];
			sy = fval[1];
		} else {
			sx = 1.0;
			sy = 1.0;
		}
		if (getnparfloat(iin+1,"translate",fval)==2) {
			tx = fval[0]*72.0;
			ty = fval[1]*72.0;
		} else {
			tx = 0.0;
			ty = 0.0;
		}
		if (getnparfloat(iin+1,"origin",fval)==2) {
			ox = fval[0]*72.0;
			oy = fval[1]*72.0;
		} else {
			ox = 0.0;
			oy = 0.0;
		}
		translate(tx,ty);
		rotate(d);
		scale(sx,sy);
		translate(-ox,-oy);
		
		/* loop over lines in file */
		while (fgets(line,MAXLINE,infp)!=NULL) {
		
			/* if comment */
			if (line[0]=='%') {
				
				/* if bounding box specification */
				if (strstr(line,"%%BoundingBox:")!=NULL) {
					
					/* if no bbox values, skip it */
					if (strstr(line,"atend")!=NULL)
						continue;
					
					/* update bounding box */
					sscanf(line,"%*s %d %d %d %d",
						&llx,&lly,&urx,&ury);
					llx = tx+(llx-ox)*sx;
					lly = ty+(lly-oy)*sy;
					urx = tx+(urx-ox)*sx;
					ury = ty+(ury-oy)*sy;
					llxm = MIN(llxm,llx);
					llym = MIN(llym,lly);
					urxm = MAX(urxm,urx);
					urym = MAX(urym,ury);
				}
				
				/* discard comments */
				continue;
			}
			
			/* discard showpage if at beginning of line */
			if (strstr(line,"showpage")==&line[0]) continue;
			
			/* write output line */
			fputs(line,outfp);
		}
		
		/* restore graphics state */
		grestore();
		
		/* close file */
		fclose(infp);
	}
	
	/* end PostScript */
	boundingbox(llxm,llym,urxm,urym);
	showpage();
	endps();
}
