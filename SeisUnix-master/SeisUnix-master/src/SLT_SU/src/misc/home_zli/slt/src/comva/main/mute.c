/* MUTE apply mute to traces*/

#include "su.h"
#include "segy.h"

char *sdoc =
"MUTE - apply mute and update headers                                  \n"
"\n"
"mute [parameters] < input.data >muted.data                             \n"
"\n"
"Required parameters:                                                   \n"
" none                                                                  \n"
"\n"
"Optional parameters:                                                   \n"
"mutefile=NONE  file of input mute cards (if no given, mute according   \n"
"                  to the mute time in the trace header)                \n"
"maxpos=128     maximum number of positions (cdp or off) where MUTE cards\n"
"               are specified at mutefile				\n"
"maxtri=128     maximum number of triplets (off-ttp-tbt or cdp-ttp-tbt) \n"
"               per position						\n" 
"zerodead=0     zero trace when trid is not 1	(1=yes 0=no)		\n" 
"mutetaper=0    number of samples to taper the mute zones   		\n"
"mucdtype=0     type of mute card                                       \n"
"       	    mucdtype=0: CARD format: 	                        \n"
"1---5---10---15----21----27----33----39----45----51----57----63----69----75\n"
"MUTE   cdp       off1  ttp1  tbt1  off2  ttp2  tbt2  off3  ttp3  tbt3 	\n"
"       	    mucdtype=1: CARD format: 	                        \n"
"MUTE   off       cdp1  ttp1  tbt1  cdp2  ttp2  tbt2  cdp3  ttp3  tbt3  \n"
"\n"
"\n"
"\n"
"AUTHOR:                Zhiming Li,       ,     8/21/91   \n"
"\n"
"\n"
"NOTE:                                                                  \n"
"                                                                       \n"
" 1. ttpi (i=1,2,...) indicates the ending time of the top-zone mute	\n"
"    	(mute starts at 1st sample of input trace). Blank=0.   		\n"
" 2. tbti (i=1,2,...) indicates the starting time of the bottom-zone mute\n"
"       (mute ends at last sample of input trace).  Blank=max time.	\n"
"                       ends at last sample of input trace).            \n"
" 3. mucdtype indicates whether mute picking is from      	   	\n"
"       cdp (mucdtype=0) or constant-offset section (mucdtype=1).       \n"
" 4. Mute time is either time (in ms) or depth (in m or ft)  		\n"
" 5. Maximum maxpos*maxtri MUTE cards allowed  				\n"
" 6. Mute pattern is as follows:                             		\n"
"                                                                       \n"
"               -------------------------  (offset/cdp)                 \n"
"               |                       |                               \n"
"               |                       |                               \n"
"               |.  TOP MUTE ZONE       |                               \n"
"               | ......                |                               \n"
"               |       .               |                               \n"
"               |        .              |                               \n"
"               |         .......       |                               \n"
"               |                .      |                               \n"
"               |   LIVE ZONE     .. .  |                               \n"
"               |                     . |    INPUT SEISMIC GATHER       \n"
"               |                      .|                               \n"
"               |                       |                               \n"
"               |........               |                               \n"
"               |        .         ..   |                               \n"
"               |         .........  .  |                               \n"
"               |   BOTTOM MUTE ZONE  ..|                               \n"
"               |                       |                               \n"
"               |                       |                               \n"
"  time(depth)  -------------------------                               \n"
"\n"
;
 
segy tr;

main(int argc, char **argv)
{
    char *cbuf; 
    int maxpos, maxtri;	
    int n1, n2, i, nchange, ichange, i1, i2, i3, i4;
    int ic, jc, icmax; 
    char read1[6], read2[7], read3[7], read4[7];
    float *r1, *r2, *r3, *r4;
    string mutefile;
    FILE *infp=stdin,*outfp=stdout,*mfp;
    int inmute;
    int mucdtype, zerodead;
    int *npairs;
    float dt, tmute, tmute_top, tmute_bot;
    int n,nn,i11,i12,i111,i112,i121,i122;
    int itmute,imute,it,idt, nt, itmute_top, itmute_bot;
    float res1, res2, f1prev, f1, f2;
    float t11_top, t12_top ;
    float t11_bot, t12_bot ;
    float tmax,ot;
    int iot, mutetaper,mtend;
    float *taper, temp;

    /* get parameters */
    initargs(argc,argv);
    askdoc(1);
    inmute = 1;
    if (!getparstring("mutefile",&mutefile)) inmute=0;
    if (!getparint("mucdtype",&mucdtype)) mucdtype=0;
    if (!getparint("mutetaper",&mutetaper)) mutetaper=0;
    if (!getparint("maxpos",&maxpos)) maxpos=128;
    if (!getparint("maxtri",&maxtri)) maxtri=128;
    if (!getparint("zerodead",&zerodead)) zerodead=0;

    /* Set up taper weights if tapering requested */
    if (mutetaper>0) {
    	taper = (float*)malloc(mutetaper*sizeof(float));
        for (i = 0; i < mutetaper; ++i) {
            temp = sin((i+1)*3.141592654/(2.*mutetaper));
            taper[i] = temp*temp;
        }
    }
/* make file size to be able to exceed 2 G on convex */
    file2g(infp);
    file2g(outfp);
	
/* read in first trace */
    if (!fgettr(infp,&tr))  err("can't get first trace");
    idt = tr.dt;
    iot = tr.delrt;
    if (idt >=1000) idt = idt/1000;
    dt = idt;
    ot = iot;
/* if depth migrated input, get dz */
    if(tr.dz!=0.) {
	dt = tr.dz;
	ot = tr.fz;
    }
    nt = tr.ns;
    tmax = ot + (nt-1)*dt; 

    cbuf = (char*) malloc(80*sizeof(char));

/* read input mute card file */
    jc = 0;
    if ( inmute == 1 ) {
       mfp = fopen(mutefile,"r");
       /* memory allocation */
       n1 = maxtri;
       n2 = maxpos;
       npairs = (int*)malloc(n2*sizeof(int));
       r1 = (float*)malloc(n2*sizeof(float));
       r2 = (float*)malloc(n1*n2*sizeof(float));
       r3 = (float*)malloc(n1*n2*sizeof(float));
       r4 = (float*)malloc(n1*n2*sizeof(float));

       icmax = maxpos*maxtri;
       ichange  = 0;
       nchange  = 0;

       for (ic=0;ic<icmax;ic++) { 
          if (feof(mfp) !=0 ) break;
          for(i=0;i<80;i++) cbuf[i]=' ';
          fgets(cbuf,80,mfp);
/*
          fprintf(stderr,"%s \n",cbuf);
*/
          if(cbuf[0]=='M' && cbuf[1]=='U' && cbuf[2]=='T' && cbuf[3]=='E') {
             strncpy(read1,&cbuf[6],5);
	     read1[5] = '\0';
	     i1 = atoi(read1);
/*	     
	     fprintf(stderr,"read1=%s i1=%d \n",read1,i1);
*/
	     if (ichange == 0 ) ichange = i1 ;
	     if (i1 != ichange && i1 !=0 && ichange != 0 ) {
/*
	     fprintf(stderr,"change at =%d ntris=%d \n",ichange, jc);
*/
	        npairs[nchange] = jc;
	        r1[nchange] = ichange ;
	        nchange = nchange + 1;
	        jc = 0;
	        ichange = i1;
	     }
	     for(i=0;i<3;i++)
	        {
	        strncpy(read2,&cbuf[15+i*18],6);
	        read2[6] = '\0';
	        i2 = atoi(read2);
	        strncpy(read3,&cbuf[21+i*18],6);
	        read3[6] = '\0';
	        i3 = atoi(read3);
	        strncpy(read4,&cbuf[27+i*18],6);
	        read4[6] = '\0';
	        i4 = atoi(read4);
		if(strncmp(read3, "      ",6)==0) i3 = 0;
		if(strncmp(read4, "      ",6)==0 || 
		   strncmp(read4, "\n",1)==0) {
			i4 = tmax;
		}
	     /*
	     fprintf(stderr,"read2=%s i2=%d \n",read2,i2);
	     fprintf(stderr,"read3=%s i3=%d \n",read3,i3);
	     fprintf(stderr,"read4=%s i4=%d \n",read4,i4);
	     */
		
	        if (read2[5] == ' ' || read2[5] == '\0') break; 
	        r2[nchange*n1+jc] = i2;
	        r3[nchange*n1+jc] = i3;
	        r4[nchange*n1+jc] = i4;
	        jc = jc + 1;
	     }
	  }
	  if(nchange>maxpos) break;
       }
       r1[nchange] = ichange;
       npairs[nchange] = jc;
       nchange = nchange + 1;	

/*
      fprintf(stderr,"nchange=%d \n",nchange);
       for(i1=0;i1<nchange;i1++) { 
	  fprintf(stderr,"r1=%f \n",r1[i1]);
	  fprintf(stderr,"npairs=%d \n",npairs[i1]);
	  for(i2=0;i2<npairs[i1];i2++) {
	     fprintf(stderr,"r2=%f r3=%f r4=%f \n",
		r2[i1*n1+i2],r3[i1*n1+i2],r4[i1*n1+i2]);
	  }
       }
*/
	
    }

    /* main loop over input traces */
    f1prev = -9999; 
    do { 
	/* get mute from mutefile and update trace header */
	if ( inmute == 1 ) {
	   if( mucdtype == 1 ) {
              f1 = tr.offset;
	      f2 = tr.cdp;
	   } else {
	      f1 = tr.cdp;
	      f2 = tr.offset;
	   }
	   /* search 1st (offset/cdp) index */
	   if(f1 != f1prev) { 
	      f1prev = f1;
	      n = nchange;
	      nn = 1;
	      bisear_(&n,&nn,r1,&f1,&i1);
	      if (i1<1 || f1 <r1[0]) {
	         i11 = 0; i12 = 0; res1 = 0.;
	      } 
	      else if(i1>=n) {
	         i11 = n-1; i12 = n-1; res1 = 0.;
	      }
	      else {
	         i11 = i1-1; i12 = i1; res1 = (f1-r1[i11])/(r1[i12]-r1[i11]);
	      }
	   }
	   /* search 2nd (cdp/offset) index */
	   n = npairs[i11];
	   nn = 1;
	   bisear_(&n,&nn,r2+i11*n1,&f2,&i2); 
	   if (i2<1 || f2 < r2[i11*n1] ) {
	     t11_top = r3[i11*n1];
	     t11_bot = r4[i11*n1];
	   } else if(i2>=n) {
	     t11_top = r3[i11*n1+n-1];
	     t11_bot = r4[i11*n1+n-1];
	   } else {
	     i111 = i11*n1+i2-1; i112 = i11*n1+i2; 
	     res2 = (f2 - r2[i111])/(r2[i112]-r2[i111]);
	     t11_top = r3[i111]+res2*(r3[i112]-r3[i111]);
	     t11_bot = r4[i111]+res2*(r4[i112]-r4[i111]);
	   }
	   n = npairs[i12];
	   nn = 1;
	   bisear_(&n,&nn,r2+i12*n1,&f2,&i2);
	   if (i2<1 || f2 < r2[i12*n1]) {
	     t12_top = r3[i12*n1];
	     t12_bot = r4[i12*n1];
	   } else if(i2>=n) {
	     t12_top = r3[i12*n1+n-1];
	     t12_bot = r4[i12*n1+n-1];
	   } else {
	     i121 = i12*n1+i2-1; i122 = i12*n1+i2; 
	     res2 = (f2 - r2[i121])/(r2[i122]-r2[i121]);
	     t12_top = r3[i121]+res2*(r3[i122]-r3[i121]);
	     t12_bot = r4[i121]+res2*(r4[i122]-r4[i121]);
	   }

	   tmute_top = t11_top + res1*(t12_top-t11_top);
	   itmute_top = tmute_top;
	   tmute_bot = t11_bot + res1*(t12_bot-t11_bot);
	   itmute_bot = tmute_bot;

/*
	   fprintf(stderr,"offset=%d itmute_top=%d itmute_bot=%d  \n", 
			tr.offset, itmute_top, itmute_bot);
*/

        } else {
	/* apply mute in the trace header */
	   itmute_top = tr.mute;
	   itmute_bot = tr.mutb;
	   if(itmute_bot==0) itmute_bot=tmax;
        }
	/* updating mute time and apply mute */
	   if(itmute_top<ot) itmute_top=ot;
	   if(itmute_top>tmax) itmute_top=tmax;
	   if(itmute_bot<ot) itmute_bot=ot;
	   if(itmute_bot>tmax) itmute_bot=tmax;

	   tr.muts = 0;
	   tr.mute = itmute_top;
	   tr.mutb = itmute_bot;

	   /* top-zone mute */
	   tmute = (itmute_top - ot)/dt;
	   imute = tmute;
/*
	   fprintf(stderr,"itmute_top=%d imute=%d  \n", itmute_top, imute);
*/

	   for(it=0;it<imute;it++) tr.data[it] = 0.;
	      /* tapering */
	   if(mutetaper>0) {
	   	mtend=((mutetaper+imute)<(int)tr.ns)?mutetaper:(int)tr.ns-imute;
	   	for(it=0;it<mtend;it++) tr.data[it+imute] *= taper[it];
	   }

	   /* bottom-zone mute */
	   tmute = (itmute_bot - ot)/dt;
	   imute = tmute;

/*
	   fprintf(stderr,"itmute_bot=%d imute=%d  \n", itmute_bot, imute);
*/

	   for(it=imute;it<nt;it++) tr.data[it] = 0.;
	      /* tapering */
	   if( mutetaper > 0 ) {
	   	mtend = ((imute-mutetaper)>0)?mutetaper:imute;
	   	for(it=0;it<mtend;it++) tr.data[imute-it] *= taper[it];
	   }

	   if(tr.trid!=1 && zerodead==1) 
		for(it=0;it<nt;it++) tr.data[it] = 0.;

	   fputtr(outfp,&tr);
       }while(fgettr(infp,&tr));

    fclose(outfp);
    free(r1);
    free(r2);
    free(r3);
    free(r4);
    if (mutetaper>0) free(taper);
    return EXIT_SUCCESS;
}
