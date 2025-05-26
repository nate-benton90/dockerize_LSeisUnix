#include "usgrid.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation **********************/
string sdoc =
" 								\n"
" SURATIO - compute ratio between two input data sets 		\n"
" 								\n"
" suratio data1 data2 [optional parameters] >output 			\n"
" 								\n"
" Required parameters:						\n"
" 	data1     input data set 1							\n"
" 	data2     input data set 2							\n"
" 	output    output data of ratio (data1/data2)		    \n"
" 								\n"
" Optional parameter:						\n"
"   rms=1     =1 ratio defined as rms amplitude ratio 	\n"
"             =0 ratio defined as absolute value ratio  \n"
"   lwin=40   window length (in ms) to compute rms or averge avsolute value \n"
"   t0=       starting time (in ms) to output the ratios \n"
"   dt=       time increment (in ms) to output the ratios \n" 
"   nt=       number of time samples to output the ratios \n"
"             t0 will default to delrt in the input trace header 	\n"
"             dt will default to dt in the input trace header 	\n"
"             nt will default to ns in the input trace header 	\n"
"   bv=       													\n"
"             value of ratio to be used in the output when 		\n"
"             window is outside the live smaple range of intput \n"
"             traces 	\n"
"             default to same as the first or last live sample ratio \n"
"             >=0 used in the no-data zone		\n"
"             <0 will use the average value of the ratios at this trace \n"
"   tmin=     minimum time to compute the ratio (in ms)		\n"
"             default to t0 			\n"
"   tmax=     maximum time to compute the ratio (in ms)		\n"
"             default to t0+(nt-1)*dt				\n"
" 								\n"
" Author:     Zhiming Li     	12/13/96		\n";
/**************** end self doc ***********************************/

segy intrace1, intrace2;
/* segy outtrace; */
segybhdr bh;
segychdr ch;


main(int argc, char **argv)
{
	FILE *fp1, *fp2, *outfp=stdout;
	int nt,lwin,rms;
	float dt, t0;
	float delt, ft;
	int ns;
	int itr;
	int mute;
	int it, itt, lh, ierr;
	double tmp, tmp1, tmp2;
	float t, tt, gmin, gmax;
	float bv;
	int ib;
	int i1,i2, j1, j2, ii, jj;
	int nd;

	float tmin, tmax;

	usghed usgh;
	float *grid;


	/* Initialize */
	initargs(argc, argv);
	askdoc(2); /* two file args required */

	/* Open two files given as arguments for reading */
	fp1 = efopen(argv[1], "r");
	fp2 = efopen(argv[2], "r");

	fgethdr(fp1,&ch,&bh);

	if(!fgettr(fp1, &intrace1)) err(" can't get first trace");

	/* Get parameters */
	if (!getparint("rms", &rms))	rms = 1;
	if (!getparint("lwin", &lwin))	lwin = 40;
	if (!getparint("nt", &nt))	nt = intrace1.ns;
	if (!getparfloat("t0", &t0))	t0 = intrace1.delrt;
	if (!getparfloat("dt", &dt))	dt = (float)intrace1.dt*0.001;
	ib = 1;
	if (!getparfloat("bv", &bv))	ib = 0;
	if (!getparfloat("tmin", &tmin))	tmin = t0;
	if (!getparfloat("tmax", &tmax))	tmax = t0 + (nt-1)*dt;

	ft = intrace1.delrt;
	ns = intrace1.ns;
	delt = (float)intrace1.dt*0.001;

	fseek(fp1,3600,0);
	fseek(fp2,3600,0);

	t = lwin/delt/2. + 0.5;
	lh = t;
	itr = 0;

/*
	bh.hdt = dt*1000;
	bh.hns = nt;
	fputhdr(outfp,&ch,&bh);
*/

	grid = (float*) emalloc(nt*sizeof(float));

	/* Loop over the traces */
	while (fgettr(fp1, &intrace1) && fgettr(fp2, &intrace2)) {

		if (intrace1.ns != intrace2.ns ) {
			warn("trace %d:", itr+1);
			err("%s and %s have different ns (%d vs %d)",
				argv[1], argv[2], intrace1.ns, intrace2.ns);
		}
		if (intrace1.delrt !=intrace2.delrt) {
			warn("trace %d:", itr+1);
			warn("%s and %s have different delrt (%d vs %d)",
				argv[1], argv[2], intrace1.delrt, intrace2.delrt);
		}
		if (intrace1.dt != intrace2.dt) {
			warn("trace %d:", itr+1);
			warn("%s and %s have different dt (%d vs %d)",
				argv[1], argv[2], intrace1.dt, intrace2.dt);
		}

/*
		bcopy((char*)&intrace2,(char*)&outtrace,240);
		outtrace.delrt = t0;
		outtrace.ns = nt;
		outtrace.d1 = dt;
*/

		i1 = ns-1;
		i2 = ns-1;
		j1 = 0;
		j2 = 0;
		/* detect live sample ranges */
	 	for(it=0;it<ns;it++) {
			if(intrace1.data[it]!=0. && i1==ns-1) i1=it;
			if(intrace2.data[it]!=0. && i2==ns-1) i2=it;
		}
	 	for(it=ns-1;it>=0;it--) {
			if(intrace1.data[it]!=0. && j1==0) j1=it;
			if(intrace2.data[it]!=0. && j2==0) j2=it;
		}

		ii = i1;
		if(ii<i2) ii = i2;
		jj = j1;
		if(jj>j2) jj = j2;
		mute = ft + ii * delt;
		nd = ft + jj * delt;


		if(rms==1) {
		 	for(it=ii;it<=jj;it++) {
				intrace1.data[it] = intrace1.data[it]*intrace1.data[it];
				intrace2.data[it] = intrace2.data[it]*intrace2.data[it];
			}
		} else {
		 	for(it=ii;it<=jj;it++) {
				if(intrace1.data[it]<0) intrace1.data[it] = -intrace1.data[it];
				if(intrace2.data[it]<0) intrace2.data[it] = -intrace2.data[it];
			}
		}

/* outtrace.mute = mute; */

		if(mute<tmin) mute = tmin;
		t = (mute+lh*delt-t0)/dt+0.5;
		ii = t;
		if(nd>tmax) nd = tmax;
		t = (nd-lh*delt-t0)/dt;
		jj = t;
		if(ii<0) ii = 0;
		if(jj>nt-1) jj = nt-1;

		for(it=ii;it<=jj;it++) {
			t = t0 + it * dt;
			itt = (t - ft)/delt;
			tmp1 = 0.;
			tmp2 = 0.;
			for(i1=itt-lh;i1<=itt+lh;i1++) {
				if(i1>=0 && i1<ns) {
					tmp1 = tmp1 + intrace1.data[i1]; 
					tmp2 = tmp2 + intrace2.data[i1]; 
				}
			}
			if(tmp2>0) {
				tmp = tmp1/tmp2;
				if(rms==1 && tmp>0.) tmp = sqrt(tmp);
			} else {
			/* tmp = outtrace.data[it-1]; */
				tmp = grid[it-1];
			}
			/* outtrace.data[it] = tmp; */
			grid[it] = tmp;
		}
		if(ib==0) {
			for(it=0;it<ii;it++) grid[it] = grid[ii];
			for(it=jj+1;it<nt;it++) grid[it] = grid[jj];
		} else if(bv>=0.) {
			for(it=0;it<ii;it++) grid[it] = bv;
			for(it=jj+1;it<nt;it++) grid[it] = bv;
		} else if(bv<0.) {
			tmp = 0.;
			for(it=ii;it<=jj;it++) tmp += grid[it];
			tmp = tmp/(jj-ii+1.);
			for(it=0;it<ii;it++) grid[it] = tmp;
			for(it=jj+1;it<nt;it++) grid[it] = tmp;
		}
		/*
		for(it=0;it<ii;it++) outtrace.data[it] = outtrace.data[ii];
		for(it=jj+1;it<nt;it++) outtrace.data[it] = outtrace.data[jj];
		*/

		if(itr==0) {gmin = grid[0]; gmax=grid[0];}
		for(it=0;it<nt;it++) {
			if(gmin>grid[it]) gmin = grid[it];
			if(gmax<grid[it]) gmax = grid[it];
		}

		/* fputtr(outfp, &outtrace); */
		efwrite(grid,sizeof(float),nt,outfp);

		itr++;
	}

/* update grid header */
	bzero(&usgh,100);
	usgh.n1 = nt; usgh.o1 = t0; usgh.d1 = dt;
	usgh.n2 = itr; usgh.o2 = 1; usgh.d2 = 1;
	usgh.n3 = 1; usgh.o3 = 0; usgh.d3 = 1;
	usgh.n4 = 1; usgh.n5 = 1; usgh.scale = 1.e-6;
	usgh.dtype = 4; usgh.gmin = gmin; usgh.gmax = gmax;
   	usgh.d4 = usgh.d5 = usgh.o4 = usgh.o5 = 0.;
   	usgh.orient = usgh.gtype = 0;

	ierr = fputusghdr(outfp, &usgh);
	if(ierr!=0) err("error output grid header ");



	free(grid);

	return EXIT_SUCCESS;
}
