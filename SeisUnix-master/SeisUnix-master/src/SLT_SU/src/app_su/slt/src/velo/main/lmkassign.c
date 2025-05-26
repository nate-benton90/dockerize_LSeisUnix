/* LMKASSIGN program */
#include "velo.h"
#include "usgrid.h"
#include "par.h"

char *sdoc = 
"LMKASSIGN - Assign t/z values to Landmark horizon 			\n"
"\n"
"lmkassign <landmark.in.file seed= >landmark.out.file	\n" 
"\n"
"Required parameters:							\n"
"landmark.in.file=  name of Landmark input file 	\n"
"landmark.out.file= name of Landmark output file 	\n"
"seed=              name of the seed point file containing \n"
"                   (x,y,seed_value) in ascii format	\n"
"Optional parameters:							\n"
"xpos=3             column position of landmark x coordinates 	\n"
"ypos=4             column position of landmark y coordinates 		\n"
"tpos=5             column position of landmark time or depth 	\n"
"maxs=2500          maximum number of rows in the seed file	\n"
"NOTES:						 			\n"
"     1. The output value will be copied from the value of the nearest \n"
"        (x,y) seed point in the input seed file.	\n"
"\n"
"AUTHOR:		Zhiming Li,       ,	6/19/02   		\n"
;

main(int argc, char **argv)
{
	FILE *infp=stdin, *outfp=stdout, *seedfp;
	int xpos=3,ypos=4,tpos=5,maxs=2500;
	string seed;
	float *xs, *ys, *ts;
	float *fbuf;
	char *cbuf;
	int i;
	int nc = 200;
	int nf = 5;
	int is, ip;
	float x,y,t,tmp, tmp2;


   	/* initialization */
   	initargs(argc,argv);
   	askdoc(1);

	/* get input parameters */

	if( !getparint("xpos",&xpos) ) xpos=3; xpos -= 1;
	if( !getparint("ypos",&ypos) ) ypos=4; ypos -= 1;
	if( !getparint("tpos",&tpos) ) tpos=5; tpos -= 1;
	if( !getparint("maxs",&maxs) ) maxs=2500;
	if( !getparstring("seed",&seed) ) err(" seed missing \n");
	seedfp = efopen(seed,"r");

	/* memory allocations */
    	xs = (float*)emalloc(maxs*sizeof(float));
    	ys = (float*)emalloc(maxs*sizeof(float));
    	ts = (float*)emalloc(maxs*sizeof(float));
    	cbuf = (char *) emalloc(nc*sizeof(char));
    	fbuf = (float *) emalloc(nf*sizeof(float));

	is = 0;
    	fgets(cbuf,nc,seedfp);
    	do {
		sscanf(cbuf,"%f %f %f", &xs[is],&ys[is],&ts[is]);
		is += 1;
		if(is>maxs) err(" number of rows of seed exceeds %d \n", maxs);
	} while(fgets(cbuf,nc,seedfp));

	fprintf(stderr," %d seed points read \n",is);
	for(i=0;i<is;i++) {
		fprintf(stderr,"x=%f y=%f seed=%f \n",xs[i],ys[i],ts[i]);
	}
	ip = 0;
    	fgets(cbuf,nc,infp);
    	do {
		sscanf(cbuf,"%f %f %f %f %f",
			&fbuf[0],&fbuf[1],&fbuf[2],&fbuf[3],&fbuf[4]);
		x = fbuf[xpos];
		y = fbuf[ypos];
	
		t = ts[0];
		tmp2 = (xs[0]-x)*(xs[0]-x)+(ys[0]-y)*(ys[0]-y);

		for(i=1;i<is;i++) {
			tmp = (xs[i]-x)*(xs[i]-x)+(ys[i]-y)*(ys[i]-y);
			if(tmp<tmp2) {
				tmp2 = tmp;
				t = ts[i];
			}
		}
		fbuf[4] = t;
		fprintf(outfp, 
		"                    %10.2f%10.2f%12.2f%12.2f%12.4f   \n",
                        fbuf[0],fbuf[1],fbuf[2],fbuf[3],fbuf[4]);
                bzero(cbuf,nc);
		ip = ip + 1;
	} while(fgets(cbuf,nc,infp));
		
	free(cbuf);
	free(fbuf);
	free(ts);
	free(ys);
	free(xs);

	exit(0);
}
