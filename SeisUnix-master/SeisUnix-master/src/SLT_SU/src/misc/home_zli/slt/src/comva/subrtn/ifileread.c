#include "comva.h"

/* read in ifile or hfile */
/* input:								*/
/*	ifilefp			file pointer to VELO card dataset	*/
/*	maxhs 			maximum number of horizons 		*/
/*	maxpks 			maximum number of picks per horizon 	*/
/* output:								*/
/*	nhs			number of horizon/interface found	*/
/*	xpicks[maxhs][maxpks]	x position of picks			*/	
/*	zpicks[maxhs][maxpks]	z position of picks			*/	
/*	veltop[maxhs][maxpks]	velocity at top 			*/	
/*	velbot[maxhs][maxpks]	velocity at bottom 			*/	
/*	difs[maxhs]		different segment mark 			*/	
/*	dens[maxhs][maxpks]	density at picks 			*/	
/*	qval[maxhs][maxpks]	Q values at picks 			*/	
/*	pois[maxhs][maxpks]	Poison ratio at picks 			*/	
/*	velavgs[maxhs][maxpks]	average velocity at picks 		*/	
/*	dvdzs[maxhs][maxpks]	interval velocity gradient at picks 	*/	
/*	hnames[maxhs][80]	names of interfaces/horizons  		*/	
/*	hnums[maxhs]		interfaces/horizons numbers 		*/	
/*	npicks[maxhs]		number of picks per interfaces/horizons	*/	
/*	xmin			minimum x position			*/
/*	xmax			maximum x position			*/
/*	zmin			minimum z position			*/
/*	zmax			maximum z position			*/
/*	cdpxmin			cdp number at minimum x position	*/
/*	cdpxmax			cdp number at maximum x position	*/
/*	vmin			minimum velocity 			*/
/*	vmax			maximum velocity 			*/
/*	vtype			velocity type 				*/
/*	dunits			distance units 				*/
/*	zattrib			z attribute 				*/
/*	dcdp			cdp spacing 				*/
/*	ihfile			IFILE/HFILE type (0=IFILE 1=HFILE)	*/
/*									*/
/* author:	Zhiming Li		      		11/91		*/

void ifileread(FILE *ifilefp, int *nhs, float *xpicks, float *zpicks,
	       float *veltop, float *velbot, int *difs,
	       float *dens, float *qval, float *pois,
	       float *velavgs, float *dvdzs, char *hnames,
	       int *hnums, int *npicks, int maxhs, int maxpks,
	       float *xmin, float *xmax, float *zmin, float *zmax,
	       int *cdpxmin, int *cdpxmax, float *vmin, float *vmax,
	       char *vtype, char *dunits, char *zattrib, float *dcdp,
	       int *ihfile) {

	int ih, ip, i, ic, ib;
	char *buf;

	buf = (char *) malloc(81*sizeof(char));

	*nhs = -1;
	*ihfile = 0;
	ip = 0;
	for(ih=0;ih<maxhs;ih++) npicks[ih] = 0;
	
	/* main loop of reading ifile */
	do {
		if(feof(ifilefp) !=0) break;
		bzero(buf,81);
		if(fgets(buf,81,ifilefp)==NULL) break;

		if(strncmp(buf, "HFILE",5)==0) {
			*ihfile = 1;
		} else if(strncmp(buf, "MODEL.X-MIN",11)==0) {
		 	sscanf(buf+20,"%f",xmin);	
		} else if(strncmp(buf, "MODEL.X-MAX",11)==0) {
		 	sscanf(buf+20,"%f",xmax);	
		} else if(strncmp(buf, "MODEL.Z-MIN",11)==0) {
		 	sscanf(buf+20,"%f",zmin);	
		} else if(strncmp(buf, "MODEL.Z-MAX",11)==0) {
		 	sscanf(buf+20,"%f",zmax);	
		} else if(strncmp(buf, "MODEL.CDP#-AT-X-MIN",19)==0) {
		 	sscanf(buf+20,"%d",cdpxmin);	
		} else if(strncmp(buf, "MODEL.CDP#-AT-X-MAX",19)==0) {
		 	sscanf(buf+20,"%d",cdpxmax);	
		} else if(strncmp(buf, "MODEL.MIN-VELOCITY",18)==0) {
		 	sscanf(buf+20,"%f",vmin);	
		} else if(strncmp(buf, "MODEL.MAX-VELOCITY",18)==0) {
		 	sscanf(buf+20,"%f",vmax);	
		} else if(strncmp(buf, "MODEL.VELOCITY-TYPE",19)==0) {
		 	sscanf(buf+21,"%s",vtype);	
		} else if(strncmp(buf, "MODEL.DIST-UNITS",16)==0) {
		 	sscanf(buf+21,"%s",dunits);	
		} else if(strncmp(buf, "MODEL.Z-ATTRIBUTE",17)==0) {
		 	sscanf(buf+21,"%s",zattrib);	
		} else if(strncmp(buf, "MODEL.CDP-SPACING",17)==0) {
		 	sscanf(buf+20,"%f",dcdp);	
		} else if(strncmp(buf, "HORIZON-NUMBER",14)==0) {
			*nhs += 1;
			sscanf(buf+15,"%d",&hnums[*nhs]);
			if(ip>0) npicks[*nhs-1] = ip; 
			if(ip>maxpks)
				err("number of picks exceeds %d \n", maxpks);
			ip = 0;
		} else if(strncmp(buf, "HORIZON-NAME",12)==0) {
			sscanf(buf+17,"%s",&hnames[(*nhs)*80]);
		} else if(strncmp(buf, "*",1)==0 ||
			  strncmp(buf,"                ",16)==0 ||
			  strncmp(buf," \n",2)==0 ||
			  strncmp(buf,"\n",1)==0 ||
			  strncmp(buf,"MODEL",5)==0 ||
			  strncmp(buf,"IFILE",5)==0 ||
			  strncmp(buf,"HFILE",5)==0 ) {
			continue;
		} else if(strncmp(buf, " ",1)==0) {
			sscanf(buf,"%f %f %f %f %d %f %f %f %f %f",
		   		&xpicks[(*nhs)*maxpks+ip],
			      	&zpicks[(*nhs)*maxpks+ip],
				&veltop[(*nhs)*maxpks+ip],
				&velbot[(*nhs)*maxpks+ip],
				&difs[(*nhs)*maxpks+ip],
				&dens[(*nhs)*maxpks+ip],
				&qval[(*nhs)*maxpks+ip],
				&pois[(*nhs)*maxpks+ip],
				&velavgs[(*nhs)*maxpks+ip],
				&dvdzs[(*nhs)*maxpks+ip]);

			ip +=1;
		}

	} while( *nhs < maxhs );

	
	if(ip>0) {
		npicks[*nhs] = ip; 
		*nhs += 1;
	}

	free(buf);

	fseek(ifilefp,0,0);
}
