/* suapstat.c */
/* B.Nemeth */

#include "suhdr.h"

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUAPSTAT - Apply residual static correction                           ",
"                                                                       ",
" suapstat <stdin >stdout                                               ",
"                                                                       ",
" Required parameters:                                                  ",
"        none                                                           ",
"                                                                       ",
" Optional parameters:                                                  ",
"      st=\"sh.st\"		shot static file                        ",
"      rt=\"rv.st\"		receiver static file                    ",
"      sign=1			direction of applying the shifts        ",
"      maxshft=0.100            maximum allowed shift                   ",
"      zero=0                   1 put zero where abs(shift) > maxshift  ",
"      nst=3000                 max number of shot static values        ",
"      nrt=3000                 max number of receiver static values    ",
NULL};

/* Segy data constans */
segy tr,outtr;;		/* SEGY trace */
float *data;		/* trace data */


int main( int argc, char *argv[] )
{
	int sign;		/* sign of static shift */
	int ns;			/* number of samples */
	float dt;		/* sample interval */
	float *tout;		/* output trace */

	char *st="";		/* shot static file name */
	char *rt="";		/* receiver static file name */
	int nst;
	int nrt;
	float **sta;
	float **rta;
	int i;
	
	FILE *stf, *rtf;	/* shot end receiver static files */
	int rv;
	int sh;
	int s_sh;
	int s_rv;
	float stv;
	float rtv;
	float stat;
	float tmin;
	int itime;
	int itr=0;
	int sshft;
	float maxshft;
	int imaxshft;
	int missing;
	int zero;
	float junk;
	
	initargs(argc, argv);
   	requestdoc(1);

	if( !getparstring("st",&st)) st="sh.st";
	if( !getparstring("rt",&rt)) rt="rv.st"; 
	if(!getparint("sign",&sign)) sign=1;
	if(!getparint("nst",&nst)) nst=3000;
	if(!getparint("nrt",&nrt)) nrt=3000;
	if(!getparfloat("maxshft",&maxshft)) maxshft=0.10;
	if(!getparint("zero",&zero)) zero=0;

	stf = efopen(st,"r");
	rtf = efopen(rt,"r");
	
	/* allocate array for static values */
	sta = ealloc2float(2,nst); 
	rta = ealloc2float(2,nrt);
	
	/* read statics into the array */
	i=0;
	do {
		fscanf(stf," %f %f %*f",&sta[i][0],&sta[i][1]);
		i++;
	} while(!feof(stf));
	fprintf(stderr," Number of shot statics: %d\n",i-1);
	nst=i-1; 
	
	i=0;
	do {
		fscanf(rtf," %f %f %*f",&rta[i][0],&rta[i][1]);
		i++;
	} while(!feof(rtf));
	fprintf(stderr," Number of receiver  statics: %d\n",i-1);
	nrt=i-1; 
	fclose(stf);
	fclose(rtf); 

        /* get information from the first header */
        if (!gettr(&tr)) err("can't get first trace");
	ns=tr.ns;
	dt   = ((double) tr.dt)/1000000.0;
	tout = ealloc1float(ns);
	imaxshft=NINT(maxshft/dt);
	
		/* loop over traces */
	do {
		
		missing=0;
		memset( (void *) outtr.data, (int) '\0', ns*FSIZE);
		tmin = tr.delrt/1000.0;

		/* Shot statics */
	    	sh = tr.ep;
		i=0;
		do {
			s_sh=(int)sta[i][0];
			if(i==nst) {
				warn(" Shot %d not on shot static file\n"
					,sh);
				missing=1;
			}
			i++;
		} while(s_sh!=sh && !missing);
		if(missing) {
			stv=0.0;
			missing=0;
		} else {
			stv=sta[i-1][1];
		}
		
		/* Receiver statics */
	    	rv = tr.sdepth;
		i=0;
		do {
			s_rv=(int)rta[i][0];
			if(i==nrt) { 
				warn (" Receiver %d not on receiver static file\n"
					,rv);
				missing=1;
			}
			i++;
		} while(s_rv!=rv && !missing);
		if(missing) {
			rtv=0.0;
			missing=0;
		} else {
			rtv=rta[i-1][1];
		}
		
/*		rtv /= -1000.0;
		stv /= -1000.0;
*/		
		stat = sign*( rtv + stv);
		sshft = NINT(stat/dt);
		if(imaxshft<abs(sshft)) {
			if(zero) {
				/*fprintf(stderr," %d <- %d\n",sshft,0); */
				sshft=0;
			} else {
				/*fprintf(stderr," %d <- %d\n",sshft,ISGN(sshft)*imaxshft); */
				sshft=ISGN(sshft)*imaxshft;
			}
		}
		
/*		fprintf(stderr," %d\n",sshft); */
		
		
		/* copy headers over to the new trace */
  		memcpy((void*) &outtr, (const void *) &tr,
			(size_t) (sizeof(segy)-SU_NFLTS*FSIZE));  
  
  		/* copy the data with the right amount of shift */
		if (sshft <= 0) {
			memcpy((void*) &(outtr.data)[0], (const void *)
				&(tr.data)[abs(sshft)],
			(size_t) (ns-abs(sshft))*FSIZE);
		} else {
			memcpy((void*) &(outtr.data)[abs(sshft)], (const void *)
			&(tr.data)[0],
			(size_t) (ns-abs(sshft))*FSIZE);
		}
			  
  
		/* set header values */
		outtr.sstat += sign*(1000.0 * stv);
                outtr.gstat += sign*(1000.0 * rtv);
                outtr.tstat += sign*(1000.0 * stat);
 
		puttr(&outtr);
		itr++;
		if(itr%500==0) fprintf(stderr,"Number of traces corrected = %10d\n",
			itr);
		
	} while(gettr(&tr));
	free2float(sta);
	free2float(rta);
	
        return EXIT_SUCCESS;
}
