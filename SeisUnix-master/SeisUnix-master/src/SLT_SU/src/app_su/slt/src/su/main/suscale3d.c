#include "usgrid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUSCLE3D - 3D Amplitude Scaling 			\n"
"\n"
"suscale3d <stdin >stdout [optional parameters]\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of input cdp gathers 			\n"
"stdout                     Name of output cdp gathers 			\n"
"scalegrid=                 Name of 3D scale grid to be applied		\n"
"                           (stored as (t,x,y) order with grid header)	\n" 
"tracekey=                  segy trace header key word for trace number \n"                  
"linekey=                   segy trace header key word for line number \n"                  
"Optional Parameters:\n"
"ocdp2=from-scalegrid       trace number at first trace of ampgrid	\n"
"dcdp2=from-scalegrid       trace number increment along the inline \n"
"                           in ampgrid \n"
"oline3=from-scalegrid      line number at first trace of ampgrid \n"
"dline3=from-scalegrid      line number increment of ampgrid \n"
"\n"
"Author:	Zhiming Li		      		2-23-2001	\n"
"Notes:									\n"
"   1. linear interpolation of scale grids before applying to the data \n" 
"   2. o1 and d1 of input scalgrid must be in ms or meter or ft \n"
"   3. input trace must have valid tracekey, linekey, delrt, dt  \n"
"\n";
/**************** end self doc *******************************************/


segytrace tr;


main(int argc, char **argv)
{

	char *agfile;
	FILE *infp=stdin,*outfp=stdout,*agfp;
	usghed usgh;

	float *scale, *at, *aread, *tin, *tout;
	int *indx;
	float o1, d1, o2, d2, o3, d3, ft, dt;
	int n1, n2, n3, nt;
	float x, y, xpre, ypre;
	int i1, ix, iy, it, change;

	String tracekey="fldr", linekey="ep", trktype, lnktype;
	Value trkval, lnkval;
	int indxtrk, indxlnk;


	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);


	/* get required parameters */
	if (!getparstring("scalegrid",&agfile)) 
		err(" scalegrid must be specified ");

	agfp = efopen(agfile,"r");
	file2g(agfp);
	/* obtain grid header info */
	if(fgetusghdr(agfp,&usgh)!=0) err(" scalegrid header error ");

	if (!getparfloat("ocdp2",&o2)) o2 = usgh.ocdp2;  
	if (!getparfloat("dcdp2",&d2)) d2 = usgh.dcdp2;  
	if (!getparfloat("oline3",&o3)) o3 = usgh.oline3;  
	if (!getparfloat("dline3",&d3)) d3 = usgh.dline3;  
	if(d2==0.) err(" dcdp2 must not be zero \n");
	if(d3==0.) err(" dline3 must not be zero \n");
	n2 = usgh.n2;
	n3 = usgh.n3;
	o1 = usgh.o1;
	d1 = usgh.d1;
	n1 = usgh.n1;

	getparstring("tracekey",&tracekey);
	getparstring("linekey",&linekey);
	trktype = hdtype(tracekey);
	lnktype = hdtype(linekey);
	indxtrk = getindex(tracekey);
	indxlnk = getindex(linekey);

	file2g(infp);
	file2g(outfp);

	/* get information from the first header */
	if (!fgettr(infp,&tr)) err("can't get first trace");
	nt = tr.ns;
	dt = (float)tr.dt/1000.0;
	ft = tr.delrt;

	fprintf(stderr," suscale3D parameters \n");
	fprintf(stderr," ================== \n");
	fprintf(stderr,"starting time or depth of ampgrid: o1=%g\n",o1);
	fprintf(stderr,"starting trace of ampgrid: ocdp2=%g\n",o2);
	fprintf(stderr,"starting line of ampgrid: oline3=%g\n",o3);
	fprintf(stderr,"time/depth increment of ampgrid: d1=%g\n",d1);
	fprintf(stderr,"trace increment of ampgrid: dcdp2=%g\n",d2);
	fprintf(stderr,"line increment of ampgrid: dline3=%g\n",d3);
	fprintf(stderr,"number of time/depth of ampgrid: n1=%d\n",n1);
	fprintf(stderr,"number of traces per line of ampgrid: n2=%d\n",n2);
	fprintf(stderr,"number of lines of ampgrid: n3=%d\n",n3);
	fprintf(stderr," tracekey=%s linekey=%s \n",tracekey,linekey);
	fprintf(stderr,"\n");
		
	/* allocate workspace */
	at = (float*) emalloc(nt*sizeof(float));
	scale = (float*) emalloc(n1*n2*n3*sizeof(float));
	tin  = (float*) emalloc(n1*sizeof(float));
	tout = (float*) emalloc(nt*sizeof(float));
	aread = (float*) emalloc(n1*sizeof(float));
	indx = (int*) emalloc(nt*sizeof(int));

	bzero(scale,n1*n2*n3*sizeof(float));


	fseek2g(agfp,0,0);
	efread(scale,sizeof(float),n1*n2*n3,agfp);

	for(i1=0;i1<n1;i1++) tin[i1] = o1 + i1*d1;
	for(i1=0;i1<nt;i1++) tout[i1] = ft + i1 * dt;

	gethval(&tr,indxtrk,&trkval);
	ix = vtoi(trktype,trkval);
	gethval(&tr,indxlnk,&lnkval);
	iy = vtoi(lnktype,lnkval);
	xpre = ix - 100;
	ypre = iy - 100;
	change = 0;

	do {

		gethval(&tr,indxtrk,&trkval);
		ix = vtoi(trktype,trkval);
		gethval(&tr,indxlnk,&lnkval);
		iy = vtoi(lnktype,lnkval);

                x = ix;
		y = iy;
		if(x!=xpre || y!=ypre) {
			change = 1;
			xpre = x;
			ypre = y;
		} else {
			change = 0;
		}

		if(change==1) {
			bilint_(&n1,&n2,&n3,&o2,&o3,&d2,&d3,&x,&y,scale,aread);
			lin1d_(tin,aread,&n1,tout,at,&nt,indx);
		}

		for(it=0;it<nt;it++) tr.data[it] *= at[it];
		fputtr(outfp,&tr);

	} while (fgettr(infp,&tr));

	free(scale);
	free(at);
	free(aread);
	free(tin);
	free(tout);
	free(indx);
	

	return EXIT_SUCCESS;
}

