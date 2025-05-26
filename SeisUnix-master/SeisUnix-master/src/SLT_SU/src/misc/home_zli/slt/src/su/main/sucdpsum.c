#include "ghdr.h"
#include "gridhd.h"
#include "grid.h"
#include "subc.h"
#include "su.h"
#include "segy.h"

/*********************** self documentation ******************************/
string sdoc =
"\n"
"SUCDPSUM - sum adjacent CDP gathers to form super cdp gathers 	\n"
"\n"
"sucdpsum <stdin >stdout 						\n"
"\n"
"Required Parameters:\n"
"stdin                      Name of the file of input cdp gathers 		\n"
"                           (can also be specified as datain=stdin,     \n"
"                           instead of <stdin)				\n"
"                           must be a disk dataset of  			\n"
"                           nfold*ncdppline*nline traces		\n"
"stdout                     Name of the file of output cdp gathers 			\n"
"                           (can also be specified as dataout=stdout,   \n"
"                           instead of >stdout)				\n"
"nfold=                     number of offset traces per cdp \n"
"ncdppline=                 number of input cdp's per line 	\n" 
"nline=                 	number of input lines 	\n" 
"Optional Parameters:\n"
"nsumcdp=1                  number of input (inline) cdp's to sum per output\n"
"nsumline=1                 number of input (xline) cdp's to sum per output\n"
"focdp=1                    1st output inline cdp location relative to input\n"
"foline=1                   1st output line location relative to input\n"
"docdp=1                    output inline cdp increment relative to input\n"
"doline=1                   output line increment relative to input\n"
"nocdp=ncdppline            number of output inline cdp's per line\n"
"noline=nline               number of output lines\n"
"\n"
"Notes:									\n"
"\n"
"Author:	Zhiming Li		      		12-8-97		\n"
"\n";
/**************** end self doc *******************************************/


segytrace tri, tro;

main(int argc, char **argv)
{
	int nt;		/* number of time samples per trace */
	int it;		/* time sample index */
	char *datain, *dataout;
	int ix,iy;
	FILE *infp,*outfp;
	int nfold; 
	float *data;
	float *fold;
	int ncdppline, nline;
	int nsumcdp, nsumline;
	int focdp, foline, docdp, doline, nocdp, noline; 

	int i1, i2, nsegy, iof;
	long iseek;


	
	
	/* hook up getpar */
	initargs(argc, argv);
	askdoc(1);

	/* get required parameters */

	if (!getparstring("datain",&datain)) {
		infp = stdin;
	} else {
		infp = efopen(datain,"r");
	} 
	file2g(infp);

	if (!getparstring("dataout",&dataout)) {
		outfp = stdout;
	} else {
		if(strcmp(dataout,datain)) { 
			outfp = efopen(dataout,"w");
		} else {
			outfp = efopen(dataout,"r+w");
		}
	} 
	file2g(outfp);

	if (!getparint("nfold",&nfold)) err(" must specify nfold ");
	if (!getparint("ncdppline",&ncdppline)) err(" must specify ncdppline ");
	if (!getparint("nline",&nline)) err(" must specify nline ");

	/* get other optional parameters */
	if (!getparint("nsumcdp",&nsumcdp)) nsumcdp = 1;
	if(nsumcdp<1) { warn("nsumcdp reset to 1 \n"); nsumcdp = 1; }
	if(nsumcdp>ncdppline) 
		{ warn("nsumcdp reset to %d \n",ncdppline); nsumcdp = ncdppline; }

	if (!getparint("nsumline",&nsumline)) nsumline = 1;
	if(nsumline<1) { warn("nsumline reset to 1 \n"); nsumline = 1; }
	if(nsumline>nline) 
		{ warn("nsumline reset to %d \n",nline); nsumline = nline; }

	if (!getparint("focdp",&focdp)) focdp = 1;
	if(focdp<1) { warn("focdp reset to 1 \n"); focdp = 1; }
	if(focdp>ncdppline) 
		{ warn("focdp reset to %d \n",ncdppline); focdp = ncdppline; }

	if (!getparint("foline",&foline)) foline = 1;
	if(foline<1) { warn("foline reset to 1 \n"); foline = 1; }
	if(foline>nline) 
		{ warn("foline reset to %d \n",nline); foline = nline; }

	if (!getparint("docdp",&docdp)) docdp = 1;
	if(docdp<1) { warn("docdp reset to 1 \n"); docdp = 1; }
	if(docdp>ncdppline) 
		{ warn("docdp reset to %d \n",ncdppline); docdp = ncdppline; }

	if (!getparint("doline",&doline)) doline = 1;
	if(doline<1) { warn("doline reset to 1 \n"); doline = 1; }
	if(doline>nline) 
		{ warn("doline reset to %d \n",nline); doline = nline; }

	if (!getparint("nocdp",&nocdp)) nocdp = ncdppline;
	if(focdp+(nocdp-1)*docdp>ncdppline) {
		warn("nocdp reset to %d \n",(ncdppline-focdp)/docdp+1);
		nocdp = (ncdppline-focdp)/docdp + 1;
	}

	if (!getparint("noline",&noline)) noline = nline;
	if(foline+(noline-1)*doline>nline) {
		warn("noline reset to %d \n",(nline-foline)/doline+1);
		nocdp = (nline-foline)/doline + 1;
	}

	/* get information from the first header */
	if (!fgettr(infp,&tri)) err("can't get first trace");
	nt = tri.ns;
	nsegy = nt*sizeof(float)+HDRBYTES;

	
	data = (float*) malloc(nt*nfold*sizeof(float));
	fold = (float*) malloc(nt*nfold*sizeof(float));
	bzero(fold,nt*nfold*sizeof(float));
	bzero(data,nt*nfold*sizeof(float));

	/* loop over output traces */
	for(iy=foline-1;iy<foline+(noline-1)*doline;iy+=doline) {
		for(ix=focdp-1;ix<focdp+(nocdp-1)*docdp;ix+=docdp) {
			/* sum traces */
			bzero(fold,nt*nfold*sizeof(float));
			bzero(data,nt*nfold*sizeof(float));
			for (i2=iy-nsumline/2;i2<=iy+nsumline/2;i2++) {
				if(i2>=0 && i2<nline) {
					for (i1=ix-nsumcdp/2;i1<=ix+nsumcdp/2;i1++) {
						if(i1>=0 && i1<ncdppline) {
							iseek = (i1+i2*ncdppline)*nfold;
							iseek = iseek * nsegy + EBCBYTES + BNYBYTES;
							fseek2g(infp,iseek,0);
		/*							
			fprintf(stderr,"l=%d s=%d ls=%d ss=%d \n",iy+1,ix+1,i2+1,i1+1);
		*/
							for(iof=0;iof<nfold;iof++) {
								if (!fgettr(infp,&tri)) 
										err(" get trace error l=%d s=%d o=%d",
											i2+1,i1+1,iof+1);
		/*
			fprintf(stderr,"cdp=%d cdpt=%d \n",tri.cdp, tri.cdpt);
		*/
								for(it=0;it<nt;it++) {
									if(tri.data[it]!=0.) {
										data[iof*nt+it] += tri.data[it]; 
										fold[iof*nt+it] +=1.;
									}
								}
							}
						}
					}
				}
			}
			/* get headers & output*/
			iseek = (ix+iy*ncdppline)*nfold;
			iseek = iseek * nsegy + EBCBYTES + BNYBYTES;
			fseek2g(infp,iseek,0);
			for(iof=0;iof<nfold;iof++) {
				if (!fgettr(infp,&tro)) 
					err(" get trace error l=%d s=%d o=%d",iy+1,ix+1,iof+1);
						
				for(it=0;it<nt;it++) {
					if(fold[it+iof*nt]>0.) {
						tro.data[it] = data[iof*nt+it]/fold[it+iof*nt];
					} else {
						tro.data[it] = 0.;
					}
				}
				fputtr(outfp,&tro);
			}
		}
	}

	free(data);
	free(fold);

	return EXIT_SUCCESS;
}
