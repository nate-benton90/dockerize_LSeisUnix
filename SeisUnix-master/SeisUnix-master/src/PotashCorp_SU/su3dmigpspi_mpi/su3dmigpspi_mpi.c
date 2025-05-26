/* su3dmigpspi */
/* Phase shift migration with interpolation */

#include "suhdr.h"
#include <mpi.h>
#define I               cmplx(0.0, 1.0)
#define PIBY2           0.5 * PI
#define TWOPI           2.0 * PI
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */

/*********************** self documentation *****************************/
char *sdoc[] = {"  SU program to perform 3D PSPI migration         ",
"  su3dmigpspi_mpi fs=stack fv=velocity volume                                 ",
"                                                                           ",
"									    ",
"  Required parameters:							    ",
"  The data volume has to be square and sorted.				    ",
"  The number of traces in second direction is nx in the third direction is ny",
"  To get the data sorted in the right way susort <inf >outf f3 f2	    ",
"  where f2 is the header word containing the numbering in the second direction  ",
"  and  f3 is the header word containing the numbering in the third  direction   ",
"  The velocity voulme has to be sorted in the same order and it also has to",
"  be the same dimensions as the data volume. 		                    ",
"  Depthwise the velocity volume could be sampled with a different rate as the data ",
"  volume as these are going to be interpolated				    ",
"                                                                           ",
"  n2=			number of traces in second dimension		    ",
"  n3=			number of traces in third  dimension		    ",
"  d2=			data spatial sampling in dir. 2			    ",
"  d3=			data spatial sampling in dir. 3			    ",
"                                                                           ",
"  dmp=5		damping zone in traces                              ",
"                                                                           ",
"  dz=1.0		depth sampling intervall                            ",
"  nz=			number of z values at output                        ",
"                                                                           ",
"  L=15			Maximum number of velocity bins in each depth layer ",
"  bdv=12.5		Minimum bin width                                   ",
"                                                                           ",
"  fftpad=50.0          Padding traces before fft in %                      ",
"                                                                           ",
"  tmpd=/tmp		Location of temporary velocity file                 ",
"                                                                           ",
"                                                                           ",
"  Survey sinking                                                           ",
"  key=none		If defined the migration starts from depth values   ",
"                       specified here; above these depth the wavefiled     ",
"                       is just downward continued without hyperbola        ",
"                       summation                                           ",
"                       (The zero velocity layer... Geophysics 1992,57,11)  ",
"                                                                           ",
NULL};
   
segy tr;			  /* SEGY data trace */
segy trv;			  /* SEGY velocity trace */

int verbose;

void absorb(int bnd, complex *m, int n3, int n2);
float *get_mv(float *vslc,int n,int L,float bdv,float *v,int *nv);
void patch_corner(void ***a,int n1,int n2,int bnd,int d1,int d2,int n,size_t asize);
void patch_segment(void ***a,int n11,int n12,int n21,int n22,int bnd,int d1,int d2,
		   int n1max, int n1min,int n2max,int n2min,int n,size_t asize);
void patch_corner_2d(void **a,int n1,int n2,int bnd,int d1,int d2,int n,size_t asize);
void patch_segment_2d(void **a,int n11,int n12,int n21,int n22,int bnd,int d1,int d2,
		   int n1min, int n1max,int n2min,int n2max,int n,size_t asize);
void comp_data_part(int ifnw,int nproc,int nwp[],int *nwpp);

int mastercode (int argc, char *argv[]);
void slavecode (void);

int
main (int argc, char *argv[])
{
	int rank;
  	MPI_Init (&argc, &argv);
  	MPI_Comm_rank (MPI_COMM_WORLD, &rank);

  	if (rank == 0)
      		mastercode (argc,argv);
  	else
    		slavecode ();
  
  fflush(stdout);
  MPI_Finalize ();
  return 0;
}

int mastercode( int argc, char *argv[] )
{
        int ntr=0;                /* number of traces                     */
        int ntrv=0;               /* number of traces                     */
	int ns=0;
	int nsv=0;
	float dt;
	float dzv;
	
	cwp_String fs;
	cwp_String fv;
	FILE *fps;
	FILE *fpv;
	FILE *headerfp;
	FILE *tmpvelfp;		/* temporary file for storing velocity slices */
	FILE *tmpmigfp;		/* temporary file for storing migrated data*/
	cwp_String tmpd;	/* directory of temporay velocity file */
	char tmpvfn[256];	/* temporary velocity slices file filename */
	char tmpmfn[256];	/* temporary migrated data file filename */
		
	float *velfi;		/* average velocity function interpolated to ns values*/
	float *velf;		/* velocity function */
	float *vz;
	float *mz;
	float dz;		/* depth sampling intervall */
	int nz;			/* number of depth samples */
	
	float d2;		/* spatial smapling of the stack */
	float d3;		/* spatial smapling of the stack */
	int n2;
	int n3;
	int n2d;
	int n3d;
	
	float vmin=9999;	/* Minimum migration velocity */
	float vmax=-9999;	/* Maximum migration velocity */
	float facs;		/* factor to scale velocity */
	float face;		/* factor to scale velocity */
	float dfac;

	complex ***dataf;	/* data matrix of the migration volume in frequency domain*/
	float **datam;		/* data matrix of the migration volume */
	float ***vel;		/* velocity matrix */
	complex ***wd;		/* data volume in frequency domain */
	
	float fmax;		/* maximum frequency to migrate */
	int ifnw;		/* integer maximum frequency */
	

	/* Fourier transform variables */
	int ntfft;
	int nw;
	float dw;
	float fw;
	
	int nd2fft;
	int nk2;
	float dk2;
	float fk2;
	
	int nd3fft;
	int nk3;
	float dk3;
	float fk3;
	float sfft;
	float tsfft;
	int dmp;
	float fftpad;		


	/* velocity interpolation */
	int L;
	float bdv;
	
	/* MPI variables */
	int NPROC;
	int mytid;
	MPI_Status status;
	
	/* survey sinking */
	cwp_String key;         /* header key word from segy.h          */
        cwp_String type="none";        /* ... its type                         */
	int indx=0;
	int is_Sr=0;
	int imax_Sr=0;
        Value val;
 	
	int **Sr_idepth;
	
	
	
	initargs(argc, argv);
   	requestdoc(1);
	
        MUSTGETPARSTRING("fs",&fs);
        MUSTGETPARSTRING("fv",&fv);
        MUSTGETPARINT("n2",&n2);
        MUSTGETPARINT("n3",&n3);
        MUSTGETPARFLOAT("d2",&d2);
        MUSTGETPARFLOAT("d3",&d3);
        MUSTGETPARINT("nz",&nz);
	
	if (!getparfloat("dz", &dz))	dz=1.0;
	if (!getparfloat("facs", &facs))	facs=1.0;
	if (!getparfloat("face", &face))	face=facs;
	dfac=(face-facs)/(nz-1);
	if (!getparint("dmp", &dmp))	dmp=5;
	
	if (!getparint("L", &L))	L=15;
	if (!getparfloat("bdv", &bdv))	bdv=12.5;
	
	if (!getparfloat("fmax", &fmax))	fmax=-1.0;
	
	if (!getparfloat("fftpad", &fftpad))	fftpad=50.0;
	fftpad/=100.0;
        
	if(getparstring("key", &key)) {
	        type = hdtype(key);
                indx = getindex(key);
		is_Sr=1;
	} else {
	 	key="none";
	}               
	
	
	if(getparstring("tmpd", &tmpd)) {
		strcpy(tmpvfn,tmpd);
		strcpy(tmpmfn,tmpd);
	} else {
		strcpy(tmpvfn,"/tmp");
		strcpy(tmpmfn,"/tmp");
	}
	strcat(tmpvfn,"/vlsXXXXXX");
	strcat(tmpmfn,"/mdtXXXXXX");
	
	if (!getparint("verbose", &verbose))	verbose=0;

	
	/* MPI get number of proceses */
 	MPI_Comm_size (MPI_COMM_WORLD, &NPROC);
	
	/* open datafile */
        fps = efopen(fs,"r");
	fpv = efopen(fv,"r");
	
	/* Open tmpfile for headers */
	headerfp = etmpfile();

	/* get information from the first data trace */
	ntr = fgettra(fps,&tr,0);
	if(n2*n3!=ntr) err(" Number of traces in file %d not equal to n2*n3 %d \n",
			     ntr,n2*n3);
	ns=tr.ns;
	if (!getparfloat("dt", &dt))	dt = ((float) tr.dt)/1000000.0;
	if (!dt) {
		dt = .002;
		warn("dt not set, assumed to be .002");
	}

	/* get information from the first velocity trace */
	ntrv = fgettra(fpv,&trv,0);
	if(ntrv!=ntr) err(" Number of traces in velocity file %d differ from %d \n",
			     ntrv,ntr);
	nsv=trv.ns;
	if (!getparfloat("dzv", &dzv))	dzv = ((float) trv.dt);
	if (!dzv) {
		dzv = 200;
		warn("dzv not set, assumed to be 200 m for velocity");
	}
	
        ntr=n2*n3;
	
	if(verbose>0) fprintf(stderr," nt n2 n3 %d %d %d\n",ns,n2,n3);
	
	/* MPI */
	
	
	
	/* Set up fourier transform constans */
	/* Time wise; Direction 1 */
	ntfft = npfar(ns);
	nw = ntfft/2+1;
	dw = 2.0*PI/(ntfft*dt);
	fw = 0;
	tsfft = 1.0/ntfft;
	
	if(fmax>0.0) {
		ifnw=MIN(NINT(fmax*2.0*dt*nw),nw);
		if(verbose>0) fprintf(stderr,"Maximum frequency to migrate %f %d\n",fmax,ifnw);
	} else {
		ifnw = nw;
	}
	
	/* Sizes with absorbing layer */
	n2d=n2+2*dmp;
	n3d=n3+2*dmp;
	
	{ int ntmp;
	
		/* Direction 2 */
		ntmp=NINT(n2d*(1.0+fftpad));
		if(verbose>0) fprintf(stderr," nk2padd %d",ntmp);
		nd2fft = npfao(ntmp,ntmp*LOOKFAC);
		nk2 = nd2fft;
		dk2 = 2.0*PI/(nd2fft*d2);
		fk2 = -PI/d2;
	
		/* Direction 3 */
		ntmp=NINT(n3d*(1.0+fftpad));
		if(verbose>0) fprintf(stderr," nk3padd %d\n",ntmp);
		nd3fft = npfao(ntmp,ntmp*LOOKFAC);
		nk3 = nd3fft;
		dk3 = 2.0*PI/(nd3fft*d3);
		fk3 = -PI/d3;
	}
	
	sfft = 1.0/(nk3*nk2);
	if(verbose>0) fprintf(stderr," ifnw nk2 nk3 %d %d %d\n",ifnw,nk2,nk3);
	

	/* allocate arrays */
	dataf = ealloc3complex(ifnw,n3d,n2d);
	vel =  ealloc3float(nz,n3d,n2d);
	Sr_idepth = ealloc2int(n3d,n2d);
	
	velf = ealloc1float(nsv); 
	velfi = ealloc1float(nz);
	vz = ealloc1float(nsv);
	mz = ealloc1float(nz);
	
	/* Times to do interpolation of velocity from sparse sampling */
	/* to fine sampling of the data */
	{ register int it;
		for(it=0;it<nsv;it++) vz[it]=it*dzv;
		for(it=0;it<nz;it++)  mz[it]=it*dz;
	}
	
	/* Read traces into data */
        /* Store headers in tmpfile */
	erewind(fps);
	erewind(fpv);
		
	{ register int i2,i3,iz;
	  float **vslc;
	  complex *ct;

	   	ct   = ealloc1complex(nw);
		
		for(i3=0;i3<n3;i3++) {
			for(i2=0;i2<n2;i2++) {
				fgettr(fps,&tr);
				fgettr(fpv,&trv);
				efwrite(&tr, 1, HDRBYTES, headerfp);
		 		
				/* Save survey datum from headers to array */
				if(is_Sr) {
				
				           gethval(&tr, indx, &val);
					   Sr_idepth[i2+dmp][i3+dmp] = NINT(vtof(type,val)/dz);
					   if(Sr_idepth[i2+dmp][i3+dmp] > imax_Sr)
					   	imax_Sr=Sr_idepth[i2+dmp][i3+dmp]; 				
				}
				
				/*bmwrite(data,1,i2+dmp,i3+dmp,1,&tr.data[0]); */
				
				/* Read data and FFT it; store it in dataf */
				memset( (void *) &tr.data[0],0,(ntfft-ns)*FSIZE);
				
				pfarc(1,ntfft,&tr.data[0],ct);

				memcpy((void *) dataf[i2+dmp][i3+dmp],(const void *) &ct[0],sizeof(complex)*ifnw);
			
				/* Apply scale factor to velocity */
				/* Migration velocity is half the intervall velocity 
				   => apply 0.5 too */
				{ register int it;
					for(it=0;it<nsv;it++) {
						trv.data[it] *=(facs+dfac*it)*0.5;
						if(vmin>trv.data[it]) vmin=trv.data[it];
						if(vmax<trv.data[it]) vmax=trv.data[it];
					}
				}
			
				/* linear interpolation of velocities from nsv to nz values */  
				intlin(nsv,vz,trv.data,trv.data[0],trv.data[nsv-1],nz,mz,velfi);
		 		/*bmwrite(vel,1,i2+dmp,i3+dmp,1,&velfi[0]); */
				memcpy((void *) vel[i2+dmp][i3+dmp],(const void *) &velfi[0],sizeof(float)*nz);
			}
		}

		/* Fill the absorbing layer around the data and velocity block and Sr */
	   	
		/* Do the corners first */
		patch_corner((void ***)vel,dmp,dmp,dmp,-1,-1,nz,sizeof(float));
		patch_corner((void ***)vel,n2d-dmp-1,dmp,dmp,1,-1,nz,sizeof(float));
		patch_corner((void ***)vel,n2d-dmp-1,n3d-dmp-1,dmp,1,1,nz,sizeof(float));
		patch_corner((void ***)vel,dmp,n3d-dmp-1,dmp,-1,1,nz,sizeof(float));
		patch_segment((void ***)vel,dmp,dmp,dmp,n3d-dmp-1,dmp,-1,0,0,n2d,0,n3d,nz,sizeof(float));
		patch_segment((void ***)vel,dmp,dmp,n2d-dmp-1,dmp,dmp,0,-1,0,n2d,0,n3d,nz,sizeof(float));
		patch_segment((void ***)vel,n2d-dmp-1,dmp,n2d-dmp-1,n3d-dmp-1,dmp,1,0,0,n2d,0,n3d,nz,sizeof(float));
		patch_segment((void ***)vel,dmp,n3d-dmp-1,n2d-dmp-1,n3d-dmp-1,dmp,0,1,0,n2d,0,n3d,nz,sizeof(float));
		
		patch_corner((void ***)dataf,dmp,dmp,dmp,-1,-1,ifnw,sizeof(complex));
		patch_corner((void ***)dataf,n2d-dmp-1,dmp,dmp,1,-1,ifnw,sizeof(complex));
		patch_corner((void ***)dataf,n2d-dmp-1,n3d-dmp-1,dmp,1,1,ifnw,sizeof(complex));
		patch_corner((void ***)dataf,dmp,n3d-dmp-1,dmp,-1,1,ifnw,sizeof(complex));
		patch_segment((void ***)dataf,dmp,dmp,dmp,n3d-dmp-1,dmp,-1,0,0,n2d,0,n3d,ifnw,sizeof(complex));
		patch_segment((void ***)dataf,dmp,dmp,n2d-dmp-1,dmp,dmp,0,-1,0,n2d,0,n3d,ifnw,sizeof(complex));
		patch_segment((void ***)dataf,n2d-dmp-1,dmp,n2d-dmp-1,n3d-dmp-1,dmp,1,0,0,n2d,0,n3d,ifnw,sizeof(complex));
		patch_segment((void ***)dataf,dmp,n3d-dmp-1,n2d-dmp-1,n3d-dmp-1,dmp,0,1,0,n2d,0,n3d,ifnw,sizeof(complex));
		
		patch_corner_2d((void **)Sr_idepth,dmp,dmp,dmp,-1,-1,1,sizeof(int));
		patch_corner_2d((void **)Sr_idepth,n2d-dmp-1,dmp,dmp,1,-1,1,sizeof(int));
		patch_corner_2d((void **)Sr_idepth,n2d-dmp-1,n3d-dmp-1,dmp,1,1,1,sizeof(int));
		patch_corner_2d((void **)Sr_idepth,dmp,n3d-dmp-1,dmp,-1,1,1,sizeof(int));
		patch_segment_2d((void **)Sr_idepth,dmp,dmp,dmp,n3d-dmp-1,dmp,-1,0,0,n2d,0,n3d,1,sizeof(int));
		patch_segment_2d((void **)Sr_idepth,dmp,dmp,n2d-dmp-1,dmp,dmp,0,-1,0,n2d,0,n3d,1,sizeof(int));
		patch_segment_2d((void **)Sr_idepth,n2d-dmp-1,dmp,n2d-dmp-1,n3d-dmp-1,dmp,1,0,0,n2d,0,n3d,1,sizeof(int));
		patch_segment_2d((void **)Sr_idepth,dmp,n3d-dmp-1,n2d-dmp-1,n3d-dmp-1,dmp,0,1,0,n2d,0,n3d,1,sizeof(int));
			
		/* Create tmpfile for storing velocity slices and migrated data*/
		if(verbose>0)
			fprintf(stderr," Creating temporary files \n");
		if(verbose>1) {
			fprintf(stderr," Migration results %s \n",tmpmfn);
			fprintf(stderr," Velocity %s \n",tmpvfn);
		}
			
		/* Velocity slices */
		if (NULL == ( tmpvelfp = fdopen(mkstemp(tmpvfn),"w+"))) {
			err(" Cannot open temporary file %s\n", tmpvfn);
		} else {
			unlink(tmpvfn);
			if(verbose>1)
				fprintf(stderr," Velocity %s \n",tmpvfn);
		}
		
		/* Migrated results */
		if (NULL == ( tmpmigfp = fdopen(mkstemp(tmpmfn),"w+"))) {
			err(" Cannot open temporary file %s\n", tmpmfn);
		} else {
			unlink(tmpmfn);
			if(verbose>1)
				fprintf(stderr," Migration results %s \n",tmpmfn);
		}
		
		/* Create velocity depth slices and store the velocities in that format */	
		fprintf(stderr," Creating Velocity depth slices \n");

		vslc = ealloc2float(n3d,n2d);	
		for(iz=0;iz<nz;iz++) { 
			for(i3=0;i3<n3d;i3++) {
				for(i2=0;i2<n2d;i2++) {
					/*bmread(vel,1,i2,i3,1,&velfi[0]); */
					memcpy((void *) &velfi[0],(const void *) vel[i2][i3],sizeof(float)*nz);
					vslc[i2][i3] = velfi[iz];
				}
			}
			efwrite(&vslc[0][0],sizeof(float),n2d*n3d,tmpvelfp);
		}
		efseeko(tmpvelfp,0,SEEK_SET);
		free2float(vslc);
		free3float(vel);
		free1complex(ct);
	}
	if(verbose) {
		fprintf(stderr," Maximum Velocity %f\n",vmax);
		fprintf(stderr," Minimum Velocity %f\n",vmin);
	}
	free1float(vz);
	free1float(mz);
	
	erewind(headerfp);

	/* START MIGRATION */

	
	{ complex **wslc;
	  int i2,i3,iw;

		wslc = ealloc2complex(n3d,n2d);
		memset( (void *) &wslc[0][0],0,n2d*n3d*sizeof(complex));
		
		fprintf(stderr," Creating frequency slices from transformed data \n");
		
		/* array for wd transformed data */
		/* allocated so that 1 element is an w slice with n2*n3 dimensions */
		/* therefore it is a 1 dimensional array(nw) of 2D matrices[n2][n3] */ 
		/* wd = ealloc1(ifnw,n2d*n3d*sizeof(complex));*/ /* This holds the frequency domain  wavefield */ 
		wd = ealloc3complex(n3d,n2d,ifnw); /* This holds the frequency domain  wavefield */
		
		/* Shuffle the data to freqeuncy slice format */
		/* And scale after FFT */
		free2complex(wslc);	
		
		for(iw=0;iw<ifnw;iw++)
			for(i2=0;i2<n2d;i2++) 
				for(i3=0;i3<n3d;i3++)
					wd[iw][i2][i3] = crmul(dataf[i2][i3][iw],tsfft);
	}

	free3complex(dataf);
	fprintf(stderr," Done\n");
	
		
	/* allocate space for the migrated depth section */
	/*data = bmalloc(sizeof(float),nz,ntr); */
	fprintf(stderr," ......\n");
	
	/*  */
	{ float **swslc_r;
	  float z;
	  int iz,i2,i3,iw;

	  /* Velocity related */
	  float **vslc;
	  float *v=NULL;
	  int nvel;
	 
	  /* MPI related */
	  int *nwp;
	  int nwp_max,flag;
	  int ipr;
	  float *re,*im;
	  char *buffer;
	  int bsize;
	  int position = 0;
	
		nwp = ealloc1int(NPROC);
		/* Optimal load for slaves */
	
		comp_data_part(ifnw,NPROC,nwp,&nwp_max);
		if(verbose>0){
			{ int irank;
				for(irank=0;irank<NPROC;irank++)
					if(verbose) fprintf(stderr,"Node %d load %d\n",irank,nwp[irank]);	
			}
		}
		if(verbose>1) fprintf(stderr," %d %d %d\n",ifnw,nwp_max,NPROC);
		
		/* Data to be sent */
		/* n2d,n3d,nk2,nk3,nwp_max,L - integers 7 */
		/* dk2,dk3,dz,dw,sfft - floats  5 */
		/* Sr_idepth - integer array n3d*n2d */
		
		
		bsize = 10*ISIZE+10*FSIZE+n3d*n2d*ISIZE;
		/* send buffer size to tasks */
		{ int irank;
			for(irank=1;irank<NPROC;irank++) {
				if(verbose>2) fprintf(stderr," Sending bufferr size to %d\n",irank);
				MPI_Send(&bsize, 1, MPI_INT,irank,1,MPI_COMM_WORLD);
			}
		}
		buffer = ealloc1(bsize,sizeof(char));
		
		/* create initial data for slaves this has to be sent only ones*/
		flag=-1;
		MPI_Pack(&flag   ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&n2d    ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&n3d    ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&nk2    ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&nk3    ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&dk2    ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&dk3    ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&dz     ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&dw     ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&nwp_max,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&sfft   ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
		MPI_Pack(&L      ,1,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD); 
		/* Survey datum part */
		MPI_Pack(&Sr_idepth[0][0],n3d*n2d,MPI_INT,buffer,bsize,&position,MPI_COMM_WORLD);/* Topography surface for migration in depth samples */			
		
		{ int irank;
			for(irank=1;irank<NPROC;irank++) {
					fprintf(stderr," Sending initial data to %d %d\n",ipr,position);
					MPI_Send(buffer, position, MPI_PACKED,irank,3,MPI_COMM_WORLD);
			}
		}
		flag=0;
	  
		vslc = ealloc2float(n3d,n2d);
		swslc_r = ealloc2float(n3d,n2d);
		v = ealloc1float(nz);
		re = ealloc1float(n3d*n2d*nwp_max);
		im = ealloc1float(n3d*n2d*nwp_max);
				
		/* Get the first velocity slice */
		efread(&vslc[0][0],sizeof(float),n2d*n3d,tmpvelfp);
		/* Process the velocity slice */
		/* Return the optimal migration velocities */
		v = get_mv(&vslc[0][0],n3d*n2d,L,bdv,v,&nvel);
		qksort(nvel,v);			
		
		
		/* Loop over Z values */
		for(iz=0,z=0;iz<nz;iz++,z+=dz) {
			if(verbose>0) fprintf(stderr," Depth step# %d , depth %f\n",iz,z);
			
			if(imax_Sr < iz) is_Sr=0; 		/* below the topography */
			
			if(is_Sr) {
				if(verbose>0) fprintf(stderr," Above topography\n");
			}
			
			/* Echo the number of velocities */
			if(verbose) fprintf(stderr," Number of velocities= %d\n",nvel);
			if(verbose) {
				{ int i;
					for(i=0;i<nvel;i++) 
						fprintf(stderr," %f\n",v[i]);
				}
			}
			if(verbose) fprintf(stderr,"\n");

			{ 
			  int iwcs=0;
			  float ws=0.00001;
				
				for(ipr=1;ipr<NPROC;ipr++) {

					
					position=0;
					MPI_Pack(&flag     ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD); 
					MPI_Pack(&ws       ,1,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD); 
					MPI_Pack(&is_Sr    ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD); 
					MPI_Pack(&iz       ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD); 
					MPI_Pack(&nwp[ipr] ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD); 
					MPI_Pack(&nvel     ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD);
					MPI_Pack(&v[0]     ,nvel,MPI_FLOAT,buffer,bsize,&position,MPI_COMM_WORLD);
					
					/* Tag 5 */
					MPI_Send (buffer,bsize, MPI_PACKED,ipr,5,MPI_COMM_WORLD);
					  
					if(verbose>1) fprintf(stderr," %d \n",nwp[ipr]);
					
					/* Tag 7 */
					MPI_Send (&wd[iwcs][0][0],n2d*n3d*nwp[ipr]*sizeof(complex),
					          MPI_CHAR,ipr,7,MPI_COMM_WORLD);/* complex wxy wavefield */
					
					/* Tag 9 */
					MPI_Send (&vslc[0][0],n2d*n3d,
					          MPI_FLOAT,ipr,9,MPI_COMM_WORLD);/* velocity slice array */
						  
					iwcs+=nwp[ipr];
					ws=iwcs*dw;
				}
		
				/* Zero the frequnecy summation  array */
				memset( (void *) &swslc_r[0][0],0,n3d*n2d*FSIZE);
				
				/* Prepare for the next step while the slaves are working */
				/* Get the next velocity slice - if not the last iteration*/
				if(iz != nz-1) {
					efread(&vslc[0][0],sizeof(float),n2d*n3d,tmpvelfp);
					/* Process the velocity slice */
					/* Return the optimal migration velocities */
					v = get_mv(&vslc[0][0],n3d*n2d,L,bdv,v,&nvel);
					qksort(nvel,v);
				}		
				
				
				if(verbose>1) fprintf(stderr," Collect start \n");		
				/*for(ipr=NPROC-1;ipr>0;ipr--,iwcs=ifnw-nwp[ipr]) { */
				iwcs=0;
				for(ipr=1;ipr<NPROC;ipr++) { 
				

					MPI_Recv(&wd[iwcs][0][0],n2d*n3d*nwp[ipr]*sizeof(complex),
					         MPI_CHAR,ipr,2,MPI_COMM_WORLD,&status);/* complex wxy wavefield */

					MPI_Recv(&re[0],n3d*n2d,
					         MPI_FLOAT,ipr,4,MPI_COMM_WORLD,&status);/* Downward continued wave filed; partial result */
					
					/* Summ the partial summs */
					
					for(i2=0;i2<n2d;i2++) 
						for(i3=0;i3<n3d;i3++)
							swslc_r[i2][i3] += re[i2*n3d+i3];
					iwcs+=nwp[ipr];
					
					if(verbose>1) fprintf(stderr," Received Stored %d %d %d %d\n",
					                               ipr,iwcs,nwp[ipr],ifnw);

				}
			}
			
			/* Store the result  , do not store the padding edge */
			
			for(i2=0;i2<n2;i2++) {
				efwrite(&swslc_r[i2+dmp][dmp],sizeof(float),n3,tmpmigfp);
			}
			if(verbose>0) fprintf(stderr," Storing results at depth done\n");
			
			/* Compute the input wavefiles for the next depth level */
			/* and do absorbtion on edges */
			/* FFT back from K-> to X */
			/* Loop of frequency values */
			if(verbose>0) fprintf(stderr," Computing wavefield at next depth step\n");
			for(iw=0;iw<ifnw;iw++) { 
					absorb(dmp,&wd[iw][0][0],n3d,n2d);
			}			
			if(verbose>0) fprintf(stderr," Done\n");
		}
		
		free3complex(wd);
		free1float(v);
		free2float(vslc);
		free2float(swslc_r);
		efclose(tmpvelfp);
		
		/* close  slaves */
		{ int ipr;
	  
			for(ipr=0;ipr<NPROC;ipr++) {
				flag=1;
				position=0;
				MPI_Pack(&flag     ,1,MPI_INT,  buffer,bsize,&position,MPI_COMM_WORLD);					/* Tag 5 */
				MPI_Send (buffer,bsize, MPI_PACKED,ipr,5,MPI_COMM_WORLD);
				fprintf(stderr," Closing slaves %d\n",ipr); 
			}
		}
	}
	
	
	/* write out the results */
	{ float **mig_slice;
	  register int iz,i2,i3;
		
		efseeko(tmpmigfp,0,SEEK_SET);
		datam = ealloc2float(nz,ntr);
		mig_slice = ealloc2float(n3,n2);
	
		
		for(iz=0;iz<nz;iz++) {
			for(i2=0;i2<n2;i2++) {
				efread(&mig_slice[i2][0],sizeof(float),n3,tmpmigfp);
				for(i3=0;i3<n3;i3++) {
					datam[i3*n2+i2][iz] = mig_slice[i2][i3];
				}
			}
		}
		free2float(mig_slice);
	}
	efclose(tmpmigfp);
	
	{ register int i2,i3;
	for(i3=0;i3<n3;i3++) 
		for(i2=0;i2<n2;i2++) {
			/* spit out the gather */
			efread(&tr, 1, HDRBYTES, headerfp);
		 	/* bmread(data,1,0,i3*n2+i2,nz,tr.data); */
			memcpy( (void *) &tr.data[0], (const void *) datam[i3*n2+i2],sizeof(float)*nz);

			tr.ns=nz;
			tr.dt=(short)(dz*1000);
			puttr(&tr);
		if(verbose>2) fprintf(stderr," %d %d\n",i2,i3);
	    }
	}
	
	/* Free data matrix */
	free2float(datam);
	
	
	/* This should be the last thing */
	efclose(headerfp);
	return EXIT_SUCCESS;
}

void slavecode()
{

	int flag;
	int n3d;
	int n2d;
	int nk3;
	int nk2;
	float dk3;
	float dk2;
	float dz;
	
	int i2;
	int i3;
	int iw;
	int nw;
	int nwp;
	float w;
	float ws;
	float dw;
	int iv;
	int nv;
	float sfft;
	int is_Sr;
	int iz;
	int L;
	
	complex cshft1;
	complex ctmpa;
	
	/* arrays */
	float   **vslc   =NULL;
	float   **vslcd  =NULL;
	float   **swslc_r=NULL;
	complex ***wxslc =NULL;
	complex **wkslc  =NULL;
	complex ***iwkslc=NULL;
	float         *re=NULL,*im =NULL;
	float   *v       =NULL;
	float   ***k_address_table =NULL;
	float   **ks_table   =NULL;
	int     **Sr         =NULL;
	short   **exp_sgn    =NULL;
	float   *re_i    =NULL,*im_i  =NULL;
	
	/* MPI */
	int rank;
	int tag;
	MPI_Status status;
	int position=0;
	char *buffer;
	int bsize;
	
	MPI_Comm_rank(MPI_COMM_WORLD,&rank);
	
	if(verbose>1) fprintf(stderr," Initialized %d\n",rank);
	
	MPI_Recv(&bsize, 1, MPI_INT,0,1,MPI_COMM_WORLD,&status);
	buffer = ealloc1(bsize,sizeof(char));
	if(verbose>2) fprintf(stderr," Reveived buffer size %d\n",bsize);
	
	/* Get Initial data */
	MPI_Recv(buffer, bsize, MPI_PACKED,0,3,MPI_COMM_WORLD,&status);
	if(verbose>2) fprintf(stderr," Reveived buffer\n");
	
	position=0;
	MPI_Unpack(buffer,bsize,&position,&flag   ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&n2d    ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&n3d    ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&nk2    ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&nk3    ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&dk2    ,1,MPI_FLOAT,MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&dk3    ,1,MPI_FLOAT,MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&dz	  ,1,MPI_FLOAT,MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&dw	  ,1,MPI_FLOAT,MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&nw	  ,1,MPI_INT,  MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&sfft   ,1,MPI_FLOAT,MPI_COMM_WORLD); 
	MPI_Unpack(buffer,bsize,&position,&L	    ,1,MPI_INT,  MPI_COMM_WORLD);
	if(verbose>2) fprintf(stderr," Unpacked buffer 1\n");
        
	/* Allocate working arrays */
	
	re = ealloc1float(n2d*n3d*nw);
        im = ealloc1float(n2d*n3d*nw);
        wxslc =  ealloc3complex(n3d,n2d,nw);
        wkslc =  ealloc2complex(nk3,nk2);
        vslc =   ealloc2float(n3d,n2d);
        vslcd=   ealloc2float(n3d,n2d);
        swslc_r =  ealloc2float(n3d,n2d);
	v=ealloc1float(n3d*n2d+1);
	Sr=ealloc2int(n3d,n2d);
	exp_sgn = (short **)alloc2(n3d,n2d,sizeof(short));
	iwkslc = ealloc3complex(nk3,nk2,L);
	re_i = ealloc1float(L);
	im_i = ealloc1float(L);
	if(verbose>2) fprintf(stderr," Allocated arrays\n");
		
	MPI_Unpack(buffer,bsize,&position,&Sr[0][0],n3d*n2d,
	          MPI_INT,  MPI_COMM_WORLD);/* Topography surface for migration in depth samples */
	
	if(verbose>2) fprintf(stderr," Unpacked buffer 2\n");
			
	k_address_table = (float***)ealloc2(nk3,nk2,sizeof(float*));
	ks_table = ealloc2float(nk3/2+1,nk2/2+1);
	
	/* prepare address table for wavenumbers */
	{ float k2,k3;
	 int nk2e,nk3e;
	
		if(ISODD(nk2)) {
			nk2e=nk2/2+1;
		} else {
			nk2e=nk2/2;
		}
		if(ISODD(nk3)) {
			nk3e=nk3/2;
		} else {
			nk3e=nk3/2-1;
		}
		for(i3=0;i3<=nk3/2;i3++) {
        		k3 = i3*dk3;
			for(i2=0;i2<=nk2/2;i2++) {
        			k2 = i2*dk2;
				ks_table[i2][i3] = k3*k3+k2*k2;  
				/* Fill the address table + + quadrant*/
				k_address_table[i2][i3] = &ks_table[i2][i3];
				/*fprintf(stderr," 1- %d %d\n",i2,i3); */
			}
		}
		/* Fill the rest of the address table */
		/* - - */
		for(i3=nk3-1;i3>=nk3/2+1;i3--) {
			for(i2=nk2-1;i2>=nk2/2+1;i2--) {
				k_address_table[i2][i3] = &ks_table[nk2-i2][nk3-i3];
				/*fprintf(stderr," 2- %d %d %d %d\n",i2,i3,nk2-i2,nk3-i3); */
			}
		}
		/* + - */
		for(i3=nk3-1;i3>=nk3/2+1;i3--) {
			for(i2=0;i2<=nk2/2;i2++) {
				k_address_table[i2][i3] = &ks_table[i2][nk3-i3];
				/*fprintf(stderr," 3- %d %d %d %d\n",i2,i3,i2,nk3-i3); */
			}
		}
		/* - + */
		for(i3=0;i3<=nk3/2;i3++) {
			for(i2=nk2-1;i2>=nk2/2+1;i2--) {
				k_address_table[i2][i3] = &ks_table[nk2-i2][i3];
				/*fprintf(stderr," 4- %d %d %d %d\n",i2,i3,nk2-i2,i3); */
			}
		}
		
		/*FILE *fp;
		fp=fopen("wavenumbers.txt","w");
		for(i3=0;i3<nk3;i3++) 
			for(i2=0;i2<nk2;i2++) {
				fprintf(stderr," W %d %d\n",i2,i3);
				fprintf(fp," %e\n",*k_address_table[i2][i3]);
			}
		fclose(fp); */
		
		
	}
	if(verbose) fprintf(stderr," Slave %d initialized..\n",rank);
	/* Receive data from master */
	while(1) {
		
		/* receive */
		position=0;
		MPI_Recv(buffer, bsize, MPI_PACKED,0,5,MPI_COMM_WORLD,&status);
		/*if(verbose>2) fprintf(stderr," Received data buffer %d\n",rank); */
		
		MPI_Unpack(buffer,bsize,&position,&flag   ,1,MPI_INT,    MPI_COMM_WORLD);
		if(flag) {
			/* received exit signal */
			exit(0);
		} 
		MPI_Unpack(buffer,bsize,&position,&ws     ,1,MPI_FLOAT,  MPI_COMM_WORLD);
		MPI_Unpack(buffer,bsize,&position,&is_Sr  ,1,MPI_INT,    MPI_COMM_WORLD);
		MPI_Unpack(buffer,bsize,&position,&iz     ,1,MPI_INT,    MPI_COMM_WORLD);
		MPI_Unpack(buffer,bsize,&position,&nwp     ,1,MPI_INT,    MPI_COMM_WORLD);
		MPI_Unpack(buffer,bsize,&position,&nv   ,1,MPI_INT,    MPI_COMM_WORLD);
		MPI_Unpack(buffer,bsize,&position,&v[0]   ,nv,MPI_FLOAT,    MPI_COMM_WORLD);
		
		/*if(verbose>2) fprintf(stderr," Unpacked data buffer %d\n",rank); */

		MPI_Recv(&wxslc[0][0][0], n3d*n2d*nwp*sizeof(complex), MPI_CHAR,0,7,MPI_COMM_WORLD,&status);
		MPI_Recv(&vslc[0][0], n3d*n2d, 		   MPI_FLOAT,0,9,MPI_COMM_WORLD,&status);
		/*if(verbose>2) fprintf(stderr," Received frequency and velocity %d\n",rank); */
		
		/* Compute */
		memcpy( (void *) &vslcd[0][0], (const void *) &vslc[0][0],n2d*n3d*FSIZE);
		for(i3=0;i3<n3d;i3++)
			for(i2=0;i2<n2d;i2++)
				exp_sgn[i2][i3]=1;

                
		if(is_Sr) {
			/* If above migration surface */
			int iflag=0,aflag=1;
			   /*Zero velocities that are above the Sr surface */
			   /* Also adjust nv */
			   
			   for(i3=0;i3<n3d;i3++)
			   	for(i2=0;i2<n2d;i2++)
					if(Sr[i2][i3] > iz) {
						vslcd[i2][i3]=0.0;
						exp_sgn[i2][i3]=-1;
						iflag=1;
					} else {
						aflag=0;
					}

			   if(iflag && aflag) {
			   	v[0]=0.0;
			  	 nv=1;
			   } else if ( iflag && !aflag) {
			   	v[nv]=0.0;
				nv++;
			   }
		}
					
		memset((void *) &swslc_r[0][0],0,n2d*n3d*FSIZE);
		
		/* Loop over frequency slices */
		for(iw=0,w=ws;iw<nwp;iw++,w+=dw) {
		
			
			/* Loop of spatial values : do the FIRST PHASE SHIFT thin lens term */ 
			
			if(is_Sr) {
				/* If z is above migration surface */
				for(i3=0;i3<n3d;i3++) {
					for(i2=0;i2<n2d;i2++) {
					
						/* first phase shift factor */
						cshft1 = crmul(cexp(cmplx(0.0,exp_sgn[i2][i3]*w*dz/vslc[i2][i3])),sfft);
						ctmpa = wxslc[iw][i2][i3];
			 			wxslc[iw][i2][i3] = cmul(ctmpa,cshft1);
						/*if(verbose>2) fprintf(stderr," iw i2 i3= %d %d %d\n",iw,i2,i3); */
					}
				}
			} else {
				/* below migration surface */
				for(i3=0;i3<n3d;i3++) {
					for(i2=0;i2<n2d;i2++) {
					
						cshft1 = crmul(cexp(cmplx(0.0,w*dz/vslc[i2][i3])),sfft);
						ctmpa = wxslc[iw][i2][i3];
			 			wxslc[iw][i2][i3] = cmul(ctmpa,cshft1);
						/*if(verbose>2) fprintf(stderr," iw i2 i3= %d %d %d\n",iw,i2,i3); */
					}
				}
			}
			
			
			/* SECOND PHASE SHIFT */
			
			
				/* PHASE SHIFT WITH NVEL VELOCITIES */
		
				/*if(verbose>2) fprintf(stderr," Second Phase shift %d %d\n",L,nv); */
				/* X -> K */
				/* Zero k array */
				memset((void *) &wkslc[0][0],0,nk2*nk3*sizeof(complex));
			
				/* Zero the interpolation array */
				memset( (void *) &iwkslc[0][0][0],0,nk3*nk2*nv*sizeof(complex));

			
				for(i2=0;i2<n2d;i2++)
					memcpy((void *) wkslc[i2],(const void *) wxslc[iw][i2],
				                                           n3d*sizeof(complex));
			
			
				/* Fourier transform of w slice from X to K */
				/* Both in d2 and d3 directions */
				pfa2cc(-1,2,nk3,nk2,&wkslc[0][0]);  
				pfa2cc(-1,1,nk3,nk2,&wkslc[0][0]);
		
				/*if(verbose>1) fprintf(stderr," FFT X->K done iw= %d \n",iw); */
				/* Do the migration with nvel velocities */
			
				{ 
			 	double kzst;
			  	float vv,phase,cfact,kfact,ephase;
			  	complex cshft2,*cwf, *cwfp;
				
					/* Loop over velocity values */
					for(iv=0;iv<nv;iv++) {
						
						vv = v[iv];
				
						/* Special case for 0 velocity */
						if(vv==0.0) {
							memcpy((void *) &iwkslc[iv][0][0],(const void *) &wkslc[0][0],
								nk2*nk3*sizeof(complex));
						} else {
				
							cfact = dz*w/vv;
							kfact = vv*vv/(w*w);
					
							/*if (verbose>1) fprintf(stderr,"Migration velocity %f\n",vv); */
						
							/* Loop of spatial values */ 
							for(i3=0;i3<nk3; i3++) {
               			 	
					
								for(i2=0;i2<nk2 ;i2++) {
									
									cwfp = &iwkslc[iv][i2][i3];
									cwf  = &wkslc[i2][i3];
									
									kzst = 1.0 - (*k_address_table[i2][i3])*kfact;
						
									if(kzst < 0) {
										/* evanesence waves */
										/* Instead of ingnoring them, exponential decay */
										ephase=-cfact*sqrt(-kzst);
										phase = cfact;
										cshft2  = crmul(cmplx(cos(phase),sin(phase)),exp(ephase));
									} else {
										/* Normal waves */
										phase = -cfact*(sqrt(kzst)+1.0);
										cshft2 = cmplx(cos(phase),sin(phase));
									*cwfp = cmul(*cwf,cshft2);
										
									}
								}
							}
						}
					}
					/*if(verbose>1) fprintf(stderr," Phase shift2 for %d velocities done\n",nv); */
				
					/* Interpolate wavefiled from different frequency slices */
					/* go from K to X */
					for(iv=0;iv<nv;iv++) {
						pfa2cc(1,2,nk3,nk2,&iwkslc[iv][0][0]);  
						pfa2cc(1,1,nk3,nk2,&iwkslc[iv][0][0]);
					}
					/*if(verbose>1) fprintf(stderr," FFT K->X done\n"); */
					/* Do the interpolation among frequency slices with different velocities */
				
					if(nv>1) {
				
						{ static float *vc, *ar,*ai;
					
							for(i3=0;i3<n3d; i3++) {
								for(i2=0;i2<n2d; i2++) {
									vc = &vslcd[i2][i3];
									ar = &wxslc[iw][i2][i3].r;
									ai = &wxslc[iw][i2][i3].i;
									for(iv=0;iv<nv;iv++) {
										re_i[iv]=iwkslc[iv][i2][i3].r;
										im_i[iv]=iwkslc[iv][i2][i3].i;
									}
									intlin(nv,v,re_i,re_i[0],re_i[nv-1],1,vc,ar);	
									intlin(nv,v,im_i,im_i[0],im_i[nv-1],1,vc,ai);
								}
							}
						}
					} else {
				
							/*for(i3=0;i3<n3d; i3++)  */
						for(i2=0;i2<n2d; i2++)
							memcpy((void *) &wxslc[iw][i2][0], (const void *) &iwkslc[0][i2][0], n3d*sizeof(complex)); 
					}
					/*if(verbose>1) fprintf(stderr," Interpolation of wavefiled done\n"); */
				}
				
				/* Store the Summ of frequencies */
				/* P(x,y,z+dz,Sum w) = P(x,y,z+dz,t=0) */
				/* The results go into swslc frequency slice*/
				for(i3=0;i3<n3d;i3++)
					for(i2=0;i2<n2d;i2++)
						swslc_r[i2][i3] += wxslc[iw][i2][i3].r;
						
		}
					/* testing */
		/*for(iw=0;iw<nw;iw++) 
			for(i2=0;i2<n2d;i2++)
				for(i3=0;i3<n3d;i3++)
					fprintf(stderr," %f\n",swslc[i2][i3].r); */

		/* Send data back to master */ 
		/*if(verbose>1) fprintf(stderr," Sending Results start %d\n",rank); */
		MPI_Send (&wxslc[0][0][0],n3d*n2d*nwp*sizeof(complex),
				MPI_CHAR,0,2,MPI_COMM_WORLD);
		MPI_Send (&swslc_r[0][0],n3d*n2d,
				MPI_FLOAT,0,4,MPI_COMM_WORLD);
		/*if(verbose>1) fprintf(stderr," Sending Results done %d\n",rank); */
		
	}
}

void absorb(int bnd, complex *m, int n3, int n2)
/* taper the values on the edges of the complex data matrix */
{
	int i3,i2,i;
	static int first;
	static float *a;
	float aa=0.005;
	
	if(bnd>n2/2 || bnd>n3/2) return;
	if(!first) {
		a = ealloc1float(bnd);
		for(i3=0;i3<bnd;i3++) {
			a[i3] = exp(-aa*(bnd-i3)*(bnd-i3));
		}
		first=1;
	}
	
	
	for(i2=0;i2<n2;i2++) {
		for(i3=0; i3<bnd;i3++) 
			m[i2*n3+i3] = crmul(m[i2*n3+i3],a[i3]);
		for(i3=n3-1, i=0; i3>=n3-bnd;i3--,i++) 
			m[i2*n3+i3] = crmul(m[i2*n3+i3],a[i]);
	}
	
	for(i3=0;i3<n3;i3++) {
		for(i2=0; i2<bnd;i2++) 
			m[i2*n3+i3] = crmul(m[i2*n3+i3],a[i2]);
		for(i2=n2-1, i=0; i2>=n2-bnd;i2--,i++) 
			m[i2*n3+i3] = crmul(m[i2*n3+i3],a[i]);
	}
		
}

float *get_mv(float *vslc,int n,int L,float bdv,float *v,int *nv)
/* Return the optimal migration velocities 
   Input:
   
   vslc - velocity array 
   n   -  dimension
   
   L	-  Maximum number of binns to determine velocities
   bdv -   Minimum bin width
   
   return:
   v	- array of optimal migration velocities; it is allocated here
   nv	- number of velocities
   
   Reference: Bagaini 1995;
   
*/
  
{
	float vmin=9999.99,vmax=-1.0;	
	int i,ib,bz;
	float *c,*P,*y,Sz=0.0,dv;
	
	for(i=0;i<n;i++) { 
		if(vmin>vslc[i]) vmin=vslc[i];
		if(vmax<vslc[i]) vmax=vslc[i];
	}

	if(v!=NULL) free1float(v);

	/* Handle case when vmax is close to vmin; constat velocity */
	if((vmax-vmin)/vmin < 0.01) {
		v=ealloc1float(1);
		*nv=1;
		v[0]=vmin+(vmax-vmin)/2.0;
		return(v);
	}
	
	/* Do not let to fine velocity binsizes */
	{ int cL;
		cL=MAX(NINT((vmax-vmin)/bdv),1);
		if(cL<L) L=cL;
	}
		
	
	/* Set up the velocity bins */
	c = ealloc1float(L+1);
	dv =(vmax-vmin)/((float)(L)); 
	for(i=0;i<L+1;i++) c[i]=vmin+i*dv;
	
	/* Probabaility density */
	P = ealloc1float(L);
	memset( (void *) P,0,L*sizeof(float));
	
	/* Do the binnig */
	/* Does not seem to worth the effort to come up with a better binnig code */
	for(i=0;i<n;i++) {
		ib=0;
		while(vslc[i] >= c[ib]+dv && ib < L-1) {
			ib++;
		}
		P[ib]+=1.0/n;
	}
	
	
			
	/* Compute entropy */	
	for(ib=0;ib<L;ib++) {
		if(!CLOSETO(P[ib],0.0))  
			Sz += P[ib]*log(P[ib]);
	}
	Sz *=-1.0;
	
	/* Optimum number of bins */
	bz = NINT(exp(Sz));
	
	/* Interpolation of the prob. dens. function */
	y = ealloc1float(L+1);
	y[0]=0.0; y[L]=1.0;
	
	for(ib=1;ib<L;ib++) {
		y[ib]=0.0;
		for(i=0;i<ib;i++) y[ib] += P[i];
	}
	
	/*Ready to compute the velocoties */
	v = ealloc1float(bz+1);
	v[0] = vmin;
	
	/* Velocity binning */
	{ float b1;
	for(i=1;i<bz+1;i++) {
		b1 = (float)i/(float)bz;
		ib=0;
		while(b1>y[ib+1] && ib < L-1 ) {
			ib++;
		}
		v[i] = c[ib] + (b1-y[ib])*(c[ib+1]-c[ib])/(y[ib+1]-y[ib]);
		/* fprintf(stderr," %d %f %f %f %f %f\n",ib,c[ib],c[ib+1],y[ib],y[ib+1],b1); */
	}
	}
		
	*nv=bz+1;
	free1float(P);
	free1float(c);
	free1float(y);
	return(v);		
	
}

/* Absornig boundary routines */

void patch_corner(void ***a,int n1,int n2,int bnd,int d1,int d2,int n,size_t asize)
/* Copy the corner element of a matrix into the absorbing zone 


	x x x x o...
        x x x x o
        x x x x o
	x x x a b
	o o o c d
	.
	.
	.
	
	
	elements marked by x are going to be filled with element a
	
	a bigmatrix matrix
	n3 = number of slices in bigmatrix
	n1 index 1 of element a
	n2 index 2 of element a
	bnd thickness
	d1 direction of fill -1 smaller towards smaller indexes
	d2 direction of fill 
	can be only -1 or +1
*/

{
	segy trt;
	
	
	int i1,i2;
	int ind1,ind2;
	
	
	/*bmread(a,1,n1,n2,1,&trt.data[0]); */			
	memcpy( (void *) &trt.data[0], (const void *) a[n1][n2],asize*n);

	
	for(i1=0, ind1=n1 ; i1<=bnd; i1++, ind1+=d1) {
		for(i2=0, ind2=n2 ; i2<=bnd; i2++, ind2+=d2) {
			/* bmwrite(a,1,ind1,ind2,1,&trt.data[0]); */
			memcpy( (void *) a[ind1][ind2], (const void *) &trt.data[0],asize*n);
			/* fprintf(stderr," %d %d\n",ind1,ind2); */
		}
	}
}

void patch_segment(void ***a,int n11,int n12,int n21,int n22,int bnd,int d1,int d2,
		   int n1min, int n1max,int n2min,int n2max,int n,size_t asize)

/* make a boundare along a segment
  defined by n11 n12 n21 n22 
  a - bigmatrix
  n11 - segment start coordinate 1
  n12 - segment start coordinate 2
  n21 - segmnet end coordinate 1
  n22 - segment end coordinate 2
  
  bnd - boundary length
  d1 - -1 0 1
  d2 - -1 0 1
  n1max, n1min , n2max, n2min, limits of a 
 */
 
 
{
	segy trt;
	
	int i1,i2;
	int n1,n2;
	int ind1,ind2;
	int inp1,inp2;
	int ip1,ip2;
	
	n1=n21-n11;
	n2=n22-n12;
	
	for(i1=0, ind1=n11; i1<=n1; i1++, ind1++) {
		ind1 = MIN(MAX(ind1,n1min),n1max);
		for(i2=0, ind2=n12; i2<=n2; i2++, ind2++) {
			ind2 = MIN(MAX(ind2,n2min),n2max);
			
			/* bmread(a,1,ind1,ind2,1,&trt.data[0]); */
			memcpy( (void *) &trt.data[0], (const void *) a[ind1][ind2],asize*n);
			
			/* do the padding on the border */
			for(ip1=0,inp1=ind1+d1;ip1<bnd;ip1++, inp1+=d1) {
				inp1 = MIN(MAX(inp1,n1min),n1max);
				for(ip2=0,inp2=ind2+d2;ip2<bnd;ip2++,inp2+=d2 ) {
					inp2 = MIN(MAX(inp2,n2min),n2max);
					/* bmwrite(a,1,inp1,inp2,1,&trt.data[0]); */
					memcpy( (void *) a[inp1][inp2], (const void *) &trt.data[0],asize*n);
					/* fprintf(stderr," %d %d\n",inp1,inp2); */

				}
			}
			
		}
	}	
}

void patch_corner_2d(void ** a,int n1,int n2,int bnd,int d1,int d2,int n,size_t asize)
/* Copy the corner element of a matrix into the absorbing zone 


	x x x x o...
        x x x x o
        x x x x o
	x x x a b
	o o o c d
	.
	.
	.
	
	
	elements marked by x are going to be filled with element a
	
	a bigmatrix matrix
	n3 = number of slices in bigmatrix
	n1 index 1 of element a
	n2 index 2 of element a
	bnd thickness
	d1 direction of fill -1 smaller towards smaller indexes
	d2 direction of fill 
	can be only -1 or +1
*/

{
	segy trt;
	
	
	int i1,i2;
	int ind1,ind2;
	
	
	/*bmread(a,1,n1,n2,1,&trt.data[0]); */			
	memcpy( (void *) &trt.data[0], (const void *) a[n1]+n2*asize,asize*n);

	
	for(i1=0, ind1=n1 ; i1<=bnd; i1++, ind1+=d1) {
		for(i2=0, ind2=n2 ; i2<=bnd; i2++, ind2+=d2) {
			/* bmwrite(a,1,ind1,ind2,1,&trt.data[0]); */
			memcpy( (void *) a[ind1]+ind2*asize, (const void *) &trt.data[0],asize*n);
			/* fprintf(stderr," %d %d\n",ind1,ind2); */
		}
	}
}

void patch_segment_2d(void **a,int n11,int n12,int n21,int n22,int bnd,int d1,int d2,
		   int n1min, int n1max,int n2min,int n2max,int n,size_t asize)

/* make a boundare along a segment
  defined by n11 n12 n21 n22 
  a - bigmatrix
  n11 - segment start coordinate 1
  n12 - segment start coordinate 2
  n21 - segmnet end coordinate 1
  n22 - segment end coordinate 2
  
  bnd - boundary length
  d1 - -1 0 1
  d2 - -1 0 1
  n1max, n1min , n2max, n2min, limits of a 
 */
 
 
{
	segy trt;
	
	int i1,i2;
	int n1,n2;
	int ind1,ind2;
	int inp1,inp2;
	int ip1,ip2;
	
	n1=n21-n11;
	n2=n22-n12;
	
	for(i1=0, ind1=n11; i1<=n1; i1++, ind1++) {
		ind1 = MIN(MAX(ind1,n1min),n1max);
		for(i2=0, ind2=n12; i2<=n2; i2++, ind2++) {
			ind2 = MIN(MAX(ind2,n2min),n2max);
			
			/* bmread(a,1,ind1,ind2,1,&trt.data[0]); */
			memcpy( (void *) &trt.data[0], (const void *) a[ind1]+ind2*asize,asize*n);
			
			/* do the padding on the border */
			for(ip1=0,inp1=ind1+d1;ip1<bnd;ip1++, inp1+=d1) {
				inp1 = MIN(MAX(inp1,n1min),n1max);
				for(ip2=0,inp2=ind2+d2;ip2<bnd;ip2++,inp2+=d2 ) {
					inp2 = MIN(MAX(inp2,n2min),n2max);
					/* bmwrite(a,1,inp1,inp2,1,&trt.data[0]); */
					memcpy( (void *) a[inp1]+inp2*asize, (const void *) &trt.data[0],asize*n);
					/* fprintf(stderr," %d %d\n",inp1,inp2); */

				}
			}
			
		}
	}	
}

void comp_data_part(int ifnw,int nproc,int nwp[],int *nwpp)
{				
		int rem;
		/* Compute data partion among processes */
		*nwpp = (int)(ifnw/nproc);
		
		rem = ifnw-(*nwpp*nproc);
		
		{ int i;
			for(i=0;i<nproc;i++)
				nwp[i]=*nwpp;
		
			/* rem is always < nproc */
			/* distribute the remaning loads */
			i=0;
			while (rem!=0) {
				nwp[i] +=1;
				rem    -=1;
				i      +=1;
			}
			
			/* Compute the maximum number of loads */
			/* return it in nwpp */
			rem=0;
			for(i=0;i<nproc;i++) {
				if(*nwpp<nwp[i]) *nwpp=nwp[i];
				rem +=nwp[i];
			}
			if(rem!=ifnw)
				err("Error in load ditribution\n");
		}
}
