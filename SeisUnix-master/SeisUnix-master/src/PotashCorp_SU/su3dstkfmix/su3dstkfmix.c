#include "suhdr.h"
#define LOOKFAC         2       /* Look ahead factor for npfao    */
#define PFA_MAX         720720  /* Largest allowed nfft           */

/*********************** self documentation **********************/
char *sdoc[] = {
"                                                               ",
" SU3DSTKFMIX   - Frequency domain trace smoothing of stack     ",
"                                                               ",
" su3dstkfmix  <stdin    >stdout              		        ",
"                                                               ",
" Required parameters:						",
"                                                               ",
"      n2=            size of the cube in direction 2           ",
"      n3=            size of the cube in direction 3           ",
"                                                               ",
"      data needs to be sorted n2 n3 (susort n2 n3)             ",
"                                                               ",
" Optional parameters:                                          ",
"      l2=3	      Operator length in dir 2                  ",
"      l3=3	      Operator length in dir 3                  ",
"      fftpad=50.0    Percent of fftpad                         ",
"                                                               ",
"      mode=1                                                   ",
"                                                               ",
"                                                               ",
"      smoothing modes:                                         ",
"                     1 damped least squares                    ",
"                     2 Savitzky-Golay least squares            ",
"                     3 Gaussian                                ",
"                                                               ",
"                                                               ",
"      Note: 	      direction 1 is equal to tr.ns             ",
"                                                               ",
"								",
NULL};

segy tr;

int
main(int argc, char **argv)
{
        int n1,n2,n3;		/* Cube dimensions */
	
	complex ***data;	/* data cube */
        char **hdrdata;		/* header array */
	
	float l2,l3;		/* Filter length */
	
				/* Fourier transform variables */
	float dt;
	int ntfft;
	int nw;
	float dw;
	float fw;
	float tsfft;
	float fftpad;
	complex *ct;
	
	
				/* switches */
	int mode;
	int test;
	
	
	float *SG_filter_2=NULL;
	float *SG_filter_3=NULL;
	int sm2,sm3;
	
	
	
	/* Initialize */
        initargs(argc, argv);
        requestdoc(1);
       	
       	MUSTGETPARINT("n2", &n2);
       	MUSTGETPARINT("n3", &n3);
       	if (!getparfloat("l2", &l2))  l2 = 3; 
       	if (!getparfloat("l3", &l3))  l3 = 3;
	sm2=NINT(l2);
	sm3=NINT(l3);	
       	if (!getparfloat("fftpad", &fftpad))  fftpad = 50.0;
	fftpad = 1.0+fftpad/100.0;	
	
	/* test mode switch */
	if (!getparint("test",&test)) test=0;
	if (!getparint("mode",&mode)) mode=1;
	
       
       
       	/* Get info from first trace */
       	if (!gettr(&tr))  err ("can't get first trace");
       	n1 = tr.ns;
	dt = (float)(tr.dt)/1000000.0;
	
	/* Set up FFT */
	ntfft = npfar(NINT(n1*fftpad));
	nw = ntfft/2+1;
	dw = 1.0/(ntfft*dt);
	fw = 0;
	tsfft = 1.0/ntfft;
	
	ct = ealloc1complex(nw);
	data = ealloc3complex(nw,n2,n3);
	hdrdata = bmalloc(HDRBYTES,n2,n3);
	
	
	/* read the rest of the traces */
	{ int i2=0,i3=0;
		
		
		do{ 
			memset((void *) &tr.data[n1], (int) '0', (ntfft-n1)*sizeof(float));			
			pfarc(1,ntfft,tr.data,ct);
			memcpy( (void *) data[i3][i2], ct,nw*sizeof(complex));
			bmwrite(hdrdata,1,i2,i3,1,&tr);
			i3++;
			if(i3==n3) {
				i3=0;
				i2++;
			}
		}while(gettr(&tr)); 
	}
	
	
	if(test) {
		/* write out the traces */
		{ int i1,i2,i3;
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
					memcpy( (void *) tr.data,data[i3][i2],nw*sizeof(complex));
					bmread(hdrdata,1,i2,i3,1,&tr);
					tr.trid=FUNPACKNYQ;
					tr.ns=nw*2;
					puttr(&tr);
				}
			}
		}
	}
	
	
	if (mode==2) {
		/* Desing SG filters */
		
		SG_filter_2=ealloc1float(2*sm2+1);
		SG_smoothing_filter(2*sm2+1,sm2,sm2,0,4,SG_filter_2);
		
		SG_filter_3=ealloc1float(2*sm3+1);
		SG_smoothing_filter(2*sm3+1,sm3,sm3,0,4,SG_filter_3);
	}
	
	
	
	/* Filtering */
	{ int iw,i2,i3;
	  float **ra;
	  float **ia;
	  	
		ra = ealloc2float(n3,n2);
		ia = ealloc2float(n3,n2);
		
		for(iw=0;iw<nw;iw++) {
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
					ra[i2][i3] = data[i3][i2][iw].r;
					ia[i2][i3] = data[i3][i2][iw].i; 
				}
			}
		
			




			switch (mode){
				case 1:
					dlsq_smoothing (n3,n2,0,n3,0,n2,(float)l3,(float)l2,0,ra);
					dlsq_smoothing (n3,n2,0,n3,0,n2,(float)l3,(float)l2,0,ia);
				break;
				
				case 2:
					/* Direction 3 */
					
					for(i2=0;i2<n2;i2++) {
						conv (2*sm3+1,-sm3,SG_filter_3,n3,0,ra[i2],n3,0,(float *)ct);
						memcpy( (void *) ra[i2],ct,n3*sizeof(float));
						conv (2*sm3+1,-sm3,SG_filter_3,n3,0,ia[i2],n3,0,(float *)ct);
						memcpy( (void *) ia[i2],ct,n3*sizeof(float));
					}
					
					/* Transpose and do Direction 2*/
					{ float **tmpa;
						tmpa = ealloc2float(n2,n3);
						transpose_2d((void **)ra,n2,n3,(void **)tmpa,sizeof(float));
						for(i3=0;i3<n3;i3++) {
							conv (2*sm2+1,-sm2,SG_filter_2,n2,0,tmpa[i3],n2,0,(float *)ct);
							memcpy( (void *) tmpa[i3],ct,n2*sizeof(float));
						}
						transpose_2d((void **)tmpa,n3,n2,(void **)ra,sizeof(float));
						
						transpose_2d((void **)ia,n2,n3,(void **)tmpa,sizeof(float));
						for(i3=0;i3<n3;i3++) {
							conv (2*sm2+1,-sm2,SG_filter_2,n2,0,tmpa[i3],n2,0,(float *)ct);
							memcpy( (void *) tmpa[i3],ct,n2*sizeof(float));
						}
						transpose_2d((void **)tmpa,n3,n2,(void **)ia,sizeof(float));
						free2float(tmpa);
					}

				break;
				case 3:
					gaussian2d_smoothing (n2,n3,l2,l3,ra);
		          		gaussian2d_smoothing (n2,n3,l2,l3,ia);
				break;
				default:
					err("Uknown filter mode %d\n",mode);
			}
				
			
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
					data[i3][i2][iw].r = ra[i2][i3];
					data[i3][i2][iw].i = ia[i2][i3]; 
				}
			}
			fprintf(stderr," %d / %d\n",iw,nw);
		}
		
		free2float(ra);
		free2float(ia); 
	}
	
	if(test) {
		/* write out the traces */
		{ int i1,i2,i3;
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
					memcpy( (void *) tr.data,data[i3][i2],nw*sizeof(complex));
					bmread(hdrdata,1,i2,i3,1,&tr);
					tr.trid=FUNPACKNYQ;
					tr.ns=nw*2;
					puttr(&tr);
				}
			}
		}
	}
	
	/* write out the traces */
	if(!test) {
		{ int i1,i2,i3;
			for(i2=0;i2<n2;i2++) {
				for(i3=0;i3<n3;i3++) {
					memcpy( (void *) ct,data[i3][i2],nw*sizeof(complex));
					bmread(hdrdata,1,i2,i3,1,&tr);
					pfacr(-1,ntfft,ct,tr.data);
					for(i1=0;i1<n1;i1++) tr.data[i1] *=tsfft;
					puttr(&tr);
				}
			}
		}
	}
	
 
	free3complex(data);
	bmfree(hdrdata);	
	return EXIT_SUCCESS;
}
