/* suapstat.c */
/* B.Nemeth */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "su.h"
#include "segy.h"

/*********************** self documentation *****************************/
char *sdoc[] = {
" SUMEGESTAT - Merge static corrections files                           ",
"                                                                       ",
" suapstat        >stdout                                               ",
"                                                                       ",
" Required parameters:                                                  ",
"        st1=                   shot static file 1                      ",
"        rt1=                   receiver static file 1                  ",
"        st2=                   shot static file 2                      ",
"        rt2=                   receiver static file 2                  ",
"        sto=                   merged shot static file                 ",
"        rto=                   merged receiver static file             ",
"                                                                       ",
"      Optional parameters:                                             ",
"      nst=3000                 max number of shot static values        ",
"      nrt=3000                 max number of receiver static values    ",
NULL};


int main( int argc, char *argv[] )
{
	char *st1="";		/* shot static file name */
	char *rt1="";		/* receiver static file name */
	char *st2="";		/* shot static file name */
	char *rt2="";		/* receiver static file name */
	char *sto="";		/* shot static file name */
	char *rto="";		/* receiver static file name */
	int nst;
	int nrt;
	int i;
	int s_sh,s_rv;
	int sh,rv;
	float st;
	float xcor;
	float **sta,**rta;
	
	FILE *stf1, *rtf1;	/* shot end receiver static files */
	FILE *stf2, *rtf2;	/* shot end receiver static files */
	FILE *stfo, *rtfo;	/* shot end receiver static files */
	initargs(argc, argv);
   	requestdoc(1);

	MUSTGETPARSTRING("st1",&st1);
	MUSTGETPARSTRING("rt1",&rt1);
	MUSTGETPARSTRING("st2",&st2);
	MUSTGETPARSTRING("rt2",&rt2);
	MUSTGETPARSTRING("sto",&sto);
	MUSTGETPARSTRING("rto",&rto);
	if(!getparint("nst",&nst)) nst=3000;
	if(!getparint("nrt",&nrt)) nrt=3000;

	stf1 = efopen(st1,"r");
	rtf1 = efopen(rt1,"r");
	
	stf2 = efopen(st2,"r");
	rtf2 = efopen(rt2,"r");
	
	stfo = efopen(sto,"w");
	rtfo = efopen(rto,"w");
	
	/* allocate array for static values */
	sta = ealloc2float(2,nst); 
	rta = ealloc2float(2,nrt);
	
	/* read statics into the array */
	i=0;
	do {
		fscanf(stf2," %f %f %*f",&sta[i][0],&sta[i][1]);
		i++;
	} while(!feof(stf2));
	fprintf(stderr," Number of shot statics: %d\n",i-1);
	nst=i-1; 
	
	i=0;
	do {
		fscanf(rtf2," %f %f %*f",&rta[i][0],&rta[i][1]);
		i++;
	} while(!feof(rtf2));
	fprintf(stderr," Number of receiver  statics: %d\n",i-1);
	nrt=i-1; 
	
	fclose(stf2);
	fclose(rtf2); 

	/* loop over shot file 1 */
	fscanf(stf1," %d %f %f",&sh,&st,&xcor);
	do {
		i=0;
		do {
			s_sh=(int)sta[i][0];
			if(i==nst) err(" Shot files have different shot entries\n");
			i++;
		} while(sh!=s_sh);
		fprintf(stfo," %d %f %f\n",sh,st+sta[i-1][1],xcor);
		
		fscanf(stf1," %d %f %f",&sh,&st,&xcor);
	} while(!feof(stf1));
	
	/* loop over receiver file 1 */
	fscanf(rtf1," %d %f %f",&rv,&st,&xcor);
	do {
		i=0;
		do {
			s_rv=(int)rta[i][0];
			if(i==nrt) err(" Receiver files have different shot entries\n");
			i++;
		} while(rv!=s_rv);
		fprintf(rtfo," %d %f %f\n",rv,st+rta[i-1][1],xcor);
		
		fscanf(rtf1," %d %f %f",&rv,&st,&xcor);
	} while(!feof(rtf1));
	
	free2float(sta);
	free2float(rta);
	fclose(stf1);
	fclose(rtf1); 
	fclose(stfo);
	fclose(rtfo); 
	
        return EXIT_SUCCESS;
}
