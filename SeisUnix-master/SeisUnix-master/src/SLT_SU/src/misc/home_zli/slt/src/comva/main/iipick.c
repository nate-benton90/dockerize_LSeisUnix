#include "comva.h"
#include "par.h"
#include "xplot.h"
#include <X11/Xatom.h>
#include <X11/keysym.h>

/* self-documentation */
char *sdoc = 
"IIPICK - X IMAGE plot and interface pick \n"
"\n"
"iipick n1= [optional parameters] <binaryfile\n"
"\n"
"X Functionality:\n"
"Button 1	Zoom with rubberband box\n"
"Button 2	Show mouse (x1,x2) coordinates while pressed\n"
"Button 3	Same as key 'a' 				\n"
"Q key		Quit (can also use Motif Action button)\n"
"a key		Append current mouse (x1,x2) location to end of picks \n"
"i key		Insert current mouse (x1,x2) location into middle of picks \n"
"d key		Delete current mouse (x1,x2) location from picks \n"
"p key		Display picks \n"
"n key		Do not display picks (seismic section display only) \n"
"s key		Append saved mouse (x1,x2) picks to pick file \n"
"c key		Clear all saved mouse (x1,x2) picks of the current horizon\n"
"f key          go to next horizon	\n" 
"b key          go to previous horizon	\n" 
"1-9 key        go to horizon 1-9	\n"
"0 key          go to horizon 10	\n"
"F1 key         go to horizon 11	\n"
"F2 key         go to horizon 12	\n"
"Fi key         go to horizon i+10	(i=1,12) \n"
"F12 key        go to horizon 22        \n"
"T key          edit top velocity of current horizon \n"
"               (Dashed line --- previous; Solid line --- edited.	\n"
"                to save edited velocity, use S key; to exit, use Q key; \n"
"		 to edit, use right button of mouse;			\n" 
"                to show mouse location, use button 2 of mouse) 	\n" 
"B key          edit bottom velocity of current horizon \n"
"               (Dashed line --- previous; Solid line --- edited.	\n"
"                to save edited velocity, use S key; to exit, use Q key; \n"
"		 to edit, use right button of mouse; 			\n" 
"                to show mouse location, use button 2 of mouse) 	\n" 
"\n"
"Required Parameters:\n"
"n1                     number of samples in 1st (fast) dimension\n"
"\n"
"Optional Parameters:\n"
"d1=1.0                 sampling interval in 1st dimension\n"
"f1=d1                  first sample in 1st dimension\n"
"n2=all                 number of samples in 2nd (slow) dimension\n"
"d2=1.0                 sampling interval in 2nd dimension\n"
"f2=d2                  first sample in 2nd dimension\n"
"perc=100.0             percentile used to determine clip\n"
"clip=(perc percentile) clip used to determine bclip and wclip\n"
"bperc=perc             percentile for determining black clip value\n"
"wperc=100.0-perc       percentile for determining white clip value\n"
"bclip=clip             data values outside of [bclip,wclip] are clipped\n"
"wclip=-clip            data values outside of [bclip,wclip] are clipped\n"
"cmap=gray              gray, hue, or rgb colormaps may be specified\n"
"verbose=1              =1 for info printed on stderr (0 for no info)\n"
"xbox=0                 x in pixels of upper left corner of window\n"
"ybox=50                y in pixels of upper left corner of window\n"
"wbox=550               width in pixels of window\n"
"hbox=700               height in pixels of window\n"
"x1beg=x1min            value at which axis 1 begins\n"
"x1end=x1max            value at which axis 1 ends\n"
"d1num=0.0              numbered tic interval on axis 1 (0.0 for automatic)\n"
"f1num=x1min            first numbered tic on axis 1 (used if d1num not 0.0)\n"
"n1tic=1                number of tics per numbered tic on axis 1\n"
"grid1=none             grid lines on axis 1 - none, dot, dash, or solid\n"
"label1=                label on axis 1\n"
"x2beg=x2min            value at which axis 2 begins\n"
"x2end=x2max            value at which axis 2 ends\n"
"d2num=0.0              numbered tic interval on axis 2 (0.0 for automatic)\n"
"f2num=x2min            first numbered tic on axis 2 (used if d2num not 0.0)\n"
"n2tic=1                number of tics per numbered tic on axis 2\n"
"grid2=none             grid lines on axis 2 - none, dot, dash, or solid\n"
"label2=                label on axis 2\n"
"labelfont=Erg14        font name for axes labels\n"
"title=                 title of plot\n"
"titlefont=Rom22        font name for title\n"
"labelcolor=blue        color for axes labels\n"
"titlecolor=red         color for title\n"
"gridcolor=SteelBlue    color for grid lines\n"
"style=seismic          normal (axis 1 horizontal, axis 2 vertical) or\n"
"labelfont=Erg14        font name for axes labels\n"
"                       seismic (axis 1 vertical, axis 2 horizontal)\n"
"mpicks=stderr          file name to save mouse picks (ifile or hfile)\n"
"                       (where stderr is standard error output --- screen)\n"
"ifile=mpicks           old ifile or hfile name 			\n"
"xtol=d2                the horizons will extend, at both ends, a distance \n" 
"                         of xtol to avoid discontinuity		\n" 
"ism=1                  smoothing option; to laterally smooth interval  \n"
"                         velocities at top and bottom, and the interval \n"
"                         velocity gradient, along interface, before output\n" 
"                         0=average all values at picks along interface and \n"
"                           this single value is assigned to all output \n"
"                           points along the interface \n"
"                         1=no smoothing \n"
"                         n=smooting values at picks along interface using \n"
"                           n data points \n"
"VELO=NULL              average velocity VELO card and interval velocity \n"
"                         gradient DVDZ card file name              \n"
"                       (if not given, i.e., NULL, the picking will output \n"
"                        the positions of the horizons with velocity \n"
"                        fields being interpolated from those 		\n"
"                        in input if present) 				\n"
"lhnvelo=0              last interface/horizon number to use VELO for 	\n"
"                       velocity updating				\n" 
"                       (for i-th horizons,				\n" 
"                              i < lhnvelo, velocity will not be updated \n"   
"                              i >= lhnvelo, velocity updated if needed) \n"   
"dif=1                  DIF values between two ends of an interface in	\n" 
"                       the output ifile (either 1 or 0)		\n"
"itopbot=0              mode of editing top and bottom velocity of same layer\n"
"                       0=top and bottom change independently 		\n"
"                       1=top and bottom change togather 		\n"
"                         (when editing top velocity of one layer, 	\n" 
"                          bottom velocity of the same layer gets 	\n"
"			   updated automatically)			\n"
"\n"
"NOTE: \n"
" 1. picks output format will follow the hfile format with velocity at top  \n"
"    and at bottom redefined (IFILE format). Average velocity and velocity \n"
"    vertical gradient are also added in IFILE format.			\n"
" 2. if itopbot=1, depths of interfaces (horizons) in input ifile must 	\n"
"    incease in order, i.e., shallow interface before deep interfaces.	\n"
"\n"
"	AUTHOR:	Zhiming Li	      	9/91	\n"
"\n";


/* define maximum number of horizons and maximum number of picks per horizon */
#define maxhs 128
#define maxpks 256

/* functions defined and used internally */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iby, float *y1b, float *y2b);
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out);
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb);
void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *x1picks, float *x2picks, float *veltop, float *velbot, int *dif,
	float *velavg, float *dvdz,
	int *npicks, int nhs, int ism, 
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib, int idif, int ihfile);
void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char hcolors[maxhs][10],
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *x1picks, float *x2picks, float *veltop, float *velbot, int *dif,
	int *npicks, int nhs, int ih, GC gc, int pkey, int *savebg);
void vtopbot(int maxp,int maxcdp, 
     	int ncdp_velo,float *x_velo,float *z_velo,float *v_velo,int *nps_velo,
	int ncdp_dvdz,float *x_dvdz,float *zt_dvdz,float *zb_dvdz, 
	float *dvdz_dvdz,int *nps_dvdz,
	int nhs, float *xpicks, float *zpicks, int *npicks, float xtol, 
	float *veltops, float *velbots, float *velavgs, float *dvdzs,
	int lhnvelo);

void ivedit(float *xpick, float *vpick, char *hcolor, int nvi, char *title,
		float xmin, float dcdp, int cdpxmin, int *isave);

void interp(float *xold, float *xnew, int nold, int nnew, float *vtop, 
	float *vbot, float *vavg, float *dvdz);

static Display *dpy;
static Window win;

main (argc,argv)
int argc; char **argv;
{
	int n1,n2,n1tic,n2tic,nfloats,
		i1,i2,grid1,grid2,style,
		n1c,n2c,i1beg,i1end,i2beg,i2end,i1c,i2c,
		nz,iz,i1step,i2step,verbose,
		xbox,ybox,wbox,hbox,
		xb,yb,wb,hb,
		x,y,width,height,
		i,j,nx,ny,nxb,nyb,ixb,iyb,
		imageOutOfDate,winwidth=-1,winheight=-1,
		showloc=0,*npicks,savebg,pkey,fex,
		cdpxmin,cdpxmax,*hnums,ih,nhs,*difs,*indx,
		cdpxmini,cdpxmaxi,ihsave,itopbot,
		ism,ncdps_velo,ncdps_dvdz,maxp,maxcdp,ihfile=0,
		*nps_velo,*nps_dvdz,*sortindex,*isort,isave,
		*cdps_velo,*cdps_dvdz,cdp, ivtopbot=0,nvi,jh,nold,idif,lhnvelo;
	float labelsize,titlesize,perc,clip,bperc,wperc,bclip,wclip,
		d1,f1,d2,f2,*z,*temp,zscale,zoffset,zi,
		x1beg,x1end,x2beg,x2end,
		x1min,x1max,x2min,x2max,
		d1num,f1num,d2num,f2num,
		x1begb,x1endb,x2begb,x2endb,
		*x1picks,*x2picks,*veltops,*velbots,
		*dens,*qval,*pois, 
		*velavgs,*dvdzs, 
		xmin,xmax,zmin,zmax,dcdp,
		xmini,xmaxi,zmini,zmaxi,dcdpi,vmini,vmaxi, xtol,
		*z_velo,*x_velo,*v_velo,*sorts,*cdpsort,
		*zt_dvdz,*zb_dvdz,*dvdz_dvdz,*x_dvdz,xcdp,*xold,
		*vtopold, *vbotold, *vavgold, *dvdzold;
	unsigned char *cz,*czp,*czb,*czbp,*czbi=NULL;
	char *label1="depth",*label2="cdp",
		*title="Seismic Interface Picking",
		*labelfont="Erg14",*titlefont="Rom22",
		*styles="seismic",*grid1s="none",*grid2s="none",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="SteelBlue",*cmap="gray",keybuf[256],*mpicks,
		hnames[maxhs][80],strhnum[3],hcolors[maxhs][10],
		*hname="NAME",*VELO="NULL",
		*dunits="METER",*zattrib="DEPTH SECTION",
		dunitsi[80], zattribi[80], *vtype, vtypei[80],
		ivtitle[80],veloyes,*ifile;
	FILE *infp=stdin, *mpicksfp, *velofp, *tty, *ifilefp;
/*
	Display *dpy;
	Window win;
*/
	XEvent event;
	KeySym keysym;
	XComposeStatus keystat;
	XImage *image=NULL;
	GC gci;
	int scr;
	unsigned long black,white,pmin,pmax;


	float cdpbegb, cdpendb;

	/* initialize getpar */
	initargs(argc,argv);
	askdoc(1);

	/* get parameters describing 1st dimension sampling */
	if (!getparint("n1",&n1))
		err("Must specify number of samples in 1st dimension!\n");
	d1 = 1.0;  getparfloat("d1",&d1);
	f1 = d1;  getparfloat("f1",&f1);
	x1min = (d1>0.0)?f1:f1+(n1-1)*d1;
	x1max = (d1<0.0)?f1:f1+(n1-1)*d1;

	/* get parameters describing 2nd dimension sampling */
	if (!getparint("n2",&n2)) {
		if (fseek(infp,0L,2)!=0)
			err("must specify n2 if in a pipe!\n");
		nfloats = eftell(infp)/sizeof(float);
		efseek(infp,0L,0);
		n2 = nfloats/n1;
	}
	d2 = 1.0;  getparfloat("d2",&d2);
	f2 = d2;  getparfloat("f2",&f2);
	x2min = (d2>0.0)?f2:f2+(n2-1)*d2;
	x2max = (d2<0.0)?f2:f2+(n2-1)*d2;


	/* set up file to save mouse picks */
	if (!getparstring("mpicks", &mpicks)) {
		mpicksfp = stderr;
	} else {
		/* if file exist */
                if((mpicksfp = fopen(mpicks,"r"))!=NULL) {
			fex = 1;
                        fclose(mpicksfp);
                        mpicksfp = efopen(mpicks,"r");
                } else {
                        mpicksfp = efopen(mpicks,"w");
                        fex = 0;
                }
	}

	if (!getparstring("ifile", &ifile)) {
		ifilefp = mpicksfp;
	} else {
                ifilefp = efopen(ifile,"r");
		fex = 1;
	}

	/* memory allocations */
	x1picks = (float *) malloc(maxhs*maxpks*sizeof(float));
	x2picks = (float *) malloc(maxhs*maxpks*sizeof(float));
	veltops = (float *) malloc(maxhs*maxpks*sizeof(float));
	velbots = (float *) malloc(maxhs*maxpks*sizeof(float));
	npicks = (int *) malloc(maxhs*sizeof(int));
	hnums = (int *) malloc(maxhs*sizeof(int));
	difs = (int *) malloc(maxhs*maxpks*sizeof(int));
	dens = (float *) malloc(maxhs*maxpks*sizeof(float));
	qval = (float *) malloc(maxhs*maxpks*sizeof(float));
	pois = (float *) malloc(maxhs*maxpks*sizeof(float));
	velavgs = (float *) malloc(maxhs*maxpks*sizeof(float));
	dvdzs = (float *) malloc(maxhs*maxpks*sizeof(float));
	xold = (float *) malloc(maxpks*sizeof(float));
	indx = (int *) malloc(maxpks*sizeof(int));
	vtopold = (float *) malloc(maxpks*sizeof(float));
	vbotold = (float *) malloc(maxpks*sizeof(float));
	vavgold = (float *) malloc(maxpks*sizeof(float));
	dvdzold = (float *) malloc(maxpks*sizeof(float));

	for(ih=0;ih<maxhs;ih++) npicks[ih]=0;
	for(ih=0;ih<maxhs*maxpks;ih++) {
		veltops[ih]=0.;
		velbots[ih]=0.;
		difs[ih] = 0;
		velavgs[ih] = 0.;
		dvdzs[ih] = 0.;
	}

	/* horizon color assignment */
	strncpy(hcolors[0],"blue",4);
	strncpy(hcolors[1],"cyan",4);
	strncpy(hcolors[2],"green",5);
	strncpy(hcolors[3],"brown",5);
	strncpy(hcolors[4],"orange",6);
	strncpy(hcolors[5],"red",3);
	strncpy(hcolors[6],"violet",6);
	strncpy(hcolors[7],"purple",6);
	for(ih=8;ih<maxhs;ih++) strcpy(hcolors[ih],hcolors[ih-ih/8*8]);

	

	/* read binary data to be plotted */
	nz = n1*n2;
	z = ealloc1float(nz);
	if (fread(z,sizeof(float),nz,infp)!=nz)
		err("error reading input file");

	/* if necessary, determine clips from percentiles */
	if (getparfloat("clip",&clip)) {
		bclip = clip;
		wclip = -clip;
	}
	if ((!getparfloat("bclip",&bclip) || !getparfloat("wclip",&wclip)) &&
		!getparfloat("clip",&clip)) {
		perc = 100.0;  getparfloat("perc",&perc);
		temp = ealloc1float(nz);
		for (iz=0; iz<nz; iz++)
			temp[iz] = z[iz];
		if (!getparfloat("bclip",&bclip)) {
			bperc = perc;	getparfloat("bperc",&bperc);
			iz = (nz*bperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			bclip = temp[iz];
		}
		if (!getparfloat("wclip",&wclip)) {
			wperc = 100.0-perc;  getparfloat("wperc",&wperc);
			iz = (nz*wperc/100.0);
			if (iz<0) iz = 0;
			if (iz>nz-1) iz = nz-1;
			qkfind(iz,nz,temp);
			wclip = temp[iz];
		}
		free1float(temp);
	}
	verbose = 1;  getparint("verbose",&verbose);
	if (verbose) warn("bclip=%g wclip=%g",bclip,wclip);

	/* get colormap specification */
	getparstring("cmap",&cmap);

	/* get axes parameters */
	xbox = 0; getparint("xbox",&xbox);
	ybox = 50; getparint("ybox",&ybox);
	wbox = 550; getparint("wbox",&wbox);
	hbox = 700; getparint("hbox",&hbox);
	x1beg = x1min; getparfloat("x1beg",&x1beg);
	x1end = x1max; getparfloat("x1end",&x1end);
	d1num = 0.0; getparfloat("d1num",&d1num);
	f1num = x1min; getparfloat("f1num",&f1num);
	n1tic = 1; getparint("n1tic",&n1tic);
	getparstring("grid1",&grid1s);
	if (STREQ("dot",grid1s)) grid1 = DOT;
	else if (STREQ("dash",grid1s)) grid1 = DASH;
	else if (STREQ("solid",grid1s)) grid1 = SOLID;
	else grid1 = NONE;
	getparstring("label1",&label1);
	x2beg = x2min; getparfloat("x2beg",&x2beg);
	x2end = x2max; getparfloat("x2end",&x2end);
	d2num = 0.0; getparfloat("d2num",&d2num);
	f2num = 0.0; getparfloat("f2num",&f2num);
	n2tic = 1; getparint("n2tic",&n2tic);
	getparstring("grid2",&grid2s);
	if (STREQ("dot",grid2s)) grid2 = DOT;
	else if (STREQ("dash",grid2s)) grid2 = DASH;
	else if (STREQ("solid",grid2s)) grid2 = SOLID;
	else grid2 = NONE;
	getparstring("label2",&label2);
	getparstring("labelfont",&labelfont);
	labelsize = 18.0; getparfloat("labelsize",&labelsize);
	getparstring("title",&title);
	getparstring("titlefont",&titlefont);
	titlesize = 24.0; getparfloat("titlesize",&titlesize);
	getparstring("style",&styles);
	if (STREQ("normal",styles)) style = NORMAL;
	else style = SEISMIC;
	getparstring("titlecolor",&titlecolor);
	getparstring("labelcolor",&labelcolor);
	getparstring("gridcolor",&gridcolor);

	if(!getparint("cdpxmin",&cdpxmin)) cdpxmin=1;
	if(!getparint("cdpxmax",&cdpxmax)) cdpxmax=n2;

	if(!getparfloat("xmin",&xmin)) xmin=f2;
	if(!getparfloat("xmax",&xmax)) xmax=f2+(n2-1)*d2;
	if(!getparfloat("zmin",&zmin)) zmin=f1;
	if(!getparfloat("zmax",&zmax)) zmax=f1+(n1-1)*d1;

	dcdp = (xmax-xmin)/(cdpxmax-cdpxmin);

	getparstring("dunits",&dunits);
	getparstring("zattrib",&zattrib);

	if(!getparint("ism",&ism)) ism=1;
	if(!getparfloat("xtol",&xtol)) xtol=d2;
	if(!getparint("dif",&idif)) idif=1;
	if(!getparint("lhnvelo",&lhnvelo)) {
		lhnvelo=0;
	} else {
		lhnvelo=lhnvelo-1;
	}
	if(idif!=1) idif=0;
	if(!getparint("itopbot",&itopbot)) itopbot=0;

	/* read in velocity card dataset, if any */
	ncdps_velo = 0;
	ncdps_dvdz = 0;
	maxp = 256;
	maxcdp= 128;
	if(getparstring("VELO",&VELO)) {
		ihfile = 0;
		tty = efopen("/dev/tty","r");
		fprintf(stderr,
		"Replace velocities in IFILE with those in VELO? (y/n) ->"); 
		veloyes = getc(tty);
		efclose(tty);

                if( (velofp=fopen(VELO,"r"))!=NULL && veloyes=='y') {
			/* VELO cards */

			 fprintf(stderr,"VELO used to compute velocities \n");
			 cdps_velo = (int*) malloc(maxcdp*sizeof(int));
			 x_velo = (float*) malloc(maxcdp*sizeof(float));
			 z_velo = (float*) malloc(maxp*maxcdp*sizeof(float));
			 v_velo = (float*) malloc(maxp*maxcdp*sizeof(float));
			 nps_velo = (int*) malloc(maxcdp*sizeof(int));
			 sorts = (float*) malloc(maxp*maxcdp*sizeof(float));
			 sortindex = (int*) malloc(maxcdp*sizeof(int));
			 isort = (int*) malloc(maxcdp*sizeof(int));
			 cdpsort = (float*) malloc(maxcdp*sizeof(float));

			 veloread(velofp,cdps_velo,z_velo,v_velo,
				  &ncdps_velo,nps_velo, maxp, maxcdp);

			/*
			for(i=0;i<ncdps_velo;i++) {
				for(iz=0;iz<nps_velo[i];iz++) 
				fprintf(stderr,"%f %f %d %d \n",
				z_velo[i*maxp+iz],v_velo[i*maxp+iz],iz+1,i+1);
			}
			*/

			/* sort cdps of VELO cards into ascending order */
			for(i=0;i<ncdps_velo;i++) {
				sortindex[i] = i;
				cdpsort[i] = cdps_velo[i];
			}
                        if(ncdps_velo>0) qkisort(ncdps_velo,cdpsort,sortindex);
                        for(i=0;i<ncdps_velo;i++) {
                                cdps_velo[i] = cdpsort[sortindex[i]];
                                isort[i] = nps_velo[sortindex[i]];
                        }
                        for(i=0;i<ncdps_velo;i++) {
                                nps_velo[i] = isort[i];
                        }
                        for(i=0;i<ncdps_velo;i++) {
                                bcopy((char*)(v_velo+sortindex[i]*maxp),
                                        (char*)(sorts+i*maxp),maxp*4);
                        }
			for(i=0;i<ncdps_velo;i++) {
                                bcopy((char*)(sorts+i*maxp),
                                        (char*)(v_velo+i*maxp),maxp*4);
                        }
                        for(i=0;i<ncdps_velo;i++) {
                                bcopy((char*)(z_velo+sortindex[i]*maxp),
                                        (char*)(sorts+i*maxp),maxp*4);
                        }
			for(i=0;i<ncdps_velo;i++) {
                                bcopy((char*)(sorts+i*maxp),
                                        (char*)(z_velo+i*maxp),maxp*4);
                        }


			/* DVDZ cards */
			 cdps_dvdz = (int*) malloc(maxcdp*sizeof(int));
			 x_dvdz = (float*) malloc(maxcdp*sizeof(float));
			 zt_dvdz = (float*) malloc(maxp*maxcdp*sizeof(float));
			 zb_dvdz = (float*) malloc(maxp*maxcdp*sizeof(float));
			 dvdz_dvdz= (float*) malloc(maxp*maxcdp*sizeof(float));


			 nps_dvdz = (int*) malloc(maxcdp*sizeof(int));
			 dvdzread(velofp,cdps_dvdz,zt_dvdz,zb_dvdz,dvdz_dvdz,
				  &ncdps_dvdz,nps_dvdz,maxp,maxcdp);
			/* sort cdps of DVDZ cards into ascending order */
			for(i=0;i<ncdps_dvdz;i++) {
				sortindex[i] = i;
				cdpsort[i] = cdps_dvdz[i];
			}
    			if(ncdps_dvdz>0) qkisort(ncdps_dvdz,cdpsort,sortindex);
    			for(i=0;i<ncdps_dvdz;i++) {
        			cdps_dvdz[i] = cdpsort[sortindex[i]];
        			isort[i] = nps_dvdz[sortindex[i]];
    			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			nps_dvdz[i] = isort[i];
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(dvdz_dvdz+sortindex[i]*maxp),
					(char*)(sorts+i*maxp),maxp*4);
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(sorts+i*maxp),
					(char*)(dvdz_dvdz+i*maxp),maxp*4);
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(zt_dvdz+sortindex[i]*maxp),
					(char*)(sorts+i*maxp),maxp*4);
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(sorts+i*maxp),
					(char*)(zt_dvdz+i*maxp),maxp*4);
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(zb_dvdz+sortindex[i]*maxp),
					(char*)(sorts+i*maxp),maxp*4);
			}
    			for(i=0;i<ncdps_dvdz;i++) {
        			bcopy((char*)(sorts+i*maxp),
					(char*)(zb_dvdz+i*maxp),maxp*4);
			}
			free(isort);
			free(sorts);
			free(sortindex);
			free(cdpsort);
		}
		/* convert cdp locations of VELO into x locations */
		for(i=0;i<ncdps_velo;i++) { 
			x_velo[i] = xmin + (cdps_velo[i]-cdpxmin)*dcdp;
		}
		/* convert cdp locations of DVDZ into x locations */
		/* still store in cdps_dvdz */
		for(i=0;i<ncdps_dvdz;i++) 
			x_dvdz[i] = xmin + (cdps_dvdz[i]-cdpxmin)*dcdp;
		free(cdps_velo);
		free(cdps_dvdz);
	
	}

	/* read in ifile if any */

	nhs = 0;
	if(fex==1) {
		ifileread(ifilefp,&nhs,x2picks,x1picks,veltops,velbots,difs,
			dens,qval,pois,
               	     	velavgs,dvdzs,
			(char*)hnames,hnums,npicks,maxhs,maxpks,
			&xmini,&xmaxi,&zmini,&zmaxi,
			&cdpxmini,&cdpxmaxi,&vmini,&vmaxi,
			vtypei,dunitsi,zattribi,&dcdpi,&ihfile);

		if(ivtopbot==0) {
			  ivtopbot=1;
			  vtopbot(maxp,maxcdp,ncdps_velo,x_velo,z_velo,
				v_velo,nps_velo,
				ncdps_dvdz,x_dvdz,zt_dvdz,zb_dvdz, 
				dvdz_dvdz,nps_dvdz,
				nhs,x2picks,x1picks,npicks,xtol, 
				veltops,velbots,velavgs,dvdzs,lhnvelo);
		}
	}


	/* adjust x1beg and x1end to fall on sampled values */
	i1beg = NINT((x1beg-f1)/d1);
	i1beg = MAX(0,MIN(n1-1,i1beg));
	x1beg = f1+i1beg*d1;
	i1end = NINT((x1end-f1)/d1);
	i1end = MAX(0,MIN(n1-1,i1end));
	x1end = f1+i1end*d1;

	/* adjust x2beg and x2end to fall on sampled values */
	i2beg = NINT((x2beg-f2)/d2);
	i2beg = MAX(0,MIN(n2-1,i2beg));
	x2beg = f2+i2beg*d2;
	i2end = NINT((x2end-f2)/d2);
	i2end = MAX(0,MIN(n2-1,i2end));
	x2end = f2+i2end*d2;

	/* allocate space for image bytes */
	n1c = 1+abs(i1end-i1beg);
	n2c = 1+abs(i2end-i2beg);
	cz = ealloc1(n1c*n2c,sizeof(unsigned char));

	/* convert data to be imaged into signed characters */
	zscale = (wclip!=bclip)?255.0/(wclip-bclip):1.0e10;
	zoffset = -bclip*zscale;
	i1step = (i1end>i1beg)?1:-1;
	i2step = (i2end>i2beg)?1:-1;
	if (style==NORMAL) {
		for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
			czp = cz+n1c*n2c-(i2c+1)*n1c;
			for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	} else {
		czp = cz;
		for (i1c=0,i1=i1beg; i1c<n1c; i1c++,i1+=i1step) {
			for (i2c=0,i2=i2beg; i2c<n2c; i2c++,i2+=i2step) {
				zi = zoffset+z[i1+i2*n1]*zscale;
				if (zi<0.0) zi = 0.0;
				if (zi>255.0) zi = 255.0;
				*czp++ = (unsigned char)zi;
			}
		}
	}
	free1float(z);
	
	/* initialize zoom box parameters */
	nxb = nx = (style==NORMAL ? n1c : n2c);
	nyb = ny = (style==NORMAL ? n2c : n1c);
	ixb = iyb = 0;
	czb = cz;
	x1begb = x1beg;  x1endb = x1end;
	x2begb = x2beg;  x2endb = x2end;

	/* connect to X server */
	if ((dpy=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));
	scr = DefaultScreen(dpy);
	black = BlackPixel(dpy,scr);
	white = WhitePixel(dpy,scr);
	
	/* create window */
	win = xNewWindow(dpy,xbox,ybox,wbox,hbox,black,white,"ipick");

	/* if necessary, create private colormap with gray scale */
	if (STREQ(cmap,"gray")) {
		XSetWindowColormap(dpy,win,xCreateGrayColormap(dpy,win));
	} else if (STREQ(cmap,"hue")) {
		XSetWindowColormap(dpy,win,xCreateHueColormap(dpy,win));
	} else if (STREQ(cmap,"rgb")) {
		XSetWindowColormap(dpy,win,xCreateRGBColormap(dpy,win));
	}
	
	/* determine min and max pixels from standard colormap */
	pmin = xGetFirstPixel(dpy);
	pmax = xGetLastPixel(dpy);
		
	/* make GC for image */
	gci = XCreateGC(dpy,win,0,NULL);
	
	/* set normal event mask */
	XSelectInput(dpy,win,
		StructureNotifyMask |
		ExposureMask |
		KeyPressMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask |
		Button1MotionMask |
		Button2MotionMask);
	
	/* map window */
	XMapWindow(dpy,win);
					
	/* determine good size for axes box */
	xSizeAxesBox(dpy,win,
		labelfont,titlefont,style,
		&x,&y,&width,&height);
	
	/* clear the window */
	XClearWindow(dpy,win);
	
	/* note that image is out of date */
	imageOutOfDate = 1;
	savebg = 1;

	/*
	fprintf(stderr,"xmin=%f\n",xmini);
	fprintf(stderr,"xmax=%f\n",xmaxi);
	fprintf(stderr,"zmin=%f\n",zmini);
	fprintf(stderr,"zmax=%f\n",zmaxi);
	fprintf(stderr,"cdpxmin=%d\n",cdpxmini);
	fprintf(stderr,"cdpxmax=%d\n",cdpxmaxi);
	fprintf(stderr,"vmin=%f\n",vmini);
	fprintf(stderr,"vmax=%f\n",vmaxi);
	fprintf(stderr,"vtype=%s\n",vtypei);
	fprintf(stderr,"dunits=%s\n",dunitsi);
	fprintf(stderr,"zattrib=%s\n",zattribi);
	fprintf(stderr,"dcdp=%f\n",dcdpi);
	fprintf(stderr,"nhs=%d\n",nhs);
	for(ih=0;ih<nhs;ih++) fprintf(stderr,"hname=%s\n",hnames[ih]);
	*/

	ih = nhs + 1;
	sprintf(hnames[nhs],"%4s%-3d\0",hname,ih);
	hnums[nhs] = ih;
	nhs = nhs + 1;
	ihsave = 0;
	nold = 0;

	/* main event loop */
	while(True) {
		XNextEvent(dpy,&event);

		/* if window was resized */
		if (event.type==ConfigureNotify &&
			(event.xconfigure.width!=winwidth ||
			 event.xconfigure.height!=winheight)) {
			winwidth = event.xconfigure.width;
			winheight = event.xconfigure.height;
							
			/* determine good size for axes box */
			xSizeAxesBox(dpy,win,
				labelfont,titlefont,style,
				&x,&y,&width,&height);
			/* clear background */
                        if(savebg) XSetWindowBackground(dpy,win,0L);
			
			/* clear the window */
			XClearWindow(dpy,win);
			
			/* note that image is out of date */
			imageOutOfDate = 1;

		/* else if window exposed */
		} else if (event.type==Expose) {
			
			/* clear all expose events from queue */
			while (XCheckTypedEvent(dpy,Expose,&event));
			
			/* if necessary, make new image */
			if (imageOutOfDate) {
				if (czbi!=NULL) free1(czbi);
				czbi = newInterpBytes(nxb,nyb,czb,
					width,height);
				if (image!=NULL) XDestroyImage(image);
				image = xNewImage(dpy,pmin,pmax,
					width,height,czbi);
				imageOutOfDate = 0;
			}

			/* clear background */
                        if(savebg) XSetWindowBackground(dpy,win,0L);
	
			/* draw image (before axes so grid lines visible) */
			XPutImage(dpy,win,gci,image,0,0,x,y,
				image->width,image->height);

			
			/* draw axes on top of image */
			cdpbegb = (x2begb - xmin)/dcdp + cdpxmin; 
			cdpendb = (x2endb - xmin)/dcdp + cdpxmin; 

			xDrawAxesBox(dpy,win,
				x,y,width,height,
				x1begb,x1endb,0.0,0.0,
				d1num,f1num,n1tic,grid1,label1,
				cdpbegb,cdpendb,0.0,0.0,
				d2num,f2num,n2tic,grid2,label2,
				labelfont,title,titlefont,
				labelcolor,titlecolor,gridcolor,
				style);

		/* else if key down */
		} else if (event.type==KeyPress) {

			XLookupString(&event,keybuf,0,&keysym,&keystat);
			/* add the pick to the pick-saved buffers */
			pkey = 99;
			if (keysym==XK_a) {
				pkey = 0;
			/* delete the pick */
			} else if (keysym==XK_d) {
				pkey = 1;
			/* insert the pick */
			} else if (keysym==XK_i) {
				pkey = 2;
			/* display the pick */
			} else if (keysym==XK_p) {
				pkey = 3;
			} else if (keysym==XK_c) {
				pkey = 5;
			} else if (keysym==XK_b) {
				pkey = 99;
				if(ih>1 && npicks[ih-1]==0) nhs = nhs -1; 
				if(ih>1) ih = ih - 1;
			} else if (keysym==XK_f) {
				if(ih<nhs || npicks[nhs-1]>0) ih = ih + 1;
				pkey = 99;
				if(ih>nhs) {
				     sprintf(hnames[nhs],"%4s%-3d\0",hname,ih);
					hnums[nhs] = ih;
					nhs = nhs + 1;
				}
			} else if (keysym==XK_n) {
				pkey = 98;
			} else if (keysym==XK_1) {
				if(nhs>=1) ih = 1;
			} else if (keysym==XK_2) {
				if(nhs>=2) ih = 2;
			} else if (keysym==XK_3) {
				if(nhs>=3) ih = 3;
			} else if (keysym==XK_4) {
				if(nhs>=4) ih = 4;
			} else if (keysym==XK_5) {
				if(nhs>=5) ih = 5;
			} else if (keysym==XK_6) {
				if(nhs>=6) ih = 6;
			} else if (keysym==XK_7) {
				if(nhs>=7) ih = 7;
			} else if (keysym==XK_8) {
				if(nhs>=8) ih = 8;
			} else if (keysym==XK_9) {
				if(nhs>=9) ih = 9;
			} else if (keysym==XK_0) {
				if(nhs>=10) ih = 10;
			} else if (keysym==XK_F1) {
				if(nhs>=11) ih = 11;
			} else if (keysym==XK_F2) {
				if(nhs>=12) ih = 12;
			} else if (keysym==XK_F3) {
				if(nhs>=13) ih = 13;
			} else if (keysym==XK_F4) {
				if(nhs>=14) ih = 14;
			} else if (keysym==XK_F5) {
				if(nhs>=15) ih = 15;
			} else if (keysym==XK_F6) {
				if(nhs>=16) ih = 16;
			} else if (keysym==XK_F7) {
				if(nhs>=17) ih = 17;
			} else if (keysym==XK_F8) {
				if(nhs>=18) ih = 18;
			} else if (keysym==XK_F9) {
				if(nhs>=19) ih = 19;
			} else if (keysym==XK_F10) {
				if(nhs>=20) ih = 20;
			} else if (keysym==XK_F11) {
				if(nhs>=21) ih = 21;
			} else if (keysym==XK_F12) {
				if(nhs>=22) ih = 22;
			}

			/* reinterpolate velocities if needed */

			/* first time, save values */
			if(ihsave==0 && npicks[ih-1]>0) {
				ihsave = ih; 
				jh = ih - 1;
				nold = npicks[jh];
				if(nold > 1) {
					bcopy(x2picks+jh*maxpks,xold,
						nold*sizeof(float));
					bcopy(veltops+jh*maxpks,vtopold,
						nold*sizeof(float));
					bcopy(velbots+jh*maxpks,vbotold,
						nold*sizeof(float));
					bcopy(velavgs+jh*maxpks,vavgold,
						nold*sizeof(float));
					bcopy(dvdzs+jh*maxpks,dvdzold,
						nold*sizeof(float));
				}
			/* change interface, or to edit velocity, we need to
			   interpolate from saved values of the interface  */
			} else if (ih!=ihsave||keysym==XK_T||keysym==XK_B) {
				jh = ihsave - 1;
				/* interpolation */
				if(nold>1 ) {
					bcopy(vtopold,veltops+jh*maxpks,
						nold*sizeof(float));
					bcopy(vbotold,velbots+jh*maxpks,
						nold*sizeof(float));
					bcopy(vavgold,velavgs+jh*maxpks,
						nold*sizeof(float));
					bcopy(dvdzold,dvdzs+jh*maxpks,
						nold*sizeof(float));
				  	interp(xold,x2picks+jh*maxpks,nold, 
						npicks[jh],veltops+jh*maxpks,
						velbots+jh*maxpks,
						velavgs+jh*maxpks, 
						dvdzs+jh*maxpks);
				}
				/* save values of current interface */
				jh = ih - 1;
				nold = npicks[jh];
				if(nold > 1) {
					bcopy(x2picks+jh*maxpks,xold,
						nold*sizeof(float));
					bcopy(veltops+jh*maxpks,vtopold,
						nold*sizeof(float));
					bcopy(velbots+jh*maxpks,vbotold,
						nold*sizeof(float));
					bcopy(velavgs+jh*maxpks,vavgold,
						nold*sizeof(float));
					bcopy(dvdzs+jh*maxpks,dvdzold,
						nold*sizeof(float));
				}
				ihsave = ih;
			}	

			if (keysym==XK_T) {
				if(npicks[nhs-1]==0) nhs = nhs -1; 
				if(ivtopbot==0) {
				  ivtopbot=1;
				  vtopbot(maxp,maxcdp,ncdps_velo,x_velo,z_velo,
					v_velo,nps_velo,
					ncdps_dvdz,x_dvdz,zt_dvdz,zb_dvdz, 
					dvdz_dvdz,nps_dvdz,
					nhs,x2picks,x1picks,npicks,xtol, 
					veltops,velbots,velavgs,dvdzs,lhnvelo);
				}
				nvi = npicks[ih-1];
				sprintf(ivtitle,"%s at interface %d \0",
				"top velocity editing",ih);

				ivedit(x2picks+(ih-1)*maxpks,
					veltops+(ih-1)*maxpks,
				       	hcolors[ih-1],nvi,ivtitle,
					xmin,dcdp,cdpxmin,&isave);

				/* copy edited result to saved buffer */ 
				if(isave==1) bcopy(veltops+(ih-1)*maxpks,
						vtopold,nold*sizeof(float));

				/* keep velocity between 2 adjacent interfaces
				   the same */
				if(itopbot==1&&ih>1&&isave==1&&ihfile==0) {
			  		lin1d_(x2picks+(ih-1)*maxpks,
					       veltops+(ih-1)*maxpks,
					       &npicks[ih-1],
					       x2picks+(ih-2)*maxpks,
					       velbots+(ih-2)*maxpks,
					       &npicks[ih-2],
					       indx);
				} else if(itopbot==1&&isave==1&&ihfile!=0) {
					lin1d_(x2picks+(ih-1)*maxpks,
					       veltops+(ih-1)*maxpks,
					       &npicks[ih-1],
					       x2picks+(ih-1)*maxpks,
					       velbots+(ih-1)*maxpks,
					       &npicks[ih-1],
					       indx);
				}
			} else if (keysym==XK_B) {
				if(npicks[nhs-1]==0) nhs = nhs -1; 
				if(ivtopbot==0) {
				  ivtopbot=1;
				  vtopbot(maxp,maxcdp,ncdps_velo,x_velo,z_velo,
					v_velo,nps_velo,
					ncdps_dvdz,x_dvdz,zt_dvdz,zb_dvdz, 
					dvdz_dvdz,nps_dvdz,
					nhs,x2picks,x1picks,npicks,xtol, 
					veltops,velbots,velavgs,dvdzs,lhnvelo);
				}
				nvi = npicks[ih-1];
				sprintf(ivtitle,"%s at interface %d \0",
				"bottom velocity editing",ih);

				ivedit(x2picks+(ih-1)*maxpks,
					velbots+(ih-1)*maxpks,
				       	hcolors[ih-1],nvi,ivtitle,
					xmin,dcdp,cdpxmin,&isave);
				/* copy edited result to saved buffer */ 
				if(isave==1) bcopy(velbots+(ih-1)*maxpks,
						vbotold,nold*sizeof(float));

				/* keep velocity between 2 adjacent interfaces
				   the same */
				if(itopbot==1&&ih<nhs&&isave==1&&ihfile==0) {
					lin1d_(x2picks+(ih-1)*maxpks,
					       velbots+(ih-1)*maxpks,
					       &npicks[ih-1],
					       x2picks+(ih)*maxpks,
					       veltops+(ih)*maxpks,
					       &npicks[ih],
					       indx);
				} else if(itopbot==1&&isave==1&&ihfile!=0) {
					lin1d_(x2picks+(ih-1)*maxpks,
					       velbots+(ih-1)*maxpks,
					       &npicks[ih-1],
					       x2picks+(ih-1)*maxpks,
					       veltops+(ih-1)*maxpks,
					       &npicks[ih-1],
					       indx);
				}
				
			}
			if (pkey<=5) {
				xMousePicks(dpy,win,event,style,
					hcolors,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					x1picks,x2picks,veltops,velbots,difs,
					npicks,nhs,ih,gci,pkey,&savebg);
			/* output the picks */
			} else if (keysym==XK_s) {
				if(fex==1) {
                			fclose(mpicksfp);
                			mpicksfp = efopen(mpicks,"w");
				}
				fex = 1;
				if(npicks[nhs-1]==0) nhs = nhs - 1;
				/* find top and bottom velocities of 
				   interfaces */
				if(ivtopbot==0) {
				  ivtopbot=1;
				  vtopbot(maxp,maxcdp,ncdps_velo,x_velo,z_velo,
					v_velo,nps_velo,
					ncdps_dvdz,x_dvdz,zt_dvdz,zb_dvdz, 
					dvdz_dvdz,nps_dvdz,
					nhs,x2picks,x1picks,npicks,xtol, 
					veltops,velbots,velavgs,dvdzs,lhnvelo);
				}

				xMousePrint(dpy,win,event,style,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					x1picks,x2picks,veltops,velbots,difs,
					velavgs,dvdzs,
					npicks,nhs,ism,
					xmin,xmax,zmin,zmax,
					cdpxmin,cdpxmax,dcdp,
					hnums,hnames,
					dunits,zattrib,idif,ihfile);
			} else if (keysym==XK_Q) {
			/* This is the exit from the event loop */
				break;
			} else if(pkey==98) {
       				XClearWindow(dpy, win);
			} else {
				continue;
			}


		/* else if button down (1 == zoom, 2 == mouse tracking */
		} else if (event.type==ButtonPress) {
			/* if 1st button: zoom */
			if (event.xbutton.button==Button1) {

				savebg = 1;
				/* track pointer and get new box */
				xRubberBox(dpy,win,event,&xb,&yb,&wb,&hb);
			
				/* if new box has tiny width or height */
				if (wb<4 || hb<4) {
				
					/* reset box to initial values */
					x1begb = x1beg;
					x1endb = x1end;
					x2begb = x2beg;
					x2endb = x2end;
					nxb = nx;
					nyb = ny;
					ixb = iyb = 0;
					if (czb!=cz) free1(czb);
					czb = cz;
			
				/* else, if new box has non-zero width
				/* and height */
				} else {
			
					/* calculate new box parameters */
					if (style==NORMAL) {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x1begb,x1endb,
						    nyb,iyb,x2endb,x2begb,
						    &nxb,&ixb,&x1begb,&x1endb,
						    &nyb,&iyb,&x2endb,&x2begb);
					} else {
					    zoomBox(x,y,width,height,
						    xb,yb,wb,hb,
						    nxb,ixb,x2begb,x2endb,
						    nyb,iyb,x1begb,x1endb,
						    &nxb,&ixb,&x2begb,&x2endb,
						    &nyb,&iyb,&x1begb,&x1endb);
					}
			
					/* make new bytes in zoombox */
					if (czb!=cz) free1(czb);
					czb = ealloc1(nxb*nyb,
						sizeof(signed char));
					for (i=0,czbp=czb; i<nyb; i++) {
					    czp = cz+(iyb+i)*nx+ixb;
					    for (j=0; j<nxb; j++)
						    *czbp++ = *czp++; 
					}
				}
			
				/* clear background */
                        	if(savebg) XSetWindowBackground(dpy,win,0L);

				/* clear area and force an expose event */
				XClearArea(dpy,win,0,0,0,0,True);
			
				/* note that image is out of date */
				imageOutOfDate = 1;
		
			/* else if 2nd button down: display mouse coords */
			} else if (event.xbutton.button==Button2) {

				showloc = 1;
				xMouseLoc(dpy,win,event,style,showloc,
					  x,y,width,height,x1begb,x1endb,
					  x2begb,x2endb);

			/* else if 3rd button down: track and pick */
                        } else if (event.xbutton.button==Button3) {
				if(ihsave==0 && npicks[ih-1]>0) {
					ihsave = ih; 
					jh = ih - 1;
					nold = npicks[jh];
					if(nold > 1) {
						bcopy(x2picks+jh*maxpks,xold,
							nold*sizeof(float));
						bcopy(veltops+jh*maxpks,vtopold,
							nold*sizeof(float));
						bcopy(velbots+jh*maxpks,vbotold,
							nold*sizeof(float));
						bcopy(velavgs+jh*maxpks,vavgold,
							nold*sizeof(float));
						bcopy(dvdzs+jh*maxpks,dvdzold,
							nold*sizeof(float));
					}
				} else if (ih!=ihsave) {
					jh = ihsave - 1;
					if(nold>1 ) {
					  bcopy(vtopold,veltops+jh*maxpks,
						nold*sizeof(float));
					  bcopy(vbotold,velbots+jh*maxpks,
						nold*sizeof(float));
					  bcopy(vavgold,velavgs+jh*maxpks,
						nold*sizeof(float));
					  bcopy(dvdzold,dvdzs+jh*maxpks,
						nold*sizeof(float));
					  interp(xold,x2picks+jh*maxpks,nold, 
						npicks[jh],veltops+jh*maxpks,
						velbots+jh*maxpks,
						velavgs+jh*maxpks, 
						dvdzs+jh*maxpks);
					}
					jh = ih - 1;
					nold = npicks[jh];
					if(nold > 1) {
						bcopy(x2picks+jh*maxpks,xold,
							nold*sizeof(float));
						bcopy(veltops+jh*maxpks,vtopold,
							nold*sizeof(float));
						bcopy(velbots+jh*maxpks,vbotold,
							nold*sizeof(float));
						bcopy(velavgs+jh*maxpks,vavgold,
							nold*sizeof(float));
						bcopy(dvdzs+jh*maxpks,dvdzold,
							nold*sizeof(float));
					}
					ihsave = ih;
				}	
				pkey = 0;
				xMousePicks(dpy,win,event,style,
					hcolors,
					mpicksfp, x,y,width,height,
					x1begb,x1endb,x2begb,x2endb,
					x1picks,x2picks,veltops,velbots,difs,
					npicks,nhs,ih,gci,pkey,&savebg);
			} else {
				continue;
			}

		/* else if pointer has moved */
		} else if (event.type==MotionNotify) {
			
			/* if button2 down, show mouse location */
			if (showloc)
				xMouseLoc(dpy,win,event,style,True,
					x,y,width,height,
					x1begb,x1endb,x2begb,x2endb);

		/* else if button2 released, stop tracking */
		} else if (event.type==ButtonRelease &&
			   event.xbutton.button==Button2) {
			showloc = 0;
		}

	} /* end of event loop */

	/* close connection to X server */
	XCloseDisplay(dpy);
	return EXIT_SUCCESS; 
}

/* update parameters associated with zoom box */
static void zoomBox (int x, int y, int w, int h, 
	int xb, int yb, int wb, int hb,
	int nx, int ix, float x1, float x2,
	int ny, int iy, float y1, float y2,
	int *nxb, int *ixb, float *x1b, float *x2b,
	int *nyb, int *iyb, float *y1b, float *y2b)
{
	/* if width and/or height of box are zero, just copy values */
	if (wb==0 || hb==0) {
		*nxb = nx; *ixb = ix; *x1b = x1; *x2b = x2;
		*nyb = ny; *iyb = iy; *y1b = y1; *y2b = y2;
		return;		
	} 
	
	/* clip box */
	if (xb<x) {
		wb -= x-xb;
		xb = x;
	}
	if (yb<y) {
		hb -= y-yb;
		yb = y;
	}
	if (xb+wb>x+w) wb = x-xb+w;
	if (yb+hb>y+h) hb = y-yb+h;
	
	/* determine number of samples in rubber box (at least 2) */
	*nxb = MAX(nx*wb/w,2);
	*nyb = MAX(ny*hb/h,2);
	
	/* determine indices of first samples in box */
	*ixb = ix+(xb-x)*(nx-1)/w;
	*ixb = MIN(*ixb,ix+nx-*nxb);
	*iyb = iy+(yb-y)*(ny-1)/h;
	*iyb = MIN(*iyb,iy+ny-*nyb);
	
	
	/* determine box limits to nearest samples */
	*x1b = x1+(*ixb-ix)*(x2-x1)/(nx-1);
	*x2b = x1+(*ixb+*nxb-1-ix)*(x2-x1)/(nx-1);
	*y1b = y1+(*iyb-iy)*(y2-y1)/(ny-1);
	*y2b = y1+(*iyb+*nyb-1-iy)*(y2-y1)/(ny-1);
}

/* return pointer to new interpolated array of bytes */
static unsigned char *newInterpBytes (int n1in, int n2in, unsigned char *bin,
	int n1out, int n2out)
{
	unsigned char *bout;
	float d1in,d2in,d1out,d2out,f1in,f2in,f1out,f2out;
	
	f1in = f2in = f1out = f2out = 0.0;
	d1in = d2in = 1.0;
	d1out = d1in*(float)(n1in-1)/(float)(n1out-1);
	d2out = d2in*(float)(n2in-1)/(float)(n2out-1);
	bout = ealloc1(n1out*n2out,sizeof(unsigned char));
	intl2b(n1in,d1in,f1in,n2in,d2in,f2in,bin,
		n1out,d1out,f1out,n2out,d2out,f2out,bout);
	return bout;
}
	
void xMouseLoc(Display *dpy, Window win, XEvent event, int style, Bool show,
	int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb)
{
	XGCValues values;
	static XFontStruct *fs=NULL;
	static XCharStruct overall;
	static GC gc;
	int dummy,xoffset=10,yoffset=10;
	float x1,x2;
	char string[256];

	/* if first time, get font attributes and make gc */
	if (fs==NULL) {
		fs = XLoadQueryFont(dpy,"fixed");
		gc = XCreateGC(dpy,win,0,&values);
		XSetFont(dpy,gc,fs->fid);
		overall.width = 1;
		overall.ascent = 1;
		overall.descent = 1;
	}

	/* erase previous string */
	XClearArea(dpy,win,xoffset,yoffset,
		overall.width,overall.ascent+overall.descent,False);

	/* if not showing, then return */
	if (!show) return;

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	/* draw string indicating mouse location */
	sprintf(string,"(%0.4g,%0.4g)",x1,x2);
	XTextExtents(fs,string,strlen(string),&dummy,&dummy,&dummy,&overall);
	XDrawImageString(dpy,win,gc,xoffset,yoffset+overall.ascent,
		string,strlen(string));
}


void xMousePrint(Display *dpy, Window win, XEvent event, int style,
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *x1picks, float *x2picks, float *veltop, float *velbot, int *dif,
	float *velavg, float *dvdz,
	int *npicks, int nhs, int ism, 
	float xmin, float xmax, float zmin, float zmax,
	int cdpxmin, int cdpxmax, float dcdp, 
	int *hnums, char hnames[maxhs][80],
	char *dunits, char *zattrib, int idif, int ihfile) {


	int ic,ip,np,ih,jp,nhout,h0; 
	int p1,p2,i,ismx,ismxh, izt;
	float vmax, vmin, s;
	float *w, *f;
	float vt, vb, g;



	/* lateral smooth velocities */
	if(ism==0) {
	/* take average */ 
		for(ih=0;ih<nhs;ih++) {
			vt = 0.;
			vb = 0.;
			g = 0.;
			np = npicks[ih];
			for(ip=0;ip<np;ip++) {
				vt += veltop[ip+ih*maxpks];
				vb += velbot[ip+ih*maxpks];
				g += dvdz[ip+ih*maxpks];
			}
			if(np>0) {
				vt = vt/np;
				vb = vb/np;
				g = g/np;
			}
			for(ip=0;ip<np;ip++) {
				veltop[ip+ih*maxpks] = vt;
				velbot[ip+ih*maxpks] = vb;
				dvdz[ip+ih*maxpks] = g;
			}
		}
	} else if (ism>1) {
	/* smoothing */
		for(ih=0;ih<nhs;ih++) {
			h0 = ih * maxpks;
			np = npicks[ih];
			if(np<2) continue;
			ismx = ism;
			ismx = (ismx/2)*2+1;
			if(ismx>np) ismx=(np/2)*2+1;
			f = (float *) malloc(ismx*sizeof(float));
			w = (float *) malloc(np*sizeof(float));
			/* triangular smoothing coefficients */
			ismxh = (ismx+1)/2;
			s = 0.;
			for(i=0;i<ismx;i++) {
				f[i] = ismxh - abs(i+1-ismxh);
				s += f[i];
			}
			s = 1./s;
			for(i=0;i<ismx;i++) {
				f[i]=f[i]*s;
			}
			/* apply smoothing to veltop, velbot and dvdz */
			ic = 1;
			smth1d_(veltop+h0,f,w,&np,&ismx,&ic);
			for(ip=0;ip<np;ip++) veltop[ip+h0] = w[ip];
			smth1d_(velbot+h0,f,w,&np,&ismx,&ic);
			for(ip=0;ip<np;ip++) velbot[ip+h0] = w[ip];
			smth1d_(dvdz+h0,f,w,&np,&ismx,&ic);
			for(ip=0;ip<np;ip++) dvdz[ip+h0] = w[ip];

			free(w);
			free(f);
		}
	}

	/* find maximum and minimum velocities */
	vmin = veltop[0];
	vmax = veltop[0];
	for (ih=0;ih<nhs;ih++) {
		for(ip=0;ip<npicks[ih];ip++) {
			jp = ip + ih * maxpks;
			if(vmin>veltop[jp]) vmin = veltop[jp];
			if(vmin>velbot[jp]) vmin = velbot[jp];
			if(vmax<veltop[jp]) vmax = veltop[jp];
			if(vmax<velbot[jp]) vmax = velbot[jp];
		}
	}

	nhout = 0;
	for(ih=0;ih<nhs;ih++) {
		if(npicks[ih]>1) nhout = nhout + 1;
	}


	/* output picks */

	if(ihfile==0) {
		fprintf(mpicksfp, 
		"IFILE   <--- FILE FORMAT DESCRIPTOR (INTERFACE FILE) \n");
	} else {
		fprintf(mpicksfp, "HFILE  \n");
	}

	fprintf(mpicksfp,"\n");
	fprintf(mpicksfp, "MODEL.X-MIN         %8.1f\n",xmin);
	fprintf(mpicksfp, "MODEL.X-MAX         %8.1f\n",xmax);
	fprintf(mpicksfp, "MODEL.Z-MIN         %8.1f \n",zmin);
	fprintf(mpicksfp, "MODEL.Z-MAX         %8.1f \n",zmax);
	fprintf(mpicksfp, "MODEL.CDP#-AT-X-MIN %6d\n",cdpxmin);
	fprintf(mpicksfp, "MODEL.CDP#-AT-X-MAX %6d\n",cdpxmax);
	fprintf(mpicksfp, "MODEL.MIN-VELOCITY  %8.1f\n",vmin); 
	fprintf(mpicksfp, "MODEL.MAX-VELOCITY  %8.1f\n",vmax); 
	fprintf(mpicksfp, "MODEL.#HORIZONS     %6d\n",nhout); 
	fprintf(mpicksfp, "MODEL.VELOCITY-TYPE  INTERVAL\n"); 
	fprintf(mpicksfp, "MODEL.DIST-UNITS     %s\n",dunits); 
	fprintf(mpicksfp, "MODEL.Z-ATTRIBUTE    %s\n",zattrib); 
	fprintf(mpicksfp, "MODEL.CDP-SPACING   %8.1f\n",dcdp); 
	fprintf(mpicksfp, "\n"); 

	if( zattrib=="DEPTH SECTION" ) {
		izt = 1; 
	} else {
		izt = 0; 
	}

	
	for(ih=0;ih<nhs;ih++) {

		np = npicks[ih];
		if(np<=1) continue;

		fprintf(mpicksfp, "\n"); 
		fprintf(mpicksfp, "\n"); 
		fprintf(mpicksfp, "HORIZON-NUMBER %3d\n", hnums[ih]); 
		fprintf(mpicksfp, "HORIZON-NAME     %s\n", hnames[ih]); 
		fprintf(mpicksfp, "* \n"); 
		fprintf(mpicksfp, 
"* X-VALUE   Z-VALUE   VELTOP   VELBOT  DIF  DENS  QVAL  POIS   VELAVG   DVDZ \n");
		fprintf(mpicksfp, 
"* =======  ========  =======  =======  ===  ====  ====  ====  =======  ======= \n");

		h0 = ih*maxpks;
		for(ip=1;ip<np-1;ip++) dif[h0+ip]=idif;
		dif[h0] = 1;
		dif[h0+np-1] = 1;
		
		for (ip=0;ip<npicks[ih];ip++) {

			if(izt==1) {
   			fprintf(mpicksfp, 
"%9.1f %9.1f %8.0f %8.0f   %1d   0.00    0.  .000 %8.0f %8.0f \n",
				x2picks[ip+h0],x1picks[ip+h0],
				veltop[ip+h0],velbot[ip+h0],dif[ip+h0],
				velavg[ip+h0],dvdz[ip+h0]);
			} else {
   			fprintf(mpicksfp, 
"%9.1f %9.3f %8.0f %8.0f   %1d   0.00    0.  .000 %8.0f %8.0f \n",
				x2picks[ip+h0],x1picks[ip+h0],
				veltop[ip+h0],velbot[ip+h0],dif[ip+h0],
				velavg[ip+h0],dvdz[ip+h0]);
			}
		}
	}
	fflush(mpicksfp);
}

void xMousePicks(Display *dpy, Window win, XEvent event, int style,
	char hcolors[maxhs][10],
	FILE *mpicksfp, int x, int y, int width, int height,
	float x1begb, float x1endb, float x2begb, float x2endb,
	float *x1picks, float *x2picks, float *veltop, float *velbot, int *dif,
	int *npicks, int nhs, int ih, GC gc, int pkey, int *savebg)
{
	float x1,x2;
	int ipicks;
	int xx1,yy1,xx2,yy2,temp,ip,jp;
	int dismin,dis;
	int ipmin, ipins, offset, jh, itmp;
	char hnum[3];

	GC gchi;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
        int scr;

	/* save bitmap of window to background for retrival */ 
	if(*savebg) {
		fg2bg(dpy,win);
		*savebg = 0;
	}

	/* convert mouse location to (x1,x2) coordinates */
	if (style==NORMAL) {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.x-x)/width;
		x2 = x2endb+(x2begb-x2endb)*(event.xmotion.y-y)/height;
	} else {
		x1 = x1begb+(x1endb-x1begb)*(event.xmotion.y-y)/height;
		x2 = x2begb+(x2endb-x2begb)*(event.xmotion.x-x)/width;
	}

	offset = (ih-1)*maxpks;
	jh = ih - 1;
	/* save to x1picks and x2picks */
	ipicks = npicks[jh];
	if( pkey == 0 ) {
		jp = ipicks + offset;
		x1picks[jp] = x1;
		x2picks[jp] = x2;
		npicks[jh] += 1;
	/* delete nearest picks from x1picks and x2picks*/
	} else if ( pkey==1 && ipicks > 0 ) {
		dismin=(x1-x1picks[offset])*(x1-x1picks[offset])+
		       (x2-x2picks[offset])*(x2-x2picks[offset]);
		ipmin = 0;
		for(ip=1;ip<ipicks;ip++) {
			jp = ip + offset;
		   	dis=(x1-x1picks[jp])*(x1-x1picks[jp])+
		       		(x2-x2picks[jp])*(x2-x2picks[jp]);
			if(dis<dismin) {
				dismin = dis;
				ipmin = ip;
			}
		}
		ipicks = ipicks - 1;
		for(ip=ipmin;ip<ipicks;ip++) {
			jp = ip + offset;
			x1picks[jp] = x1picks[jp+1];
			x2picks[jp] = x2picks[jp+1];
		}
		npicks[jh] = ipicks;
	/* insert the pick */
	} else if ( pkey==2 ) {
		dismin=(x2-x2picks[offset])*(x2-x2picks[offset]);
		ipmin = 0;
		for(ip=1;ip<ipicks;ip++) {
			jp = ip + offset;
			dis=(x2-x2picks[jp])*(x2-x2picks[jp]);
			if(dis<dismin) {
				dismin = dis;
				ipmin = ip;
			}
		}
		if(x2>x2picks[ipmin+offset]) { 
			ipins = ipmin+1;
			ipicks +=1;
			for(ip=ipicks;ip>ipins;ip--) {
				jp = ip+offset;
				x1picks[jp] = x1picks[jp-1];
				x2picks[jp] = x2picks[jp-1];
			}
			x1picks[ipins+offset] = x1;
			x2picks[ipins+offset] = x2;
		} else if(x2<x2picks[ipmin+offset]) {
			ipins = ipmin;
			ipicks +=1;
			for(ip=ipicks;ip>ipins;ip--) {
				jp = ip + offset;
				x1picks[jp] = x1picks[jp-1];
				x2picks[jp] = x2picks[jp-1];
			}
			x1picks[ipins+offset] = x1;
			x2picks[ipins+offset] = x2;
		}
		npicks[jh] = ipicks;
	}
	if ( pkey==5 ) {
		for(ip=0;ip<npicks[jh];ip++) {
			jp = ip + offset;
			x1picks[jp] = 0.;
			x2picks[jp] = 0.;
		}
		npicks[jh] = 0;
		ipicks = 0;
	}

       	XClearWindow(dpy, win);

	/* draw lines between picks */
	for(jh=0;jh<nhs;jh++) {
	
		/* get screen */
                scr = DefaultScreen(dpy);
                /* determine window's current colormap */
                XGetWindowAttributes(dpy,win,&wa);
                cmap = wa.colormap;
		/* create graphics contexts */
                gchi = XCreateGC(dpy,win,0,values);
                if (XAllocNamedColor(dpy,cmap,hcolors[jh],&scolor,&ecolor))
                        XSetForeground(dpy,gchi,ecolor.pixel);
                else
                        XSetForeground(dpy,gchi,1L);
                XSetLineAttributes(dpy,gchi,2,LineSolid,CapButt,JoinMiter);
		ipicks = npicks[jh];
		offset = jh*maxpks;
		itmp = ipicks / 5;
		if(itmp<2) itmp=ipicks/2;
		if(ipicks>1) {
			for(ip=1;ip<ipicks;ip++) {
				jp = ip + offset;
				/*
				if(x1picks[jp-1]<x1begb ||
				   x1picks[jp-1]>x1endb ||
				   x1picks[jp]<x1begb ||
				   x1picks[jp]>x1endb ||
				   x2picks[jp-1]<x2begb ||
				   x2picks[jp-1]>x2endb ||
				   x2picks[jp]<x2begb ||
				   x2picks[jp]>x2endb) continue;
				*/

				if (style==NORMAL) {
			xx1=(x1picks[jp-1]-x1begb)/(x1endb-x1begb)*width+x;
			yy1=(x2picks[jp-1]-x2endb)/(x2begb-x2endb)*height+y;
			xx2=(x1picks[jp]-x1begb)/(x1endb-x1begb)*width+x;
			yy2=(x2picks[jp]-x2endb)/(x2begb-x2endb)*height+y;
				} else {
			yy1=(x1picks[jp-1]-x1begb)/(x1endb-x1begb)*height+y;
			xx1=(x2picks[jp-1]-x2begb)/(x2endb-x2begb)*width+x;
			yy2=(x1picks[jp]-x1begb)/(x1endb-x1begb)*height+y;
			xx2=(x2picks[jp]-x2begb)/(x2endb-x2begb)*width+x;
				}
				XDrawLine(dpy,win,gchi,xx1,yy1,xx2,yy2);
				if(ip==1) {
					XDrawLine(dpy,win,gchi,
						xx1-4,yy1,xx1+4,yy1);
					XDrawLine(dpy,win,gchi,
						xx1,yy1-4,xx1,yy1+4);
				}
				XDrawLine(dpy,win,gchi,xx2-4,yy2,xx2+8,yy2);
				XDrawLine(dpy,win,gchi,xx2,yy2-4,xx2,yy2+4);
				if( (ip%itmp)==0 ) {
					sprintf(hnum,"%-3d",jh+1);
					XDrawString(dpy,win,gchi,
                        		xx2,yy2,hnum,strlen(hnum));
				} 
			}
			sprintf(hnum,"%-3d",jh+1);
			XDrawString(dpy,win,gchi,
                        xx2,yy2,hnum,strlen(hnum));
			
			XFreeGC(dpy,gchi);
		}
	}
}


/* compute velocities from VELO and DVDZ cards */
void vtopbot(int maxp,int maxcdp, 
     int ncdp_velo,float *x_velo,float *z_velo,float *v_velo,int *nps_velo,
     int ncdp_dvdz,float *x_dvdz,float *zt_dvdz,float *zb_dvdz, 
     float *dvdz_dvdz, int *nps_dvdz,
     int nhs,float *xpicks,float *zpicks,int *npicks,float xtol, 
     float *veltops,float *velbots,float *velavgs,float *dvdzs,int lhnvelo) {

	float x, z, va, g, vat, gt, vab, gb;
	int ih, ip, jh;
	float zmax, xmin, xmax, zt, zb, tmp;
	float *vth, *vbh, *vah, *dvdzh, *xvh, *zvs, *zvh;
	float *zgrid, *vagrid, *dvdzgrid, dz, *g1, *g2, *z2, z21, z2n;
	/* float *vt2, *vb2, *va2, *dvdz2; */
	int i1,i2,n1,n2,one,nzgrid;
	int *ivs, *nvh;
	int iz, n, j2, j1, nvs;
	float *xnew, *znew, *vanew, *vtnew, *vbnew, *gnew, xpre, xnow;
	int *indx, inow;

	float *zsort;
	int *isort;

	FILE *tty;
        char vtopyes,vbotyes;
	
	
	if(ncdp_velo==0) {
		/* if no velo card input, simply return with warning message */
		warn("no VELO card input, ifile editing may be needed ! \n");
	} else {
		/* find maximum depth */
		zmax = 0.;
		for(ih=0;ih<nhs;ih++) {
			for(iz=0;iz<npicks[ih];iz++) {
				if(zmax<zpicks[ih*maxpks+iz])
				  	zmax=zpicks[ih*maxpks+iz];
			}
		}


		for(i2=0;i2<ncdp_velo;i2++) {
			for(iz=0;iz<nps_velo[i2];iz++) {
				if(zmax<z_velo[i2*maxp+iz])
				  	zmax=z_velo[i2*maxp+iz];
			}
		}




		nzgrid = 1024;
		zgrid = (float *) malloc(nzgrid*sizeof(float));
		vagrid = (float *) malloc(nzgrid*ncdp_velo*sizeof(float));
		dvdzgrid = (float *) malloc(nzgrid*ncdp_velo*sizeof(float));
		g1 = (float *) malloc(nzgrid*sizeof(float));
		g2 = (float *) malloc(nzgrid*sizeof(float));

		vth = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		vbh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		vah = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		dvdzh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		xvh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		zvh = (float *) malloc(nhs*ncdp_velo*sizeof(float));
		nvh = (int *) malloc(nhs*sizeof(int));

		zvs = (float *) malloc(nhs*sizeof(float));
		ivs = (int *) malloc(nhs*sizeof(int));
		zsort = (float *) malloc(nhs*sizeof(float));
		isort = (int *) malloc(nhs*sizeof(int));

		vtnew = (float *) malloc(maxpks*sizeof(float));
		vbnew = (float *) malloc(maxpks*sizeof(float));
		indx = (int *) malloc(maxpks*sizeof(int));
		vanew = (float *) malloc(maxpks*sizeof(float));
		gnew = (float *) malloc(maxpks*sizeof(float));
		xnew = (float *) malloc(maxpks*sizeof(float));
		znew = (float *) malloc(maxpks*sizeof(float));

		z2 = (float *) malloc(nhs*sizeof(float));
		/* spline interpolation of velocities --- not a good idea 
		vt2 = (float *) malloc(maxpks*sizeof(float));
		vb2 = (float *) malloc(maxpks*sizeof(float));
		va2 = (float *) malloc(maxpks*sizeof(float));
		dvdz2 = (float *) malloc(maxpks*sizeof(float));
		*/

	

		dz = zmax/(nzgrid-2);
		for(iz=0;iz<nzgrid;iz++) zgrid[iz] = iz * dz;
		/* compute average velocity and gradient at velo location and
		   at depth zgrid */


		for(i2=0;i2<ncdp_velo;i2++) {
			n2 = i2*nzgrid;
			/* find gradient */
			x = x_velo[i2];

			if(ncdp_dvdz==0) {
				for(iz=0;iz<nzgrid;iz++) 
					dvdzgrid[iz+n2] = 0.;
			} else if (ncdp_dvdz==1) {
				n = nps_dvdz[0];
				dvdzint(zt_dvdz,zb_dvdz,dvdz_dvdz,n,
					zgrid,dvdzgrid+n2,nzgrid);
			} else {
				bisear_(&ncdp_dvdz,&one,x_dvdz,&x,&j2);
				if(x<=x_dvdz[0]) {
					n = nps_dvdz[0];
					dvdzint(zt_dvdz,zb_dvdz,dvdz_dvdz,n,
					     zgrid,dvdzgrid+n2,nzgrid);
				} else if (x>=x_dvdz[ncdp_dvdz-1]) {
					n = nps_dvdz[ncdp_dvdz-1];
					i1 = (ncdp_dvdz-1)*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,zgrid,
						dvdzgrid+n2,nzgrid);
				} else {
					j1 = j2 - 1;
					n = nps_dvdz[j1];
					i1 = j1*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,
						zgrid,g1,nzgrid);
					n = nps_dvdz[j2];
					i1 = j2*maxp;
					dvdzint(zt_dvdz+i1,zb_dvdz+i1,
						dvdz_dvdz+i1,n,
						zgrid,g2,nzgrid);
					tmp = (x-x_dvdz[j1]) /
					      (x_dvdz[j2]-x_dvdz[j1]);
				
					for(iz=0;iz<nzgrid;iz++) {
						dvdzgrid[iz+n2] = g1[iz]+
						tmp * (g2[iz]-g1[iz]);
					}
				}
			}
			/* find average velocity at zgrid */
			n = nps_velo[i2];
			n1 = i2*maxp;
			j1 = 0;
			j2 = 0;
			vconint(z_velo+n1,v_velo+n1,n,zgrid,vagrid+n2,nzgrid,
				j1,j2,dvdzgrid+n2);

		}
			


		/* compute vt and vb along horizons at velan locations */

		one = 1;
		for(ih=0;ih<nhs;ih++) nvh[ih] = 0;
		for(i2=0;i2<ncdp_velo;i2++) {
			x = x_velo[i2];	
			jh = 0;
			/* find cross points of horizons at velan locations */

			for(ih=0;ih<nhs;ih++) {
				xmin = xpicks[ih*maxpks]-xtol;
				xmax = xpicks[ih*maxpks+npicks[ih]-1]+xtol;
				n1 = npicks[ih]; 
				n2 = ih * maxpks;
				if(x>=xmin && x<=xmax) {
					ivs[jh] = ih;
					/* find z */
					bisear_(&n1,&one,xpicks+n2,&x,&i1);
					if(x<=xpicks[n2]) {
						z = zpicks[n2];
					} else if(x>=xpicks[n1+n2-1]) {
						z = zpicks[n1+n2-1];
					} else {
						/*
						if(n1==2) {
						*/
						/* linear interpolation */
						   z = zpicks[n2+i1-1] +
						    (x-xpicks[n2+i1-1])*
						    (zpicks[n2+i1]-
						     zpicks[n2+i1-1])/
						    (xpicks[n2+i1]-
						     xpicks[n2+i1-1]);
						/*
						} else {
						*/
						/* spline interpolation */
						/* not a good idea */
						/*
						   z21 = 0.;
                                 		   z2n = 0.;
                                                   spline_(xpicks+n2,zpicks+n2,
                                         		   &n1,&z21,&z2n,z2);
                                 		   splint_(xpicks+n2,zpicks+n2,
							   z2,&n1,&x,&z);
						}
						*/
					}
					zvs[jh] = z; 
					jh = jh + 1;
				}
			}

			/* sort zvs into ascending order */
			for (iz=0;iz<jh;iz++) {
				isort[iz] = iz;
				zsort[iz] = zvs[iz];
			}

			qkisort(jh,zsort,isort);
			for(iz=0;iz<jh;iz++) zvs[iz] = zsort[isort[iz]];

			/* find vt and vb */
			for(iz=0;iz<jh;iz++) {

				z = zvs[iz];

				if(iz==0) {
					zt = z-0.1*dz;
				} else {
					zt = zvs[iz-1];
				}	
				if(iz==jh-1) {
					zb = z+dz*0.1;
				} else {
					zb = zvs[iz+1];
				}

				tmp = zt/dz;
				i1 = tmp;
				if (i1<0) {
					i1 = 0;
				} else if (i1>=nzgrid) {
					i1 = nzgrid-1;
				}
				vat = vagrid[i2*nzgrid+i1];
				gt = dvdzgrid[i2*nzgrid+i1];

				tmp = z/dz;
				i1 = tmp;
				if (i1<0) {
					i1 = 0;
				} else if (i1>=nzgrid) {
					i1 = nzgrid-1;
				}
				va = vagrid[i2*nzgrid+i1];
				g = dvdzgrid[i2*nzgrid+i1];
				
				tmp = zb/dz;
				i1 = tmp;
				if (i1<0) {
					i1 = 0;
				} else if (i1>=nzgrid) {
					i1 = nzgrid-1;
				}
				vab = vagrid[i2*nzgrid+i1];
				gb = dvdzgrid[i2*nzgrid+i1];

				/*
				fprintf(stderr,
					"zt=%f zb=%f x=%f z=%f \n",
					zt,zb,x,z);

				fprintf(stderr,
					"va=%f vab=%f vat=%f \n",
					va,vab,vat);
				*/
				
				i1 = ivs[isort[iz]]*ncdp_velo
					+nvh[ivs[isort[iz]]];

				vth[i1] = (z*va-zt*vat)/(z-zt) + 0.5*gt*(z-zt);
				vbh[i1] = (zb*vab-z*va)/(zb-z) - 0.5*gb*(zb-z);
				vah[i1] = va;
				dvdzh[i1] = g;
				xvh[i1] = x;
				zvh[i1] = z;
				nvh[ivs[isort[iz]]] += 1;
				if(iz==0) {
					vth[i1] = va;
				} else if (iz==jh-1) {
					vbh[i1] = vth[i1];
				}
				/*
				fprintf(stderr,
			"vt=%6.0f vb=%6.0f va=%6.0f g=%6.0f x=%6.0f z=%6.0f \n",
					vth[i1],vbh[i1],vah[i1],dvdzh[i1],x,z);
				*/
			}
		}


		/* fing vt, bt, dvdz and va at pick positions */
		for(ih=lhnvelo;ih<nhs;ih++) {
			tty = efopen("/dev/tty","r");
                        fprintf(stderr,
      "Replace Top velocities in horizon %d with those in VELO? (y/n) ->",ih+1);
                        vtopyes = getc(tty);
                        efclose(tty);
			tty = efopen("/dev/tty","r");
                        fprintf(stderr,
  "Replace Bottom velocities in horizon %d with those in VELO? (y/n) ->",ih+1);
                        vbotyes = getc(tty);
                        efclose(tty);

                        if(vtopyes=='n' && vbotyes=='n') continue;

			n2 = ih*maxpks;
			n1 = ih*ncdp_velo;
			nvs = nvh[ih];
			if(nvs==0) {
				fprintf(stderr, 
				"warning: no velan at horizon %d\n",ih+1); 
				fprintf(stderr, 
				"warning: editing of ifile needed !!!\n"); 
				for(ip=0;ip<npicks[ih];ip++) {
					if(vtopyes=='y') veltops[ip+n2] = 0.;
                                	if(vbotyes=='y') velbots[ip+n2] = 0.;
                                	velavgs[ip+n2] = 0.;
                                	dvdzs[ip+n2] = 0.;
				}
			} else if (nvs==1) {
				for(ip=0;ip<npicks[ih];ip++) {
					if(vtopyes=='y') veltops[ip+n2]=vth[n1];
                                	if(vbotyes=='y') velbots[ip+n2]=vbh[n1];
                                	velavgs[ip+n2] = vah[n1];
                                	dvdzs[ip+n2] = dvdzh[n1];
				}
			} else {
				/* not a good idea to use spline interpolation
					here */
				/*
				if(nvs>2) {
					z21 = 0.;
                                        z2n = 0.;
                                        spline_(xvh+n1,vth+n1,
						&nvs,&z21,&z2n,vt2,vb2);
                                        spline_(xvh+n1,vbh+n1,
						&nvs,&z21,&z2n,vb2);
                                        spline_(xvh+n1,vah+n1,
						&nvs,&z21,&z2n,va2);
                                        spline_(xvh+n1,dvdzh+n1,
						&nvs,&z21,&z2n,dvdz2);
				} 
				*/
				for(ip=0;ip<npicks[ih];ip++) {
					x = xpicks[ip+ih*maxpks];
					bisear_(&nvs,&one,xvh+n1,&x,&i2);
					if(x<=xvh[n1]) {
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1];
						if(vbotyes=='y')
                                		velbots[ip+n2] = vbh[n1];
                                		velavgs[ip+n2] = vah[n1];
                                		dvdzs[ip+n2] = dvdzh[n1];
					} else if(x>=xvh[n1+nvs-1]) {
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1+nvs-1];
						if(vbotyes=='y')
                                		velbots[ip+n2] = vbh[n1+nvs-1];
                                		velavgs[ip+n2] = vah[n1+nvs-1];
                                		dvdzs[ip+n2] = dvdzh[n1+nvs-1];
					} else { 
					/*} else if(nvs==2) {  */
						tmp = (x-xvh[n1+i2-1])/
						      (xvh[n1+i2]-xvh[n1+i2-1]);
						if(vtopyes=='y') 
						veltops[ip+n2] = vth[n1+i2-1] +
						  tmp*(vth[n1+i2]-vth[n1+i2-1]);
						if(vbotyes=='y')
						velbots[ip+n2] = vbh[n1+i2-1] +
						  tmp*(vbh[n1+i2]-vbh[n1+i2-1]);
                                		velavgs[ip+n2] = vah[n1+i2-1] +
						  tmp*(vah[n1+i2]-vah[n1+i2-1]);
                                		dvdzs[ip+n2] = dvdzh[n1+i2-1] +
						  tmp*
						  (dvdzh[n1+i2]-dvdzh[n1+i2-1]);
					/*
					} else {
						if(vtopyes=='y') 
                                        	splint_(xvh+n1,vth+n1,vt2,&nvs,
							&x,&veltops[ip+n2]);
						if(vbotyes=='y')
                                        	splint_(xvh+n1,vbh+n1,vb2,&nvs,
							&x,&velbots[ip+n2]);
                                        	splint_(xvh+n1,vah+n1,va2,&nvs,
							&x,&velavgs[ip+n2]);
                                        	splint_(xvh+n1,dvdzh+n1,dvdz2,
							&nvs,&x,&dvdzs[ip+n2]);
					*/
					}
				}
			}
			/* add velan locations to horizon picks */
			j2 = nvs + npicks[ih];
			for(i2=0;i2<npicks[ih];i2++) { 
				xnew[i2] = xpicks[i2+ih*maxpks];
				znew[i2] = zpicks[i2+ih*maxpks];
				vtnew[i2] = veltops[i2+ih*maxpks];
				vbnew[i2] = velbots[i2+ih*maxpks];
				vanew[i2] = velavgs[i2+ih*maxpks];
				gnew[i2] = dvdzs[i2+ih*maxpks];
			}
			for(i2=0;i2<nvs;i2++) { 
				xnew[i2+npicks[ih]] = xvh[n1+i2];
				znew[i2+npicks[ih]] = zvh[n1+i2];
				vtnew[i2+npicks[ih]] = vth[n1+i2];
				vbnew[i2+npicks[ih]] = vbh[n1+i2];
				vanew[i2+npicks[ih]] = vah[n1+i2];
				gnew[i2+npicks[ih]] = dvdzh[n1+i2];
			}

			if(vtopyes=='n') {
				i2 = npicks[ih];
				lin1d_(xpicks+ih*maxpks,veltops+ih*maxpks,&i2,
					xnew,vtnew,&j2,indx);
			}
			if(vbotyes=='n') {
				i2 = npicks[ih];
				lin1d_(xpicks+ih*maxpks,velbots+ih*maxpks,&i2,
					xnew,vbnew,&j2,indx);
			}
			for(i2=0;i2<j2;i2++) indx[i2] =  i2;
			qkisort(j2,xnew,indx);
			xpre = xnew[indx[0]] - 999.;
			j1 = 0;
			for(i2=0;i2<j2;i2++) {
				inow = indx[i2];
			   	xnow = xnew[inow];
				i1 = j1 + ih*maxpks;
			   	if(abs(xnow-xpre)>=1.) {
					xpicks[i1] = xnow;
					zpicks[i1] = znew[inow];
					veltops[i1] = vtnew[inow];
					velbots[i1] = vbnew[inow];
					velavgs[i1] = vanew[inow];
					dvdzs[i1] = gnew[inow];
					xpre = xnow;
					j1 = j1 + 1;
				}
			}
			if(j1>maxpks) 
			err("too many picks!! npick=%d at horizon %d",j1,ih+1); 
			npicks[ih] = j1;
		}

		free(indx);
		free(xnew);
		free(znew);
		free(vtnew);
		free(vbnew);
		free(vanew);
		free(gnew);

		free(zvs);
		free(ivs);
		free(isort);
		free(zsort);
		free(nvh);
		free(xvh);
		free(zvh);
		free(vbh);
		free(vth);
		free(vah);
		free(dvdzh);
		free(zgrid);
		free(vagrid);
		free(dvdzgrid);
		free(g1);
		free(g2);

		free(z2);
		/*
		free(vt2);
		free(vb2);
		free(va2);
		free(dvdz2);
		*/
	}
}

/* interface velocity edit */ 
void ivedit(float *xpick, float *vpick, char *hcolor, int nvi, char *title,
		float xmin, float dcdp, int cdpxmin, int *isave) {

	Display *dpy1;
	Window win1;
	XEvent event;
	KeySym keysym;

	GC gci, gci2;
        XGCValues *values;
        XColor scolor,ecolor;
        XWindowAttributes wa;
        Colormap cmap;
	int scr;
	XComposeStatus keystat;
	char keybuf[256];

	unsigned long black, white;

	int xbox, ybox, wbox, hbox, showloc=0;
	int xx1, yy1, xx2, yy2, i, n1tic, n2tic;
	float d1num, d2num, f1num, f2num, x1begb, x2begb, x1endb, x2endb;
	float vmin, vmax;
	int x, y, width, height, style ;
	int grid1=SOLID, grid2=SOLID,ip;
	float x1,x2,dis,dismin;
	float *vps;
	float cdpbegb, cdpendb;


	char *label1="cdp",*label2="velocity",
		*labelfont="Erg14",*titlefont="Rom22",
		*labelcolor="blue",*titlecolor="red",
		*gridcolor="SteelBlue";

	int shown=0;
	*isave = 0;

	/* allocate space for vpick updates */
	vps = (float *) malloc(nvi *sizeof(float));
	for(i=0;i<nvi;i++) vps[i] = vpick[i];

	/* connect to X server */
	if ((dpy1=XOpenDisplay(NULL))==NULL)
		err("Cannot connect to display %s!\n",XDisplayName(NULL));

        /* get screen */
        scr = DefaultScreen(dpy1);
	black = BlackPixel(dpy1,scr);
	white = WhitePixel(dpy1,scr);

	/* create window */
	xbox=50;
	ybox=50;
	wbox=1000;
	hbox=500;

        win1 = xNewWindow(dpy1,xbox,ybox,wbox,hbox,black,white,"ivedit");


        /* determine window's current colormap */
        XGetWindowAttributes(dpy1,win1,&wa);
        cmap = wa.colormap;

	/* create graphics contexts */
       	gci = XCreateGC(dpy1,win1,0,NULL);
       	gci2 = XCreateGC(dpy1,win1,0,NULL);

       	if (XAllocNamedColor(dpy1,cmap,hcolor,&scolor,&ecolor)) {
       		XSetForeground(dpy1,gci,ecolor.pixel);
       		XSetForeground(dpy1,gci2,ecolor.pixel);
        } else {
        	XSetForeground(dpy1,gci,1L);
        	XSetForeground(dpy1,gci2,1L);
	}


        XSetLineAttributes(dpy1,gci,1,LineSolid,CapButt,JoinMiter);
        XSetLineAttributes(dpy1,gci2,1,LineOnOffDash,CapButt,JoinMiter);

	/* set normal event mask */
	XSelectInput(dpy1,win1,
		StructureNotifyMask |
		ExposureMask |
		KeyPressMask |
		PointerMotionMask |
		ButtonPressMask |
		ButtonReleaseMask |
		Button1MotionMask |
		Button2MotionMask);

	/* map window */
        XMapWindow(dpy1,win1);

	/* determine good size for axes box */
	style=NORMAL;
        xSizeAxesBox(dpy1,win1,
                labelfont,titlefont,style,
                &x,&y,&width,&height);

	/* clear the window */
	XClearWindow(dpy1,win1);

	/* draw axes */
	x1begb = xpick[0];
	x1endb = xpick[nvi-1];
	n1tic = 1;
	d1num = 0.;
	f1num = 0.;

	vmin = vpick[0];
	vmax = vpick[0];
	for(i=0;i<nvi;i++) {
		if(vmin>vpick[i]) vmin = vpick[i];
		if(vmax<vpick[i]) vmax = vpick[i];
	}
	x2begb = vmin - 500.;
	x2endb = vmax + 500.;
	n2tic = 1;
	d2num=0.;
	f2num=0.;

	cdpbegb = (x1begb - xmin)/dcdp + cdpxmin; 
	cdpendb = (x1endb - xmin)/dcdp + cdpxmin; 

	/*
	xDrawAxesBox(dpy1,win1,
			x,y,width,height,
			cdpbegb,cdpendb,0.0,0.0,
			d1num,f1num,n1tic,grid1,label1,
			x2begb,x2endb,0.0,0.0,
			d2num,f2num,n2tic,grid2,label2,
			labelfont,title,titlefont,
			labelcolor,titlecolor,gridcolor,
			style);
	for(i=1;i<nvi;i++) {
        	xx1=(xpick[i-1]-x1begb)/(x1endb-x1begb)*width+x;
                yy1=(vpick[i-1]-x2endb)/(x2begb-x2endb)*height+y;
                xx2=(xpick[i]-x1begb)/(x1endb-x1begb)*width+x;
                yy2=(vpick[i]-x2endb)/(x2begb-x2endb)*height+y;
                XDrawLine(dpy1,win1,gci2,xx1,yy1,xx2,yy2);
	}

	XFlush(dpy1);
	*/
	/* save foreground to background */
	/*
        fg2bg(dpy1,win1);
	*/

	while(True) {
		XNextEvent(dpy1,&event);
		if (event.type==KeyPress) {
			XLookupString(&event,keybuf,0,&keysym,&keystat);
			if (keysym==XK_Q) {
				/* free resources before returning */
        			XFreeGC(dpy1,gci);
        			XFreeGC(dpy1,gci2);
				free(vps);
				/* close connection to X server */
				XCloseDisplay(dpy1);
                                return;
			} else if (keysym==XK_S) {
				/* save updates */
				*isave = 1;
        			for(i=0;i<nvi;i++) vpick[i] = vps[i]; 
                        } else {
                                continue;
                        }
		} else if (event.type==ButtonPress) {
			if (event.xbutton.button==Button3) {
				x1 = x1begb+(x1endb-x1begb)*
					(event.xmotion.x-x)/width;
                		x2 = x2endb+(x2begb-x2endb)*
					(event.xmotion.y-y)/height;

				dismin = (x1-xpick[0])*(x1-xpick[0]); 
				ip = 0;
				for(i=1;i<nvi;i++) {
					dis = (x1-xpick[i])*(x1-xpick[i]);
					if(dis<dismin) {
						dismin = dis;
						ip = i;
					}
				}
				vps[ip] = x2;
			
				XClearWindow(dpy1, win1);
	xDrawAxesBox(dpy1,win1,
			x,y,width,height,
			cdpbegb,cdpendb,0.0,0.0,
			d1num,f1num,n1tic,grid1,label1,
			x2begb,x2endb,0.0,0.0,
			d2num,f2num,n2tic,grid2,label2,
			labelfont,title,titlefont,
			labelcolor,titlecolor,gridcolor,
			style);
	for(i=1;i<nvi;i++) {
        	xx1=(xpick[i-1]-x1begb)/(x1endb-x1begb)*width+x;
                yy1=(vpick[i-1]-x2endb)/(x2begb-x2endb)*height+y;
                xx2=(xpick[i]-x1begb)/(x1endb-x1begb)*width+x;
                yy2=(vpick[i]-x2endb)/(x2begb-x2endb)*height+y;
                XDrawLine(dpy1,win1,gci2,xx1,yy1,xx2,yy2);
	}

				for(i=1;i<nvi;i++) {
        				xx1=(xpick[i-1]-x1begb)/
						(x1endb-x1begb)*width+x;
                			yy1=(vps[i-1]-x2endb)/
						(x2begb-x2endb)*height+y;
                			xx2=(xpick[i]-x1begb)/
						(x1endb-x1begb)*width+x;
                			yy2=(vps[i]-x2endb)/
						(x2begb-x2endb)*height+y;
                			XDrawLine(dpy1,win1,gci,
						xx1,yy1,xx2,yy2);
				}
			} else if (event.xbutton.button==Button2) {
                                showloc = 1;
                                xMouseLoc(dpy1,win1,event,style,showloc,
                                          x,y,width,height,x1begb,x1endb,
                                          x2begb,x2endb);
			}
		} else if(shown==0) {
			shown = 1;
				XClearWindow(dpy1, win1);
	xDrawAxesBox(dpy1,win1,
			x,y,width,height,
			cdpbegb,cdpendb,0.0,0.0,
			d1num,f1num,n1tic,grid1,label1,
			x2begb,x2endb,0.0,0.0,
			d2num,f2num,n2tic,grid2,label2,
			labelfont,title,titlefont,
			labelcolor,titlecolor,gridcolor,
			style);
	for(i=1;i<nvi;i++) {
        	xx1=(xpick[i-1]-x1begb)/(x1endb-x1begb)*width+x;
                yy1=(vpick[i-1]-x2endb)/(x2begb-x2endb)*height+y;
                xx2=(xpick[i]-x1begb)/(x1endb-x1begb)*width+x;
                yy2=(vpick[i]-x2endb)/(x2begb-x2endb)*height+y;
                XDrawLine(dpy1,win1,gci2,xx1,yy1,xx2,yy2);
	}

				for(i=1;i<nvi;i++) {
        				xx1=(xpick[i-1]-x1begb)/
						(x1endb-x1begb)*width+x;
                			yy1=(vps[i-1]-x2endb)/
						(x2begb-x2endb)*height+y;
                			xx2=(xpick[i]-x1begb)/
						(x1endb-x1begb)*width+x;
                			yy2=(vps[i]-x2endb)/
						(x2begb-x2endb)*height+y;
                			XDrawLine(dpy1,win1,gci,
						xx1,yy1,xx2,yy2);
				}

		}

	}

}

/* interpolate vtop, vbot, vavg and dvdz to xnew positions */

void interp(float *xold, float *xnew, int nold, int nnew, float *vtop, 
	float *vbot, float *vavg, float *dvdz) {

	float *tmp;
	int *indx, i;
	
	tmp = (float *) malloc(nold*sizeof(float));
	indx = (int *) malloc(nnew*sizeof(int));

	/* find index */
	bisear_(&nold,&nnew,xold,xnew,indx);
	
	/* linear interpolation */
	for(i=0;i<nold;i++) {
		tmp[i] = vtop[i];
		vtop[i] = 0.;
	}
	linin_(&nold,&nnew,xold,xnew,indx,tmp,vtop);
	for(i=0;i<nold;i++) {
		tmp[i] = vbot[i];
		vbot[i] = 0.;
	}
	linin_(&nold,&nnew,xold,xnew,indx,tmp,vbot);
	for(i=0;i<nold;i++) {
		tmp[i] = vavg[i];
		vavg[i] = 0.;
	}
	linin_(&nold,&nnew,xold,xnew,indx,tmp,vavg);
	for(i=0;i<nold;i++) {
		tmp[i] = dvdz[i];
		dvdz[i] = 0.;
	}
	linin_(&nold,&nnew,xold,xnew,indx,tmp,dvdz);

	free(tmp);
	free(indx);
}
