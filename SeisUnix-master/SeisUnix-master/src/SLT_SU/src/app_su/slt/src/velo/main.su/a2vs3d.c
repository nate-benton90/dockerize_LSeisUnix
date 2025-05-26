/* ascii to vs3d conversion */

#include "par.h"


char *sdoc[] = {
"									",
" A2VS3D - convert ascii 2-column (t,z) file (checkshot) to VS3D cards	",
"									",
"a2vs3d [parameters] <ascii-file >vs3d-cards				", 
"									",
"Required parameters:						 	",
"s=             inline coordinate    (trace position)			",
"l=             crossline coordinate (line position)			",
"		or							",
"slcard=        =1 inline/crossline coordinates from (s,l,x,y,api) card ",
"               when slcard is not specified, ignore s and l		",
"              								",
"Optional parameters:						 	",
"tscale=1.      scale to be applied to time (first column) of ascii file",
"zscale=1.      scale to be applied to depth (second column) of ascii file",
"               (could be cosine of angle of borehole)			",
"zshift=0.      shift to be added to depth (second column) of ascii file ",
"               (could be rotary table to sea level)			",
"               actuall depth (sea level) = z_ascii*zscale + zshift	", 
"tunit=0        unit of time in ascii file (0=ms 1=s)			",
"type=0         output depth/velocity type in vs3d 			",
"               0=depth 						",
"               1=average velocity					",
"               2=rms velocity 						",
"npmax=1024     maximum number of t-z pairs (number of rows) per location ",
"               in the ascii file					",
"nlocs=1        number of locations					",
"									",
"AUTHOR:		Zhiming Li,       ,	9/28/94   		",
NULL};    

main(int argc, char **argv)
{
    	char *cbuf; 
    	FILE *infp=stdin,*outfp=stdout;
    	float time, depth;
    	float s,l,tscale,zscale,zshift;
    	float  *times, *depths, *velos; 
    	int  *itimes, *ivelos;
	int type,tunit,npmax;
	int ip=0,np,i,loc;
	int slcard=0,nlocs=1;
	float x,y,sr,lr;
	float api; 

    	/* get parameters */
    	initargs(argc,argv);
   	requestdoc(1);

	if (!getparint("slcard",&slcard)) slcard=0;
	if (!getparint("nlocs",&nlocs)) nlocs=1;
	if(slcard==0) {
		if (!getparfloat("s",&s)) err("must specify s");
		if (!getparfloat("l",&l)) err("must specify l");
	}
	if (!getparfloat("tscale",&tscale)) tscale = 1.0;
	if (!getparfloat("zscale",&zscale)) zscale = 1.0;
	if (!getparfloat("zshift",&zshift)) zshift = 0.0;
	if (!getparint("type",&type)) type=0;
	if (!getparint("tunit",&tunit)) tunit=0;
	if (!getparint("npmax",&npmax)) npmax=1024;

/* memory allocation */
    	cbuf = (char*)malloc(81*sizeof(char));

    	times = (float*)malloc(npmax*sizeof(float));
   	depths = (float*)malloc(npmax*sizeof(float));
   	velos = (float*)malloc(npmax*sizeof(float));
   	itimes = (int*)malloc(npmax*sizeof(int));
   	ivelos = (int*)malloc(npmax*sizeof(int));


/*
    fprintf(outfp,
"1--4----------16------24------32------40------48------56------64------72 \n");
    fprintf(outfp,
"CARD           S       L      t1      v1      t2      v2      t3      v3 \n");
    fprintf(outfp,"\n");
*/

	sr = s;
	lr = l;

	for(loc=0;loc<nlocs;loc++) {
    np = 0;
    for (ip=0;ip<npmax;ip++) {
       	if (feof(infp) !=0 ) break;
       	for(i=0;i<81;i++) cbuf[i]=' ';
       	gets(cbuf);
		api = 0.;
		if (slcard==1) {
			sscanf(cbuf,"%f %f %f %f %f \n",&sr,&lr,&x,&y,&api);
			if(api>0 && np>0) break;
		}
		if(api>0 && np==0) {
			s = sr;
			l = lr;
			fprintf(stderr,"%s\n",cbuf);
			continue;
		}
	fprintf(stderr,"%s\n",cbuf);
	time = 0.;
	depth = 0.;
	sscanf(cbuf,"%f %f",&time,&depth);
	if(depth!=0. && time!=0.) {
		if(tunit==0) time=time*0.001;
		times[np] = time;
		depths[np] = depth;
		np = np + 1;
	}
     }
     for(ip=0;ip<np;ip++) {
		times[ip] = times[ip] * tscale;
		depths[ip] = depths[ip] * zscale + zshift;
     }

     if(type==0) {
	for(ip=0;ip<np;ip++) {
		itimes[ip] = 1000.*times[ip]+0.5;
		ivelos[ip] = depths[ip]+0.5;
	}
     } else if(type==1) {
	for(ip=0;ip<np;ip++) { 
		velos[ip] = depths[ip]/times[ip]*2.;
		itimes[ip] = 1000.*times[ip]+0.5;
		ivelos[ip] = velos[ip]+0.5;
        }
     } else if(type==2) {
	velos[0] = depths[0]/times[0]*2.0;
	for(ip=1;ip<np;ip++) { 
		velos[ip] = (depths[ip]-depths[ip-1])/
				(times[ip]-times[ip-1])*2.;
	}
	velos[0] = velos[0]*velos[0]*times[0];
	for(ip=1;ip<np;ip++) {
		velos[ip] = velos[ip-1] + velos[ip]*velos[ip]*
						(times[ip]-times[ip-1]);
	}
	velos[0] = depths[0]/times[0]*2.0;
	itimes[0] = 1000.*times[0]+0.5;
	ivelos[0] = velos[0]+0.5;
	for(ip=1;ip<np;ip++) {
		velos[ip] = sqrt(velos[ip]/times[ip]);
		itimes[ip] = 1000.*times[ip]+0.5;
		ivelos[ip] = velos[ip]+0.5;
        }
     }
     printvs3d(s,l,np,itimes,ivelos,outfp);
     fprintf(stderr,"ASCII to VS3D done for %d t-z pairs at s=%f l=%f \n",
		np,s,l);
     fprintf(stderr,"\n");
	 s = sr;
	 l = lr;
     if (feof(infp) ==0 ) fprintf(stderr,"%s\n",cbuf);
 	}


     free(times);
     free(depths);
     free(velos);
     free(ivelos);
     free(itimes);
     free(cbuf);

     return (0);
}

